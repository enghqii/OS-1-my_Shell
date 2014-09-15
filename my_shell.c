#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define CMD_LENGTH 256
#define PROMPT_STRING "prompt> "
#define DELIM " \r\n"

int main()
{
	char cmd_input[CMD_LENGTH];
	printf(PROMPT_STRING);

	while(fgets(cmd_input, CMD_LENGTH, stdin))
	{
		char * cmd_buffer = (char*) malloc(sizeof(char) * CMD_LENGTH);
		char * cmd_token = 0;

		char * cmd_operator = 0;

		int i = 0;
		pid_t pid;

		strcpy(cmd_buffer, cmd_input);

		cmd_operator = strsep(&cmd_buffer, DELIM);

		while(cmd_token = strsep(&cmd_buffer, DELIM))
		{
			
			if(cmd_token != NULL)
			{
				//printf("%d: %s\n", i, cmd_token);
			}

			i++;

		}

		free(cmd_buffer);

		pid = fork();

		switch(pid)
		{
			case -1:
				printf("fork() failed. abort.\n");
				return pid;
			break;

			case 0:
			{
				printf("\tChild: process created.\n\texeclp %s", cmd_operator);

				execlp( cmd_operator, cmd_operator, NULL);
				printf("If you can see this message, Executing designated program has been failed.\n");

				return -1;
			}
			break;

			default:
			{
				int status;

				printf("Parent: process is waiting..\n");
				wait(&status);
				printf("Parent: child process done.\n");
			}
			break;
		}

		printf(PROMPT_STRING);
	}

	return 0;
}