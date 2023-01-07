#----------------------------------------------------------------------------
#				MakeFile
#	Creation Date : 2023.01.02
#----------------------------------------------------------------------------

NAME = Server

CC = gcc
CFLAGS = -Wall -Wextra -Werror -fsanitize=address -O3

SRCS = ./server.c\
       ./listener.c\
       ./thread.c\
       ./http.c\

OBJS = $(SRCS:.c=.o)

all : $(NAME)

$(NAME) : $(OBJS)
	    $(CC) $(CFLAGS) $(OBJS) -lpthread -o $@ 

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

fclean : clean
	rm -f $(NAME)

clean :
	rm -f $(OBJS)

re : clean all

.PHONY : all fclean clean re
