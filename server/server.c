#include <assert.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "server.h"

int	main(int argc, char *argv[])
{
	int	port_num;
	int	sock;
	int	clnt_sock;
	struct	sockaddr_in	sin;
	struct	sockaddr_in	clnt_sin;
	socklen_t		len;

	
	//arg check
	assert(argc == 2);
	//init port number
	port_num = atoi(argv[1]);
	//port number range check
	assert(port_num > 0 || port_num < 65535);

	//socket init
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	//init sockaddr
	memset(&sin, 0, sizeof(sin));
	sin.sin_len = sizeof(sin);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port_num);
	sin.sin_addr.s_addr = INADDR_ANY;
	
	//bind
	if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		//error
		exit(1);
	}
	//listen
	if (listen(sock, 5) == -1)
	{
		//error
		exit(1);
	}
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
