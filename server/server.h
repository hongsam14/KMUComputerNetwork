#ifndef SERVER_H
# define SERVER_H
//https://man7.org/linux/man-pages/man0/sys_socket.h.0p.html
# include <sys/socket.h>
//https://man7.org/linux/man-pages/man7/ip.7.html
# include <netinet/in.h>

# include <unistd.h>
# include <stdio.h>

# define LISTEN_QUEUE_SIZE 5

int	TCPlistener(int port);

#endif
