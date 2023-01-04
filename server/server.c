#include <assert.h>
#include <stdlib.h>

#include "server.h"

int	main(int argc, char *argv[])
{
	int	sock;
	int	clnt_sock;
	int	port_num;
	struct	sockaddr_in	clnt_sin;
	socklen_t		len;

	
	//arg check
	assert(argc == 2);
	//init port number
	port_num = atoi(argv[1]);
	//port number range check
	assert(port_num > 0 || port_num < 65535);
	//init listen socket
	if (!(sock = TCPlistener(port_num)))
		exit(1);
	//accept
	len = sizeof(clnt_sin);
	if ((clnt_sock = accept(sock, (struct sockaddr *)&clnt_sin, &len)) < 0)
	{
		//error
		exit(1);
	}
	//send data
	write(clnt_sock, "hello world\n", 13);
	
	close(clnt_sock);
	close(sock);
	return (0);
}
