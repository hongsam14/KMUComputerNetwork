#include <signal.h>
#include <string.h>

#include "thread.h"
#include "client.h"

static int	status_control(int status)
{
	int	signal;

	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
	{
		signal = WTERMSIG(status);
		return (128 + signal);
	}
	return (status);
}

static void	parent_wait(pid_t pid, int *status)
{
	waitpid(pid, status, WUNTRACED);
	//wait until exited or signaled
	while (!WIFEXITED(*status) && !WIFSIGNALED(*status))
		waitpid(pid, status, WUNTRACED);
}

int	main_prompt(int sock)
{
	int	pid;
	int	fd[2];
	int	wstatus;

	char	*buffer;
	t_tid	tid[THREAD_POOL_SIZE];

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
	//fork
	pid = fork();
	if (pid == -1)
	{
		perror("fork error");
		exit(1);
	}
	//child process(thread manager)
	else if (pid == 0)
	{
		exit(child_proc(sock, tid, fd, buffer));
	}
	//parent process(prompt)
	else
	{
		parent_proc(fd, buffer);
		parent_wait(pid, &wstatus);
		free_mutex();
	}
	return status_control(wstatus);
}
