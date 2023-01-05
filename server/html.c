#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "html.h"

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

char	*head_builder(int status)
{
	char	*header;

	if (!(header = (char *)malloc(sizeof(char) * HEADER_LEN)))
		return NULL;
	memset(header, 0, HEADER_LEN);
	switch (status)
	{
		case NOT_FOUND:
			snprintf(header, HEADER_LEN, HEADER_FORMAT, "404 Not Found");
			break;
		case FORBIDDEN:
			snprintf(header, HEADER_LEN, HEADER_FORMAT, "403 Forbidden");
			break;

		case BAD_REQUEST:
			snprintf(header, HEADER_LEN, HEADER_FORMAT, "400 Bad Request");
			break;
		default:
			snprintf(header, HEADER_LEN, HEADER_FORMAT, "200 OK");
	}
	cpy_to_header(header, "\r\n");
	return header;
}

int	protocol_reader(const char *req, t_tid *tid)
{
	char	m_tmp[10];
	char	*u_tmp;
	char	h_tmp[20];

	if (!(u_tmp = (char *)malloc(sizeof(char) * 100)))
	{
		perror("malloc error");
		return -1;
	}
	sscanf(req, "%s %s HTTP/1.1\r\nHost: %s\r\n", m_tmp, u_tmp, h_tmp);
	//put data to tid
	if (!strcmp(m_tmp, "GET"))
		tid->method = GET;
	else if (!strcmp(m_tmp, "HEAD"))
		tid->method = HEAD;
	else if (!strcmp(m_tmp, "POST"))
		tid->method = POST;
	else if (!strcmp(m_tmp, "PUT"))
		tid->method = PUT;
	//else 0
	tid->url = u_tmp;
	return 0;
}

















