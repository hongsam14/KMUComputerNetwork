#ifndef THREAD_H
# define THREAD_H

# include <pthread.h>
# include <netinet/in.h>

typedef struct s_tid
{
	int	idx;
	int	free;
	int	port;
	char	*method;
	char	*url;
	in_addr_t	dest_addr;
	pthread_t	id;
}	t_tid;

#endif
