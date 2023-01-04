#ifndef THREAD_H
# define THREAD_H

# include <pthread.h>

# define THREAD_POOL_SIZE 10

typedef struct s_tid
{
	int	free;
	int	clnt_sock;
	pthread_t	id;
}	t_tid;

#endif
