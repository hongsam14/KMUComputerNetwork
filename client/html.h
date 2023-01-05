#ifndef HTML_H
# define HTML_H

# define HEADER_FORMAT "%s %s HTTP/1.1\r\nHost: %s\r\n"
# define CONNECTION "Connection: %s\r\n"
# define CACHECONTROL "Cache-Control: max-age=%d\r\n"

# define HEADER_LEN 200

char	*head_builder(char *method, char *url, char *addr);

#endif
