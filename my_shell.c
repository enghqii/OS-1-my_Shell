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

// definitions
#define CMD_LENGTH 256

#define EXIT_OP "exit"

#define HISTORY_OP "history"
#define HISTORY_FILE_NAME ".history"

#define PROMPT_STRING "prompt> "
#define GOOD_BYE "good bye.\n"

#define IN  
#define OUT  

// GLOBALS
pid_t	pid;
FILE *	fp_hist;

/* PROTO TYPES */
void sigint_handler(int signo);

// init, final
int set_up();
int clean_up();

// util
void trim_linefeed(char * str);

// cmd examing
bool is_white_spaces(const char * const str);

bool has_history_execution(const char * const str);
bool has_pipe(const char * const str);
bool has_redirection(const char * const str);

// string tokenize & parsing
void get_history_replaced_cmd(char* cmd);
void get_argv(char* cmd, char** argv);
void get_redir_filename(char* cmd, char * in_file, char * out_file, bool* redir_in, bool* redir_out, bool* redir_append);
void get_cmds_from_pipe(char * cmd, char** cmds);

void execute(char ** argv);

// history related
void record_history(const char * const cmd);
void print_history();

void get_history_last_command(char * cmd);
void get_history_nth_command(int n, char * cmd);

int main()
{
	{
		set_up();
	}

	char cmd_input[CMD_LENGTH];

	while(printf(PROMPT_STRING) && fgets(cmd_input, CMD_LENGTH, stdin))
	{
		char history_replaced_cmd[CMD_LENGTH];

		char * argv[64];
		char * argv2[64];

		bool piped = false;

		bool redir_in = false;
		bool redir_out = false;
		bool redir_append = false;

		char in_file[256];
		char out_file[256];

		trim_linefeed(cmd_input);

		if(is_white_spaces(cmd_input) == true) 
			continue;

		strcpy(history_replaced_cmd, cmd_input);

		{
			/* Analysis */

			if(has_history_execution(cmd_input))
			{
				get_history_replaced_cmd(cmd_input);
				strcpy(history_replaced_cmd, cmd_input);
			}

			if(has_pipe(cmd_input))
			{
				char * cmds[64];
				char ** p_cmd;

				piped = true;
				
				get_cmds_from_pipe(cmd_input, cmds);
				p_cmd = cmds;

				// reversed
				get_argv(cmds[1], argv);
				get_argv(cmds[0], argv2);

			}
			else if(has_redirection(cmd_input))
			{
				redir_in = redir_out = redir_append = false;
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

			/* exit */
			if(strcmp(EXIT_OP, argv[0]) == 0)
			{
				break;
			}
		}

		{
			/* record history */
			if(strcmp(HISTORY_OP, argv[0]) != 0)
				record_history(history_replaced_cmd);
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

				// IO Redirection
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

				// PIPE
				if( piped )
				{
					int pd[2];
        				pipe(pd);

					pid_t pid2 = fork();
					
					switch(pid2)
					{
						case -1:
							printf("fork() failed. abort.\n");
							return EXIT_FAILURE;
						break;
						case 0:
							dup2(pd[1], 1);
							execute(argv2);
						break;
						default:
							dup2(pd[0], 0);
							close(pd[1]);
						break;
					}
				}
	
				execute(argv);
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
	/* 1. override terminate signal  */
	{
		signal( SIGINT, sigint_handler);
	}

	/* 2. history file open (for write) */
	{
		if(fp_hist = fopen(HISTORY_FILE_NAME, "w+"))
		{
			setbuf(fp_hist, NULL);
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

void trim_linefeed(char * str)
{
	char * c = str;
	while( *c != '\r' && *c != '\n' && *c != '\0' ) { c++; }
	*c = '\0';
}

bool is_white_spaces(const char * const str)
{
	const char * p = str;

	while(*p != '\0')
	{
		if(isspace(*p) == false)
			return false;

		p++;
	}

	return true;
}

bool has_history_execution(const char * const str)
{
	bool res = false;
	char * p = NULL;

	res |= (strstr(str, "!!") != NULL);

	p = strstr(str, "!");
	
	if(p != NULL)
	{
		if( (*(p+1) != '\0') && isdigit(*(p+1)) )
		{
			res |= true;
		}
	}

	return res;
}

bool has_pipe(const char * const str)
{
	return (strstr(str, "|") != NULL);
}

bool has_redirection(const char * const str)
{
	return (strstr(str, ">") != NULL) || (strstr(str, "<") != NULL) || (strstr(str, ">>") != NULL);
}

void get_history_replaced_cmd(char* cmd)
{
	char * p = NULL;
	char temp[CMD_LENGTH];

	if( (p = strstr(cmd , "!!")) != NULL )
	{
		char last_command[CMD_LENGTH];
		int len = 0;

		get_history_last_command(last_command);

		len = strlen(last_command);

		strcpy(temp, p + 2);
		strcpy(p + len , temp);
		strncpy(p, last_command, len);
	}
	else
	{
		char nth_command[CMD_LENGTH];
		char * p_num_cnt;
		int num_cnt = 0;
		int num = -1;
		int len = 0;

		p = strstr(cmd, "!");
		sscanf(p, "!%d", &num);

		get_history_nth_command(num, nth_command);

		len = strlen(nth_command);

		// counting num length ( wanna use math.log )
		p_num_cnt = p + 1;
		while(isdigit(*p_num_cnt)){p_num_cnt++; num_cnt++;}

		strcpy(temp, p + num_cnt + 1);
		strcpy(p + len , temp);

		strncpy(p, nth_command, len);
	}
}

void get_argv(char* cmd, char** argv)
{
	char * 	delim = " \r\n";
	int 		i = 0;

	if( argv[0] = strtok(cmd, delim))
		while(argv[++i] = strtok(NULL, delim)){}

}

void get_redir_filename(char* cmd, char * in_file, char * out_file, bool* redir_in, bool* redir_out, bool* redir_append)
{
	char * p = cmd;


	while(*cmd != '\0')
	{
		//printf("get redirection file name : %c\n", *cmd);
		//printf("CMD : \'%s\'\n", cmd);

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
				//cmd++;
			}
			else
			{
				*redir_out = true;
			}

			do{ cmd++; } while(isspace(*cmd));
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

void get_cmds_from_pipe(char * cmd, char** cmds)
{
	char* 	delim = "|\r\n";
	int 		i = 0;

	//printf("cmd is (%s)\n", cmd);

	if( (cmds[0] = strtok(cmd, delim)) != NULL )
	{
		//printf("cmds %s\n", cmds[0]);

		while(cmds[++i] = strtok(NULL, delim)){ }
	}
}

void execute(char ** argv)
{
	//printf("executing : %s\n", argv[0]);

	if(strcmp(HISTORY_OP, argv[0]) == 0)
	{
		// do history things
		print_history();
	}
	else
	{
		execvp(argv[0], argv);
		printf("Invalid Command.\n");
	}
}

void record_history(const char * const cmd)
{
	fprintf(fp_hist, "%s\n", cmd);
}

void print_history()
{
	FILE * fp_hist_read = NULL;

	if(fp_hist_read = fopen(HISTORY_FILE_NAME, "r"))
	{
		char 	str[CMD_LENGTH];
		int 		i = 0;

		while(fgets(str, CMD_LENGTH, fp_hist_read))
		{
			printf("%d. %s\n", ++i ,str);
		}
	}
	else
	{
		printf("Cannot open history file\n");
	}

	fclose(fp_hist_read);
}

void get_history_last_command(char * cmd)
{
	FILE * fp_hist_read = NULL;
	char 	str[CMD_LENGTH];
	char 	last_command[CMD_LENGTH];

	if(fp_hist_read = fopen(HISTORY_FILE_NAME, "r"))
	{

		while(fgets(str, CMD_LENGTH, fp_hist_read))
		{
			// copy without '\n'
			strncpy(last_command, str, strlen(str));
		}
	}
	else
	{
		printf("Cannot open history file\n");
	}

	trim_linefeed(last_command);
	strcpy(cmd, last_command);
	fclose(fp_hist_read);
}

void get_history_nth_command(int n, char * cmd)
{
	FILE * fp_hist_read = NULL;
	char 	str[CMD_LENGTH];
	char	nth_command[CMD_LENGTH];

	if(fp_hist_read = fopen(HISTORY_FILE_NAME, "r"))
	{
		int 		i = 0;

		while(fgets(str, CMD_LENGTH, fp_hist_read))
		{
			if( (++i) == n)
			{
				// copy without '\n'
				strncpy(nth_command, str, strlen(str));
			}
		}

		if(n < 1 || i < n){
			printf("Event not found\n");
			//return -1;
		}
	}
	else
	{
		printf("Cannot open history file\n");
	}
	
	trim_linefeed(nth_command);
	strcpy(cmd, nth_command);
	fclose(fp_hist_read);
}