#include "queue.h"

void	init_queue(t_queue *queue)
{
	t_node	*node;

	queue->size = 0;
	//node init
	node = (t_node *)malloc(sizeof(t_node));
	node->next = NULL;
	node->sock = 0;
	
	queue->start = node;
}

void	enqueue(t_queue *queue, t_node *new_)
{
	t_node	*tail;

	tail = queue->start;
	while (tail->next != NULL)
		tail = tail->next;
	tail->next = new_;
	queue->size += 1;
}

int	dequeue(t_queue *queue)
{
	t_node	*head;
	
	if (!queue->size)
	{
		perror("queue is empty");
		return -1;
	}
	queue->size -= 1;
	head = queue->start->next;
	queue->start->next = head->next;
	free(head);
	return 1;
}

void	del_queue(t_queue *queue)
{
	t_node	*idx;
	t_node	*tmp;

	idx = queue->start;
	while (idx)
	{
		tmp = idx;
		idx = idx->next;
		free(tmp);
	}
}

int	get_head(t_queue *queue)
{
	return queue->start->next->sock;
}

#if 0
int	main(void)
{
	t_queue queue;

	init_queue(&queue);
	printf("%d\n", queue.size);
	enqueue(&queue, (t_node *)malloc(sizeof(t_node)));
	printf("%d\n", queue.size);
	enqueue(&queue, (t_node *)malloc(sizeof(t_node)));
	printf("%d\n", queue.size);
	enqueue(&queue, (t_node *)malloc(sizeof(t_node)));
	printf("%d\n", queue.size);
	enqueue(&queue, (t_node *)malloc(sizeof(t_node)));
	printf("%d\n", queue.size);
	dequeue(&queue);
	printf("%d\n", queue.size);
	dequeue(&queue);
	printf("%d\n", queue.size);
	dequeue(&queue);
	printf("%d\n", queue.size);
	del_queue(&queue);
	return 0;
}
#endif
