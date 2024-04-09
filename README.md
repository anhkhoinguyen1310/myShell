## **Overview:**


---



The provided shell.c code constitutes a basic shell program named "myShell." It imports necessary header files and defines constants for token buffer size and delimiters. The program implements three built-in commands: ‘cd’, ‘help’, and ‘exit’. The loop function repeatedly prompts users for input, reads the input line, and splits it into arguments to execute commands. Additionally, there are functions for reading input lines (read_line) and splitting lines into tokens (split_line). The built-in commands' functionalities include changing the directory (cd), displaying help information (help), and exiting the shell (exit). The execution function (execute) handles both built-in and external commands, including wildcard expansion, input/output redirection, and piping. The ‘execute_with_pipe’ function facilitates the execution of commands involving pipes. The main function initializes the shell, enters the command loop, and terminates upon user command to exit. In essence, the code provides a fundamental shell environment with support for executing commands, handling built-in functionalities, and managing input/output redirection and pipes.

# **Wildcard**


---


The wildcard.c function implements the ‘expand_wildcards’ function, which is intended to expand wildcard patterns in command-line arguments. It includes the necessary header file ‘wildcard.h’ and standard C library headers. The ‘expand_wildcards’ function takes a wildcard string, a pointer to a pointer for expanded arguments, and a pointer to an integer for the number of expanded arguments as parameters. Within the function, it utilizes the glob function from the standard C library to perform wildcard expansion. If wildcard expansion is successful (ret == 0), it allocates memory for the expanded arguments array, assigns the count of expanded arguments, and duplicates each expanded argument string. If wildcard expansion fails, it prints an error message using error. Finally, it frees the memory allocated for the glob results using globfree. This function serves as a crucial addition to the shell program, enabling support for wildcard expansion in command-line arguments, thus enhancing the functionality and usability of the shell.

We attempted to to retrieve a list of files starting with m* with the following command ls -l m*

# **Redirection**


---



Redirection attempts to write the list of items into the testfile by using echo "Apple Banh Mi Beans Candy Mango Mango Pho" > test.txt

-> cat < test.txt 
- We asked the task of the output the file out to the terminal
- A single command can redirect both standard input and standard output.
	- grep Apple < test1.txt > test2.txt

#**Pipes**
---
To demonstrate the functionality of pipes, we utilized the command to sort the contents of the "test.txt" file in reverse alphabetical order. 

Specifically, we employed the command "cat test.txt | sort -r". This command first outputs the contents of the "test.txt" file using cat, then pipes the output to sort -r to perform a reverse alphabetical sorting of the text.
 






