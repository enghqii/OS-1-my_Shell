#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include <ctype.h>

#include <sys/types.h>
#include <signal.h>

#define CMD_LENGTH 256
#define PROMPT_STRING "prompt> "
#define DELIM " \r\n"
#define EXIT_OP "exit"
#define HISTORY_FILE_NAME ".history"

#define IN  
#define OUT  

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

char ** get_argv(char* cmd, char** argv);

int main()
{
	{
		set_up();
	}

	char cmd_input[CMD_LENGTH];
	printf(PROMPT_STRING);

	while(fgets(cmd_input, CMD_LENGTH, stdin))
	{
		char * argv[64];

		if(has_pipe(cmd_input))
		{
		
		}
		else if(has_redirection(cmd_input))
		{

		}
		else
		{	
			get_argv(cmd_input, argv);
		}

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
				execvp(argv[0], argv);
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

char ** get_argv(char* cmd, char** argv)
{
	char * 	delim = " \r\n";
	int 		i = 0;

	argv[0] = strtok(cmd, delim);

	while(argv[++i] = strtok(NULL, delim)){}

	return argv;
}