#include <signal.h>

#include "thread.h"
#include "client.h"

static pthread_mutex_t	g_mutex;

//thread
static void	*clnt_thread(void *arg)
{
	int	sock;
	t_tid	*tid;
	char	*out_buf;

	tid =  (t_tid *)arg;
	//mutex lock. change free flag.
	pthread_mutex_lock(&g_mutex);
	tid->free = 0;
	//mutex unlock
	pthread_mutex_unlock(&g_mutex);
	//send data
	if ((sock = TCPconnector(tid->port, tid->dest_addr)) < 0)
		exit(1);
	printf("send:%s", tid->buffer);
	if (send(sock, tid->buffer, BUFFER_SIZE, 0) < 0)
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
	tid->free = 1;
	free(tid->buffer);
	tid->buffer = 0;
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
		tid_lst[i].buffer = 0;
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

static int	child_proc(in_addr_t dest, int port_num, int *fd)
{
	int	t_idx;
	char	*buffer;
	t_tid	tid[THREAD_POOL_SIZE];

	if (!(buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE)))
	{
		perror("malloc error");
		return 1;
	}
	close(fd[1]);
	//copy fd
	dup2(fd[0], 0);
	//init
	if (init_(&g_mutex, tid) < 0)
	{
		perror("mutex allocate error");
		return 1;
	}
	memset(buffer, 0, BUFFER_SIZE);
	while (read(0, buffer, BUFFER_SIZE) != EOF)
	{
		printf("%s\n", buffer);
		//exit when type 'stop'
		if (!strcmp(buffer, "stop\n") || !strcmp(buffer, "exit\n"))
			break;
		//wait for thread is free
		t_idx = pigeon_hole(tid);
		//make send thread
		tid[t_idx].dest_addr = dest;
		tid[t_idx].port = port_num;
		tid[t_idx].buffer = buffer;
		if ((pthread_create(&(tid[t_idx].id), NULL, clnt_thread, &tid[t_idx])))
		{
			perror("failed to create thread.");
			return 1;
		}
		pthread_detach(tid[t_idx].id);
		if (!(buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE)))
		{
			perror("malloc error");
			return 1;
		}
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
