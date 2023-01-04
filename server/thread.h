#ifndef THREAD_H
# define THREAD_H

# include <pthread.h>

typedef struct s_tid
{
	int	idx;
	int	free;
	int	clnt_sock;
	pthread_t	id;
}	t_tid;

#endif
