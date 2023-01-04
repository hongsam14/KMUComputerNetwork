#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "client.h"

static void	child_process(int *fd, char *buffer)
{
	close(fd[1]);
	memset(buffer, 0, BUFFER_SIZE);
	//make thread until EOF
	while (read(fd[0], buffer, BUFFER_SIZE) != EOF)
	{
		printf("%s", buffer);
		memset(buffer, 0, BUFFER_SIZE);
	}
	close(fd[0]);
	free(buffer);
	exit(0);
}

static void	parent_process(int *fd, char *buffer)
{
	close(fd[0]);
	while (1)
	{
		memset(buffer, 0, BUFFER_SIZE);
		read(0, buffer, BUFFER_SIZE);
		write(fd[1], buffer, BUFFER_SIZE);
		//exit when type 'exit'
		if (!strcmp(buffer, "exit\n"))
			break;
	}
	close(fd[1]);
	free(buffer);
}

int	main_prompt(void)
{
	int	fd[2];
	pid_t	pid;

	char	*buffer;

	//init pipe
	if (pipe(fd) == -1)
	{
		perror("pipe error");
		exit(1);
	}
	//init buffer
	buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE);
	if (!buffer)
	{
		perror("malloc error");
		exit(1);
	}
	//signal
	signal(SIGCHLD, SIG_IGN);
	//fork
	pid = fork();
	if (pid == -1)
	{
		perror("fork error");
		exit(1);
	}
	//child process(thread manager)
	else if (pid == 0)
		child_process(fd, buffer);
	//parent process(prompt)
	else
		parent_process(fd, buffer);
	return 1;
}
