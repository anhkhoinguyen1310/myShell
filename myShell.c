#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "wildcard.h"
#include <fcntl.h>
#include <unistd.h>

#define MYSH_TOK_BUFSIZE 64
#define MYSH_TOK_DELIM " \t\r\n\a"

// Function Prototypes
void loop(void);
char *read_line(void);
void execute_with_pipe(char **, char **);
char **split_line(char *);
int execute(char **);
int myShell_num_builtins();
int myShell_cd(char **args);
int myShell_help(char **args);
int myShell_exit(char **args);

char *builtin_str[] = {
	"cd",
	"help",
	"exit"};

void loop(void)
{
	char *line;
	char **args;
	int status;

	do
	{
		printf("> ");
		line = read_line();
		args = split_line(line);
		status = execute(args);

		free(line);
		free(args);
	} while (status);
}
char *read_line(void)
{
	char *line = NULL;
	size_t bufsize = 0; // getline will allocate a buffer
	getline(&line, &bufsize, stdin);
	return line;
}

char **split_line(char *line)
{
	int bufsize = MYSH_TOK_BUFSIZE, position = 0;
	char **tokens = malloc(bufsize * sizeof(char *));
	char *token;

	if (!tokens)
	{
		fprintf(stderr, "myShell: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, MYSH_TOK_DELIM);
	while (token != NULL)
	{
		tokens[position++] = token;

		if (position >= bufsize)
		{
			bufsize += MYSH_TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char *));
			if (!tokens)
			{
				fprintf(stderr, "myShell: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, MYSH_TOK_DELIM);
	}
	tokens[position] = NULL;
	return tokens;
}

int myShell_cd(char **args)
{
	if (args[1] == NULL)
	{
		fprintf(stderr, "myShell: expected argument to \"cd\"\n");
	}
	else
	{
		if (chdir(args[1]) != 0)
		{
			perror("myShell");
		}
	}
	return 1;
}

int myShell_help(char **args)
{
	printf("myShell, the simplest shell\n");
	printf("Type program names and arguments, and hit enter.\n");
	printf("The following are built in:\n");

	for (int i = 0; i < myShell_num_builtins(); i++)
	{
		printf("  %s\n", builtin_str[i]);
	}

	printf("Use the man command for information on other programs.\n");
	return 1;
}

int myShell_exit(char **args)
{
	return 0;
}

int (*builtin_func[])(char **) = {
	&myShell_cd,
	&myShell_help,
	&myShell_exit};
int myShell_num_builtins()
{
	return sizeof(builtin_str) / sizeof(char *);
}

int execute(char **args)
{

	if (args[0] == NULL)
	{
		return 1; // Handle empty command
	}

	// Execute built-in commands
	for (int i = 0; i < myShell_num_builtins(); i++)
	{
		if (strcmp(args[0], builtin_str[i]) == 0)
		{
			return (*builtin_func[i])(args);
		}
	}

	// Wildcard expansion
	char **expandedArgs = NULL;
	int totalArgs = 0;
	expand_wildcards(*args, &expandedArgs, &totalArgs);
	char **execArgs = (expandedArgs != NULL) ? expandedArgs : args;
	int totalExecArgs = 0;

	// Allocate initial space for execArgs
	execArgs = malloc(sizeof(char *));
	execArgs[0] = NULL; // NULL-terminate the array

	for (int i = 0; args[i] != NULL; i++)
	{
		if (strchr(args[i], '*') != NULL)
		{ // Check if the argument contains a wildcard
			char **expandedArgs = NULL;
			int totalArgs = 0;
			expand_wildcards(args[i], &expandedArgs, &totalArgs); // Expand the wildcard

			// Append expandedArgs to execArgs
			execArgs = realloc(execArgs, sizeof(char *) * (totalExecArgs + totalArgs + 1)); // +1 for NULL termination
			for (int j = 0; j < totalArgs; j++)
			{
				execArgs[totalExecArgs++] = strdup(expandedArgs[j]); // Copy each expanded arg
			}
			free(expandedArgs); // Assuming expand_wildcards allocated expandedArgs
		}
		else
		{
			// No wildcard, just copy the argument directly
			execArgs = realloc(execArgs, sizeof(char *) * (totalExecArgs + 2)); // +1 for new arg, +1 for NULL termination
			execArgs[totalExecArgs++] = strdup(args[i]);
		}
		execArgs[totalExecArgs] = NULL; // NULL-terminate the array
	}

	// Prepare for execution

	int fd = -1;
	char *input_filename = NULL;
	char *output_filename = NULL;

	int redirect_out = 0, redirect_in = 0;

	// Check for redirection
	for (int i = 0; execArgs[i] != NULL; i++)
	{
		if (strcmp(execArgs[i], ">") == 0 && execArgs[i + 1] != NULL)
		{
			output_filename = execArgs[i + 1];
			execArgs[i] = NULL; // Effectively removes ">" and the filename from execArgs
		}
		else if (strcmp(execArgs[i], "<") == 0 && execArgs[i + 1] != NULL)
		{
			input_filename = execArgs[i + 1];
			execArgs[i] = NULL; // Removes "<" and the filename from execArgs
		}
	}

	// Check for pipes and handle them
	for (int i = 0; args[i] != NULL; i++)
	{
		if (strcmp(args[i], "|") == 0)
		{
			args[i] = NULL; // Split the command at the pipe symbol
			char **cmd1 = args;
			char **cmd2 = &args[i + 1];
			execute_with_pipe(cmd1, cmd2);
			return 1;
		}
	}
	printf("Executing command: %s\n", execArgs[0]);
	for (int i = 0; execArgs[i] != NULL; i++)
	{
		printf("Argument %d: %s\n", i, execArgs[i]);
	}

	pid_t pid = fork();
	if (pid == 0)
	{ // Child process
		// Input redirection
		if (input_filename)
		{
			int fd_in = open(input_filename, O_RDONLY);
			if (fd_in < 0)
			{
				perror("Failed to open input file");
				exit(EXIT_FAILURE);
			}
			dup2(fd_in, STDIN_FILENO);
			close(fd_in);
		}

		// Output redirection
		if (output_filename)
		{
			int fd_out = open(output_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd_out < 0)
			{
				perror("Failed to open output file");
				exit(EXIT_FAILURE);
			}
			dup2(fd_out, STDOUT_FILENO);
			close(fd_out);
		}

		// Execute the command
		if (execvp(execArgs[0], execArgs) == -1)
		{
			perror("execvp");
			exit(EXIT_FAILURE);
		}
	}
	else if (pid < 0)
	{
		perror("Failed to fork");
		return -1; // Indicate fork failure
	}
	else
	{
		// Parent process waits for child
		int status;
		waitpid(pid, &status, 0);
	}

	// Cleanup
	if (expandedArgs != NULL)
	{
		for (int i = 0; i < totalArgs; i++)
		{
			free(expandedArgs[i]);
		}
		free(expandedArgs);
	}
	return 1; // Success
}

void execute_with_pipe(char **cmd1, char **cmd2)
{
	int pipefd[2];
	pid_t pid1, pid2;

	if (pipe(pipefd) < 0)
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	pid1 = fork();
	if (pid1 == 0)
	{
		// First child - writes to the pipe
		close(pipefd[0]);				// Close the read end
		dup2(pipefd[1], STDOUT_FILENO); // Replace stdout with the write end of the pipe
		close(pipefd[1]);				// Close the write end after dup2

		if (execvp(cmd1[0], cmd1) < 0)
		{
			perror("execvp");
			exit(EXIT_FAILURE);
		}
	}

	pid2 = fork();
	if (pid2 == 0)
	{
		// Second child - reads from the pipe
		close(pipefd[1]);			   // Close the write end
		dup2(pipefd[0], STDIN_FILENO); // Replace stdin with the read end of the pipe
		close(pipefd[0]);			   // Close the read end after dup2

		if (execvp(cmd2[0], cmd2) < 0)
		{
			perror("execvp");
			exit(EXIT_FAILURE);
		}
	}

	// Parent closes both ends of the pipe and waits for children
	close(pipefd[0]);
	close(pipefd[1]);
	waitpid(pid1, NULL, 0);
	waitpid(pid2, NULL, 0);
}

int main(int argc, char **argv)
{
	printf("Welcome to my shell!\n");

	// Run command loop
	loop();

	printf("Exiting my shell.\n");

	return EXIT_SUCCESS;
}