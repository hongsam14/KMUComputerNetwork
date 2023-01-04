#ifndef SERVER_H
# define SERVER_H
//https://man7.org/linux/man-pages/man0/sys_socket.h.0p.html
# include <sys/socket.h>
//https://man7.org/linux/man-pages/man7/ip.7.html
# include <netinet/in.h>

# include <unistd.h>
# include <stdio.h>
# include <fcntl.h>

# include "setting.h"

//listener.c
int	TCPlistener(int port);
//thread.c
int	main_thread(int sock);

#endif
