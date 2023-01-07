#ifndef HTTP_H
# define HTTP_H

# define HEADER_FORMAT "%s %s HTTP/%s\r\nHost: %s\r\n"
# define CONNECTION "Connection: %s\r\n"
# define CACHECONTROL "Cache-Control: max-age=%d\r\n"

# define HEADER_LEN 200

char	*head_builder(char *method, char *url, char *addr);

#endif
