// standard libs
#include <stdio.h>
#include <stdlib.h>

// sys calls
#include <fcntl.h>
#include <unistd.h>

// types
#include <string.h>
#include <stdbool.h>

#include <ctype.h>

// signals
#include <sys/types.h>
#include <signal.h>

#define CMD_LENGTH 256
#define PROMPT_STRING "prompt> "
#define DELIM " \r\n"
#define EXIT_OP "exit"
#define HISTORY_FILE_NAME ".history"
#define GOOD_BYE "good bye.\n"

#define IN  
#define OUT  

// GLOBALS
pid_t	pid;
FILE *	fp_hist;

// PROTO TYPES
void sigint_handler(int signo);

int set_up();
int clean_up();

bool has_pipe(const char * str);
bool has_redirection(const char * str);

char ** get_argv(char* cmd, char** argv);
void get_redir_filename(char* cmd, char * in_file, char * out_file, bool* redir_in, bool* redir_out, bool* redir_append);


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

		bool redir_in = false;
		bool redir_out = false;
		bool redir_append = false;

		char in_file[256];
		char out_file[256];

		{
			/* Analysis */
			int res;

			if(has_pipe(cmd_input))
			{
			
			}
			else if(has_redirection(cmd_input))
			{
				get_redir_filename(cmd_input, in_file, out_file, &redir_in, &redir_out, &redir_append);

				if(redir_out && redir_append)
				{
					printf("Redirection Error.\n");
					continue;
				}

				get_argv(cmd_input, argv);
			}
			else
			{	
				get_argv(cmd_input, argv);
			}

			if(strcmp(EXIT_OP, argv[0]) == 0)
			{
				break;
			}
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

				if (redir_in)
				{
					int fd = open(in_file, O_RDONLY, 0);
					dup2(fd, STDIN_FILENO);
					close(fd);
				}
				
				if (redir_out)
				{
					int fd = creat(out_file, 0644);
					dup2(fd, STDOUT_FILENO);
					close(fd);
				}
				else if (redir_append)
				{
					int fd = open(out_file, O_WRONLY | O_APPEND, 0644);
					dup2(fd, STDOUT_FILENO);
					close(fd);
				}

				execvp(argv[0], argv);

				printf("Invalid Command.\n");
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
		printf(GOOD_BYE);
		clean_up();
	}

	return 0;
}

void sigint_handler(int signo)
{
	if(pid != 0)
	{
		kill(pid, SIGKILL);
	}
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

bool has_pipe(const char * str)
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

bool has_redirection(const char * str)
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

void get_redir_filename(char* cmd, char * in_file, char * out_file, bool* redir_in, bool* redir_out, bool* redir_append)
{
	char * p = cmd;


	while(*cmd != '\0')
	{
		//printf("get redirection file name : %c\n", *cmd);

		if(*cmd == '<')
		{
			char * start = cmd;
			*redir_in = true;

			do{ cmd++; }while(isspace(*cmd));
			while(isspace(*cmd) == false && *cmd != '\0')
			{
				*in_file = *cmd;
				in_file++;
				cmd++;
			}
			*(++in_file) = '\0';
			cmd++;

			{
				int rest = 0;
				char* p = cmd;
				while( *p != '\0' ) { rest++; p++; }
				memmove(start, cmd, rest + 1);

				cmd = start;
				continue;
			}
		}

		else if(*cmd == '>')
		{
			char * start = cmd;

			if(*(cmd + 1) == '>')
			{
				*redir_append = true;
				cmd++;
			}
		
			else
			{
				*redir_out = true;
			}

			do{ cmd++; }while(isspace(*cmd));
			while(isspace(*cmd) == false && *cmd != '\0')
			{
				*out_file = *cmd;
				out_file++;
				cmd++;
			}
			*(++out_file) = '\0';
			cmd++;

			{
				int rest = 0;
				char* p = cmd;
				while( *p != '\0' ) { rest++; p++; }
				memmove(start, cmd, rest + 1);

				cmd = start;
				continue;
			}
		}

		cmd++;
	}
}