#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#include "server.h"
#include "thread.h"
#include "http.h"

static pthread_mutex_t	g_mutex;

//thread
static void	*clnt_thread(void *arg)
{
	t_tid	*tid;
	char	*buffer;
	char	*out_buf;

	tid = (t_tid *)arg;
	if (!(buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE)))
	{
		perror("malloc error");
		pthread_exit(NULL);
	}
	memset(buffer, 0, BUFFER_SIZE);
	//mutex lock
	pthread_mutex_lock(&g_mutex);
	tid->free = 0;
	//mutex unlock
	pthread_mutex_unlock(&g_mutex);
	
	//receive
	recv(tid->clnt_sock, buffer, BUFFER_SIZE, 0);
	//print
	dprintf(2, "receive:%s", buffer);
	
	//parsing
	pthread_mutex_lock(&g_mutex);
	protocol_reader(buffer, tid);
	pthread_mutex_unlock(&g_mutex);
	//test delay
#if 1
	sleep(5);
#endif

	//send
	out_buf = head_builder(tid->ver, 0);
	dprintf(tid->clnt_sock, "%s", out_buf);
	
	//close socket
	close(tid->clnt_sock);
	
	//close and free
	free(buffer);
	free(out_buf);
	//mutex lock
	pthread_mutex_lock(&g_mutex);
	tid->free = 1;
	tid->method = 0;
	free(tid->url);
	tid->url = 0;
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
		tid_lst[i].method = 0;
		tid_lst[i].ver = 0;
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
		while (idx < THREAD_POOL_SIZE && !(tid[idx].free))
			idx++;
		if (idx < THREAD_POOL_SIZE)
			break;
		sleep(1);
	}
	return idx;
}

int	main_thread(int sock)
{
	int	clnt_sock;
	int	t_idx;
	
	socklen_t		len;
	struct sockaddr_in	clnt_sin;

	t_tid		tid[THREAD_POOL_SIZE];

	if (init_(&g_mutex, tid) < 0)
		return -1;
	//loop
	while (1)
	{
		len = sizeof(clnt_sin);
		//accept
		if ((clnt_sock = accept(sock, (struct sockaddr *)&clnt_sin, &len)) < 0)
		{
			perror("accept error");
			return -1;
		}
		//wait
		t_idx = pigeon_hole(tid);
		//make thread
		tid[t_idx].clnt_sock = clnt_sock;
		if ((pthread_create(&(tid[t_idx].id), NULL, clnt_thread, &tid[t_idx])))
		{
			perror("failed to create thread.");
			return -1;
		}
		pthread_detach(tid[t_idx].id);
	}
	//free
	pthread_mutex_destroy(&g_mutex);
	return 1;
}
