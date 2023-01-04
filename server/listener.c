#include <string.h>

#include "server.h"

static void	init_IPV4sockaddr(struct sockaddr_in *sin, int port)
{
	memset(sin, 0, sizeof(*sin));
	sin->sin_len = sizeof(*sin);
	sin->sin_family = AF_INET;
	sin->sin_port = htons(port);
	sin->sin_addr.s_addr = INADDR_ANY;
}

int	TCPlistener(int port)
{
	int	sock;
	struct sockaddr_in	sin;

	//init socket
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
	{
		perror("socket error");
		return -1;
	}
	//init sockaddr
	init_IPV4sockaddr(&sin, port);
	//bind
	if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		perror("socket error");
		return -1;
	}
	//listen
	if (listen(sock, LISTEN_QUEUE_SIZE) == -1)
	{
		perror("socket error");
		return -1;
	}
	return sock;
}
