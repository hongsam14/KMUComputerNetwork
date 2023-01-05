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
	int	status;
	int	port_num;
	in_addr_t	dest_addr;

	//arg check
	assert(argc == 3);
	//init ip address
	//assert(check_ip(argv[1]) != NULL);
	check_ip(argv[1]);
	dest_addr = inet_addr(argv[1]);
	//init port number
	port_num = atoi(argv[2]);
	//port number range check
	assert(port_num > 0 || port_num < 65535);
	//init connect
	status = main_prompt(dest_addr, port_num);
	return status;
}
