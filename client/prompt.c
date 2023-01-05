#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

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
	dprintf(2, "childThread: %d\n", getpid());
	
	tid->free = 0;
	if ((sock = TCPconnector(tid->port, tid->dest_addr)) < 0)
		exit(1);
	//make http header
	tmp.s_addr = tid->dest_addr;
	send_buf = head_builder(tid->method, tid->url, inet_ntoa(tmp));
	
	dprintf(2, "send:%s", send_buf);
	//mutex unlock
	pthread_mutex_unlock(&g_mutex);
	//send data
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
	//mutex lock
	pthread_mutex_lock(&g_mutex);
	printf("receive:%s", out_buf);
	//clear
	tid->free = 1;
	free(out_buf);
	free(tid->method);
	free(tid->url);
	tid->method = 0;
	tid->url = 0;
	//mutex unlock
	pthread_mutex_unlock(&g_mutex);
	close(sock);
	pthread_exit(NULL);
	return 0;
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

static void	child_proc(in_addr_t dest, int port_num, int *fd)
{
	int	result;

	int	t_idx;
	char	*buffer;
	t_tid	tid[THREAD_POOL_SIZE];

	//delay
	sleep(1);
	if (!(buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE)))
	{
		perror("malloc error");
		exit(1);
	}
	memset(buffer, 0, BUFFER_SIZE);
	close(fd[1]);
	//copy fd
	dup2(fd[0], 0);
	//init
	if (init_(&g_mutex, tid) < 0)
	{
		perror("mutex allocate error");
		exit(1);
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
			exit(1);
		if (!result)
		{
			perror("wrong form. please input 'method-url' form\n");
			memset(buffer, 0, BUFFER_SIZE);
			continue;
		}
		//thread start
		if ((pthread_create(&(tid[t_idx].id), NULL, clnt_thread, &tid[t_idx])))
		{
			perror("failed to create thread.");
			exit(1);
		}
		pthread_detach(tid[t_idx].id);
		memset(buffer, 0, BUFFER_SIZE);
	}
	close(fd[0]);
	free(buffer);
	pthread_mutex_destroy(&g_mutex);
	exit(0);
}

static void	parent_proc(int *fd)
{
	char	*buffer;
	ssize_t	size_;
	
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
		size_ = read(0, buffer, BUFFER_SIZE);
		size_ = write(1, buffer, size_);
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

int	main_prompt(in_addr_t dest, int port_num)
{
	pid_t	pid;
	int	fd[2];
	int	wstatus;

	//init pipe
	wstatus = 0;
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
		dprintf(2, "childPid: %d\n", getpid());
		child_proc(dest, port_num, fd);
	}
	//parent process(prompt)
	else
	{
		dprintf(2, "parentPid: %d\n", getpid());
		parent_proc(fd);
		waitpid(pid, &wstatus, WUNTRACED);
	}
	return status_control(wstatus);
}
