#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	pid_t pid;
	printf("pid_t pid returned %d\n", pid);
	pid = fork();
	printf("fork returned %d\n", pid);
	exit(0);
}
