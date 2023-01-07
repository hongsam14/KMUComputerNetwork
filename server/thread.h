#ifndef THREAD_H
# define THREAD_H

# include <pthread.h>

typedef struct s_tid
{
	int	idx;
	int	free;
	int	clnt_sock;
	//protocol
	int	method;
	int	ver;
	char	*url;
	//thread
	pthread_t	id;
}	t_tid;

#endif
