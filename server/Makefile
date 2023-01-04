#----------------------------------------------------------------------------
#				MakeFile
#	Creation Date : 2023.01.02
#----------------------------------------------------------------------------

NAME = Server

CC = gcc
CFLAGS = -Wall -Wextra -Werror -O3

SRCS = ./server.c\
       ./listener.c\
       ./thread.c\

OBJS = $(SRCS:.c=.o)

all : $(NAME)

$(NAME) : $(OBJS)
	    $(CC) $(CFLAGS) $(OBJS) -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

fclean : clean
	rm -f $(NAME)

clean :
	rm -f $(OBJS)

re : clean all

.PHONY : all fclean clean re
