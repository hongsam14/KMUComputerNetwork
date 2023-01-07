#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "http.h"

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

static char	*ver_str(int ver)
{
	switch (ver)
	{
		case VER_1_1:
			return "1.1";
		case VER_1:
		default:
			return "1.0";
	}
	return 0;
}

char	*head_builder(int ver, int status)
{
	char	*header;

	if (!(header = (char *)malloc(sizeof(char) * HEADER_LEN)))
		return NULL;
	memset(header, 0, HEADER_LEN);
	switch (status)
	{
		case NOT_FOUND:
			snprintf(header, HEADER_LEN, HEADER_FORMAT, ver_str(ver), "404 Not Found");
			break;
		case FORBIDDEN:
			snprintf(header, HEADER_LEN, HEADER_FORMAT, ver_str(ver), "403 Forbidden");
			break;
		case BAD_REQUEST:
			snprintf(header, HEADER_LEN, HEADER_FORMAT, ver_str(ver), "400 Bad Request");
			break;
		case OK:
		default:
			snprintf(header, HEADER_LEN, HEADER_FORMAT, ver_str(ver), "200 OK");
	}
	cpy_to_header(header, "\r\n");
	return header;
}

int	protocol_reader(const char *req, t_tid *tid)
{
	char	m_tmp[10];
	char	v_tmp[4];
	char	*u_tmp;
	char	h_tmp[20];

	if (!(u_tmp = (char *)malloc(sizeof(char) * 100)))
	{
		perror("malloc error");
		return -1;
	}
	sscanf(req, "%s %s HTTP/%s\r\nHost: %s\r\n", m_tmp, u_tmp, v_tmp, h_tmp);
	//put data to tid
	if (!strcmp(m_tmp, "HEAD"))
		tid->method = HEAD;
	else if (!strcmp(m_tmp, "POST"))
		tid->method = POST;
	else if (!strcmp(m_tmp, "PUT"))
		tid->method = PUT;
	else if (!strcmp(m_tmp, "GET"))
		tid->method = GET;

	if (!strcmp(v_tmp, "1.1"))
		tid->ver = VER_1_1;
	else if (!strcmp(v_tmp, "1.0"))
		tid->ver = VER_1;
	//url
	tid->url = u_tmp;
	return 0;
}

















