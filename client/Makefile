#----------------------------------------------------------------------------
#				MakeFile
#	Creation Date : 2023.01.04
#----------------------------------------------------------------------------

NAME = Client

CC = gcc
CFLAGS = -Wall -Wextra -Werror -O3

SRCS = ./client.c\
       ./prompt.c\
       ./connector.c\

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
