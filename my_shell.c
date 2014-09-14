#include <stdio.h>
#include <unistd.h>

int main()
{
	pid_t pid;

	printf("forking.. \n");

	pid = fork();

	switch( pid )
	{
		case -1:
			printf("forking failed\n");
		break;

		case 0:
			printf("I'm child\n");
		break;

		default:
			{
				int status;
				printf("I'm parent\n");
				wait(&status);

				printf("child process ended\n");
			}
		break;
	}

	printf("wow break\n");

	return 0;
}