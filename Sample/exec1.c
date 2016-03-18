#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	char *arg[] = {"/bin/ls", 0};
	// char *arg[] = {'/bin/ls', 0};

	// it was said that this step is for
	// fork, and exec with in child progress
	// don't quite understand	
	if (fork() == 0)
	{
		printf("In child progress:\n");
		execv(arg[0], arg);
	}
	
	printf("Execution continuous in parent progree...\n");
}

