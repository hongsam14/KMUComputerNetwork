#ifndef CLIENT_H
# define CLIENT_H

//https://man7.org/linux/man-pages/man0/sys_socket.h.0p.html
# include <sys/socket.h>
//https://man7.org/linux/man-pages/man7/ip.7.html
# include <netinet/in.h>
//https://linux.die.net/man/3/inet_addr
# include <arpa/inet.h>

# include <unistd.h>
# include <stdio.h>
# include <fcntl.h>
# include <stdlib.h>
# include <string.h>

# include "setting.h"
# include "queue.h"
# include "thread.h"

typedef struct s_info
{
	t_queue	*queue;
	t_tid	*tid;
}	t_info;

//prompt.c
int	main_prompt(in_addr_t dest, int port_num);
//connector.c
int	TCPconnector(int port, in_addr_t dest_addr, t_queue *queue);
void	disconnector(t_queue *queue, int sock);

#endif
