#include <assert.h>
#include <stdlib.h>
#include <pthread.h>

#include "server.h"

int	main(int argc, char *argv[])
{
	int	sock;
	int	port_num;
	
	//arg check
	assert(argc == 2);
	//init port number
	port_num = atoi(argv[1]);
	//port number range check
	assert(port_num > 0 || port_num < 65535);
	//init listen socket
	if ((sock = TCPlistener(port_num)) < 0)
		exit(1);
	//accept
	if (main_thread(sock) < 0)
		exit(1);
	close(sock);
	return (0);
}
