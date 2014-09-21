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

int main()
{
	{
		set_up();
	}

	char cmd_input[CMD_LENGTH];
	printf(PROMPT_STRING);

	while(fgets(cmd_input, CMD_LENGTH, stdin))
	{
		if(has_pipe(cmd_input))
		{

		}
		else if(has_redirection(cmd_input))
		{

		}
		else
		{
			// Just a cmd.
		}

		char * cmd_buffer = (char*) malloc(sizeof(char) * CMD_LENGTH);
		char * cmd_token = 0;

		char * cmd_operator = 0;

		int i = 0;

		strcpy(cmd_buffer, cmd_input);

		cmd_operator = strsep(&cmd_buffer, DELIM);

		if( strcmp(cmd_operator, EXIT_OP) == 0 )
		{
			exit(0);
		}

		while(cmd_token = strsep(&cmd_buffer, DELIM))
		{
			
			if(cmd_token != NULL)
			{
				//printf("%d: %s\n", i, cmd_token);
			}

			i++;

		}

		free(cmd_buffer);

		// forking
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

				execlp( cmd_operator, cmd_operator, NULL);
				return -1;
			}
			break;

			default:
			{
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
		signal( SIGINT, sigint_handler);
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