#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "client.h"

static char	*check_ip(char *const input)
{
	char	*idx;

	idx = input;
	while (*idx != '\0')
	{
		if (!strchr(".0123456789", *idx))
			return NULL;
		idx++;
	}
	return input;
}

int	main(int argc, char *argv[])
{
	int	port_num;
	in_addr_t	dest_addr;
	char	*buffer;

	int	sock;
	struct sockaddr_in	sin;

	//arg check
	assert(argc == 3);
	//init ip address
	assert(check_ip(argv[1]) != NULL);
	dest_addr = inet_addr(argv[1]);
	//init port number
	port_num = atoi(argv[2]);
	//port number range check
	assert(port_num > 0 || port_num < 65535);
	//client
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	//init addr
	memset(&sin, 0, sizeof(sin));
	sin.sin_len = sizeof(sin);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port_num);
	sin.sin_addr.s_addr = dest_addr;
	//connect
	connect(sock, (struct sockaddr *)&sin, sizeof(sin));
	//send
	if (send(sock, "hello world\n", 13, 0) < 0)
	{
		perror("send failed");
		exit(1);
	}
	buffer = (char *)malloc(sizeof(char) * 100);
	if (!buffer)
	{
		perror("malloc error");
		exit(1);
	}
	memset(buffer, 0, 100);
	if (recv(sock, buffer, 100, 0) < 0)
	{
		perror("receive failed");
		exit(1);
	}
	printf("receive:%s\n", buffer);
	free(buffer);
	close(sock);
	return 0;
}
