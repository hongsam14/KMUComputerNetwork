#ifndef THREAD_H
# define THREAD_H

# include <pthread.h>
# include <netinet/in.h>

typedef struct s_tid
{
	int	idx;
	int	free;
	int	port;
	//protocol
	char	*method;
	char	*url;
	in_addr_t	dest_addr;
	//thread
	pthread_t	id;
}	t_tid;

#endif
