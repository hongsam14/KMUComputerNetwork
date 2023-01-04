#include "client.h"
#include "thread.h"

static pthread_mutex_t	g_mutex;

//thread
static void	*clnt_thread(void *arg)
{
	t_tid	*tid;
	char	*out_buf;

	tid =  (t_tid *)arg;
	//mutex lock. change free flag.
	pthread_mutex_lock(&g_mutex);
	tid->free = 0;
	//mutex unlock
	pthread_mutex_unlock(&g_mutex);
	//send data
	if (send(tid->clnt_sock, tid->buffer, tid->buf_size, 0) < 0)
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
	if (recv(tid->clnt_sock, out_buf, BUFFER_SIZE, 0) < 0)
	{
		perror("receive failed");
		exit(1);
	}
	printf("receive:%s\n", out_buf);
	free(out_buf);
	//mutex lock
	pthread_mutex_lock(&g_mutex);
	tid->free = 1;
	tid->buf_size = 0;
	free(tid->buffer);
	tid->buffer = 0;
	//mutex unlock
	pthread_mutex_unlock(&g_mutex);
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
		tid_lst[i].buf_size = 0;
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

int	child_proc(int sock, t_tid *tid, int *fd, char *buffer)
{
	int	t_idx;
	
	//init
	if (init_(&g_mutex, tid) < 0)
	{
		perror("mutex allocate error");
		return 1;
	}
	close(fd[1]);
	//dup fd
	dup2(fd[0], 0);
	memset(buffer, 0, BUFFER_SIZE);
	while (read(0, buffer, BUFFER_SIZE) != EOF)
	{
		//exit when type 'stop'
		if (!strcmp(buffer, "stop\n"))
			break;
		//wait for thread is free
		t_idx = pigeon_hole(tid);
		//make send thread
		tid[t_idx].clnt_sock = sock;
		tid[t_idx].buffer = strcpy(tid[t_idx].buffer, buffer);
		tid[t_idx].buf_size = strlen(buffer);
		if ((pthread_create(&(tid[t_idx].id), NULL, clnt_thread, &tid[t_idx])))
		{
			perror("failed to create thread.");
			return 1;
		}
		pthread_detach(tid[t_idx].id);
		//clean buffer
		memset(buffer, 0, BUFFER_SIZE);
	}
	close(fd[0]);
	free(buffer);
	return 0;
}

void	parent_proc(int *fd, char *buffer)
{
	close(fd[0]);
	//dup fd
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

void	free_mutex(void)
{
	pthread_mutex_destroy(&g_mutex);
}
