#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include <sys/types.h>
#include <signal.h>

#define CMD_LENGTH 256
#define PROMPT_STRING "prompt> "
#define DELIM " \r\n"
#define EXIT_OP "exit"
#define HISTORY_FILE_NAME ".history"

// GLOBALS
pid_t	pid;
FILE *	fp_hist;

void sigint_handler(int signo)
{
	if(pid != 0)
	{
		kill(pid, SIGKILL);
	}
}

// PROTO TYPES
int set_up();
int clean_up();

bool has_pipe(char * str);
bool has_redirection(char * str);

bool run_command(char * str);

int main()
{
	{
		set_up();
	}

	char cmd_input[CMD_LENGTH];
	printf(PROMPT_STRING);

	while(fgets(cmd_input, CMD_LENGTH, stdin))
	{
		pid = fork();

		switch(pid)
		{
			case -1:
			{
				printf("fork() failed. abort.\n");
				return EXIT_FAILURE;
			}
			break;

			case 0:
			{
				// child
				
				if(has_pipe(cmd_input))
				{

				}
				else if(has_redirection(cmd_input))
				{

				}
				else
				{
					run_command(cmd_input);
				}

				return -1;
			}
			break;

			default:
			{
				// parent
				int status;
				wait(&status);
			}
			break;
		}

		printf(PROMPT_STRING);
	}

	{
		clean_up();
	}

	return 0;
}

int set_up()
{
	/* 1. terminate signal override */
	{
		//signal( SIGINT, sigint_handler);
	}

	/* 2. history file open */
	{
		if(fp_hist = fopen(HISTORY_FILE_NAME, "w+"))
		{

		}
		else
		{
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

int clean_up()
{
	fclose(fp_hist);

	return EXIT_SUCCESS;
}

bool has_pipe(char * str)
{
	int len = strlen(str);
	int i;
	
	for(i = 0; i < len; i++)
	{
		if(str[i] == '|')
			return true;
	}

	return false;
}

bool has_redirection(char * str)
{
	int len = strlen(str);
	int i;
	
	for(i = 0; i < len; i++)
	{
		if(str[i] == '<' || str[i] == '>')
			return true;
	}

	return false;
}

bool run_command(char * str)
{
	int argc = 0;
	char * delim = " \r\n";

	// counting argc
	{
		char * cmd = (char *) malloc(sizeof(char) * CMD_LENGTH);
		char * token = NULL;
		strcpy(cmd, str);

		while((token = strsep(&cmd, DELIM)) != NULL)
		{
			printf("token is \'%s\'\n", token);
			argc += 1;
		}

		free(cmd);
	}

	if ( argc > 0 )
	{
		char * cmd = (char *) malloc(sizeof(char) * CMD_LENGTH);
		char ** argv;

		int i = 0;

		strcpy(cmd, str);

		argv = (char**) malloc(sizeof(char*) * (argc + 1));

		for(i = 0; i < argc; i++)
		{
			char * token = strsep(&cmd, delim);
			argv[i] = (char *) malloc(sizeof(char) * strlen(token));
			strcpy(argv[i], token);

			printf("argv[%d] is \'%s\'\n", i, argv[i]);
		}

		// Need to NULL terminate your argument strings to 'execvp'.
		argv[argc] = NULL;

		// Don't need to free heap memories. 
		// http://man7.org/linux/man-pages/man2/execve.2.html
		execvp(argv[0], argv);
		printf("There're problems on execvp.\n");

		return EXIT_FAILURE;
	}
}