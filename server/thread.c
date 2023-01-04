#include <pthread.h>

#include "server.h"
#include "thread.h"

static pthread_mutex_t	g_mutex;

//thread
static void	*clnt_thread(void *arg)
{
	t_tid	*tid;

	tid = (t_tid *)arg;
	//mutex lock
	pthread_mutex_lock(&g_mutex);
	//send data
	write(tid->clnt_sock, "hello world", 12);
	//close
	printf("%d is exit.\n", tid->clnt_sock);
	close(tid->clnt_sock);
	tid->free = 1;
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
		tid_lst[i].free = 1;
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
		sleep(10);
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