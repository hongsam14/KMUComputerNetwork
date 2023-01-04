#ifndef THREAD_H
# define THREAD_H

# include <pthread.h>

typedef struct s_tid
{
	int	idx;
	int	free;
	int	buf_size;
	char	*buffer;
	int	clnt_sock;
	pthread_t	id;
}	t_tid;

int	child_proc(int sock, t_tid *tid, int *fd, char *buffer);
void	parent_proc(int *fd, char *buffer);
void	free_mutex(void);

#endif
