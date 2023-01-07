#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "http.h"
#include "setting.h"

static char	*cpy_to_header(char *header, char *str)
{
	char	*idx;

	idx = header;
	while (*idx != '\0')
		idx++;
	while (*str != '\0' && idx - header < HEADER_LEN)
	{
		*idx = *str;
		idx++;
		str++;
	}
	return idx;
}

char	*head_builder(char *method, char *url, char *addr)
{
	char	*header;

	if (!(header = (char *)malloc(sizeof(char) * HEADER_LEN)))
		return NULL;
	memset(header, 0, HEADER_LEN);
	snprintf(header, HEADER_LEN, HEADER_FORMAT, method, url, HTTP_VERSION, addr);
	cpy_to_header(header, "\r\n");
	return header;
}
