#include <string.h>

#include "client.h"
#include "setting.h"

static void	init_IPV4sockaddr(struct sockaddr_in *sin, int port, in_addr_t dest_addr)
{
	memset(sin, 0, sizeof(*sin));
	sin->sin_family = AF_INET;
	sin->sin_port = htons(port);
	sin->sin_addr.s_addr = dest_addr;
}

int	TCPconnector(int port, in_addr_t dest_addr, t_queue *queue)
{
	int	sock;
	struct sockaddr_in	sin;

	sock = 0;
	if (!strcmp(HTTP_VERSION, "1.1") && queue->size > 0)
		sock = get_head(queue);
	else
	{
		//init socket
		sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		//init addr
		init_IPV4sockaddr(&sin, port, dest_addr);
		//connect
		if ((connect(sock, (struct sockaddr *)&sin, sizeof(sin))) < 0)
		{
			perror("connection error");
			return -1;
		}
	}
	return sock;
}

void	disconnector(t_queue *queue, int sock)
{
	if (!strcmp(HTTP_VERSION, "1.1"))
	{
		if (queue->size > 0)
			return;
	}
	close(sock);
}
