#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	pid_t pid;
	printf("pid is %d\n", getpid());

	pid = fork();
	if (pid == 0)
	{
		printf("In child process: pid = %d, ppid = %d\n", \
		getpid(), getppid());
	}
	else
	{
		printf("In parent, pid = %d, fork returned = %d\n", \
		getpid(), pid);
	}

	exit(0);
}
