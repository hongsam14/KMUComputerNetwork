#ifndef HTML_H
# define HTML_H

# include "thread.h"

# define HEADER_FORMAT "HTTP/1.1 %s\r\n"
# define CONNECTION "Connection: %s\r\n"
# define CACHECONTROL "Cache-Control: max-age=%d\r\n"

# define HEADER_LEN 200

# define GET 1
# define HEAD 2
# define POST 3
# define PUT 4

# define OK 200
# define BAD_REQUEST 400
# define FORBIDDEN 403
# define NOT_FOUND 404

char	*head_builder(int status);
int	protocol_reader(const char *req, t_tid *tid);

#endif
