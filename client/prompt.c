#include <signal.h>

#include "thread.h"
#include "client.h"
#include "html.h"

static pthread_mutex_t	g_mutex;

//thread
static void	*clnt_thread(void *arg)
{
	int	sock;
	t_tid	*tid;
	char	*send_buf;
	char	*out_buf;

	struct in_addr	tmp;

	tid =  (t_tid *)arg;
	//mutex lock. change free flag.
	pthread_mutex_lock(&g_mutex);
	
	tid->free = 0;
	if ((sock = TCPconnector(tid->port, tid->dest_addr)) < 0)
		exit(1);
	//make http header
	tmp.s_addr = tid->dest_addr;
	send_buf = head_builder(tid->method, tid->url, inet_ntoa(tmp));
	
	//mutex unlock
	pthread_mutex_unlock(&g_mutex);
	
	//send data
	printf("send:%s", send_buf);
	if (send(sock, send_buf, BUFFER_SIZE, 0) < 0)
	{
		perror("send failed");
		exit(1);
	}
	
	//malloc outbuffer
	if (!(out_buf = (char *)malloc(sizeof(char) * BUFFER_SIZE)))
	{
		perror("malloc error");
		exit(1);
	}
	memset(out_buf, 0, BUFFER_SIZE);
	
	//receive
	if (recv(sock, out_buf, BUFFER_SIZE, 0) < 0)
	{
		perror("receive failed");
		exit(1);
	}
	printf("receive:%s", out_buf);
	
	free(out_buf);
	//mutex lock
	pthread_mutex_lock(&g_mutex);
	//clear
	tid->free = 1;
	free(tid->method);
	free(tid->url);
	tid->method = 0;
	tid->url = 0;
	//mutex unlock
	pthread_mutex_unlock(&g_mutex);
	close(sock);
	pthread_exit(NULL);
}

static int	init_(pthread_mutex_t *mutex, t_tid *tid_lst)
{
	//init mutex
	if (pthread_mutex_init(mutex, NULL))
	{
		perror("mutex init failed");
		return -1;
	}
	//init tid lst
	for (int i = 0; i < THREAD_POOL_SIZE; i++)
	{
		tid_lst[i].idx = i;
		tid_lst[i].free = 1;
		tid_lst[i].method = 0;
		tid_lst[i].url = 0;
	}
	return 1;
}

static int	pigeon_hole(const t_tid *tid)
{
	int	idx;

	idx = 0;
	while (1)
	{
		idx = 0;
		while (!(tid[idx].free) && idx < THREAD_POOL_SIZE)
			idx++;
		if (idx < THREAD_POOL_SIZE)
			break;
		sleep(1);
	}
	return idx;
}

static int	set_tid(t_tid *tid, in_addr_t dest, int port, char *buffer)
{
	char	*tok;
	char	*tmp[2];
	char	*d_idx;
	char	*o_idx;
	size_t	size;

	tmp[0] = 0;
	tmp[1] = 0;
	//make send thread
	tid->dest_addr = dest;
	tid->port = port;
	//split
	tok = strtok(buffer, "-");
	for (int i = 0; tok != NULL; i++)
	{
		size = strlen(tok) + 1;
		tmp[i] = (char *)malloc(sizeof(char) * size);
		memset(tmp[i], 0, size);
		if (!tmp[i])
		{
			perror("malloc error. ");
			return -1;
		}
		d_idx = tmp[i];
		o_idx = tok;
		while (*o_idx != '\0' && *o_idx != '\n')
		{
			*d_idx = *o_idx;
			o_idx++;
			d_idx++;
		}
		tok = strtok(NULL, "-");
	}
	if (!tmp[0] || !tmp[1])
		return 0;
	tid->method = tmp[0];
	tid->url = tmp[1];
	return 1;
}

static int	child_proc(in_addr_t dest, int port_num, int *fd)
{
	int	result;

	int	t_idx;
	char	*buffer;
	t_tid	tid[THREAD_POOL_SIZE];

	if (!(buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE)))
	{
		perror("malloc error");
		return 1;
	}
	memset(buffer, 0, BUFFER_SIZE);
	close(fd[1]);
	//copy fd
	dup2(fd[0], 0);
	//init
	if (init_(&g_mutex, tid) < 0)
	{
		perror("mutex allocate error");
		return 1;
	}
	while (read(0, buffer, BUFFER_SIZE) != EOF)
	{
		//exit when type 'stop'
		if (!strcmp(buffer, "stop\n") || !strcmp(buffer, "exit\n"))
			break;
		//wait for thread is free
		t_idx = pigeon_hole(tid);
		//make send thread
		result = set_tid(&tid[t_idx], dest, port_num, buffer);
		if (result < 0)
			return 1;
		if (!result)
		{
			perror("wrong form. please input 'method-url' form\n");
			return 1;
		}
		//thread start
		if ((pthread_create(&(tid[t_idx].id), NULL, clnt_thread, &tid[t_idx])))
		{
			perror("failed to create thread.");
			return 1;
		}
		pthread_detach(tid[t_idx].id);
		memset(buffer, 0, BUFFER_SIZE);
	}
	close(fd[0]);
	return 0;
}

static void	parent_proc(int *fd)
{
	char	*buffer;
	
	if (!(buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE)))
	{
		perror("malloc error");
		close(fd[1]);
		return;
	}
	close(fd[0]);
	//copy fd
	dup2(fd[1], 1);
	while (1)
	{
		memset(buffer, 0, BUFFER_SIZE);
		read(0, buffer, BUFFER_SIZE);
		write(1, buffer, BUFFER_SIZE);
		//exit when type 'exit'
		if (!strcmp(buffer, "exit\n"))
			break;
	}
	//free
	close(fd[1]);
	free(buffer);
}

static int	status_control(int status)
{
	int	signal;

	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
	{
		signal = WTERMSIG(status);
		return (128 + signal);
	}
	return (status);
}

static void	parent_wait(pid_t pid, int *status)
{
	waitpid(pid, status, WUNTRACED);
	//wait until exited or signaled
	while (!WIFEXITED(*status) && !WIFSIGNALED(*status))
		waitpid(pid, status, WUNTRACED);
}

int	main_prompt(in_addr_t dest, int port_num)
{
	pid_t	pid;
	int	fd[2];
	int	wstatus;

	//init pipe
	if (pipe(fd) == -1)
	{
		perror("pipe error");
		exit(1);
	}
	//fork
	pid = fork();
	if (pid == -1)
	{
		perror("fork error");
		exit(1);
	}
	//child process(thread manager)
	else if (pid == 0)
	{
		exit(child_proc(dest, port_num, fd));
	}
	//parent process(prompt)
	else
	{
		parent_proc(fd);
		parent_wait(pid, &wstatus);
		pthread_mutex_destroy(&g_mutex);
	}
	return status_control(wstatus);
}
