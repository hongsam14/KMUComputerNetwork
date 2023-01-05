#ifndef HTML_H
# define HTML_H

# define HEADER_FORMAT "%s %s HTTP/1.1\r\nHost: %s\r\n"
# define LANGUAGE "Accept-Language: %s\r\n"
# define CONNECTION "Connection: %s\r\n"
# define CACHECONTROL "Cache-Control: max-age=%d\r\n"

# define HEADER_LEN 200

# define GET 0
# define HEAD 1
# define POST 2
# define PUT 3

char	*head_builder(char *method, char *url, char *addr);

#endif
