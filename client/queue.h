#ifndef QUEUE_H
# define QUEUE_H

# include <stdlib.h>
# include <pthread.h>
# include <stdio.h>

typedef	struct s_node
{
	int	sock;
	struct s_node	*next;
}	t_node;

typedef struct s_queue
{
	int	size;
	t_node	*start;
}	t_queue;

void	init_queue(t_queue *queue);
void	enqueue(t_queue *queue, t_node *new_);
int	dequeue(t_queue *queue);
void	del_queue(t_queue *queue);
int	get_head(t_queue *queue);

#endif
