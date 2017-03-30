/*
// 2/12/2017
// Pei-Tzu Chen
//
// It’s an implementation of a simple command line shell in C 
// that interacts with user and has some of the limitations as below: 
// •    Enter single line commands 
// •	Arguments separated by white space only
// •	Built in cd command
// •	No piping and redirection
// •	Exit with ctr+c
// •   Compile with gcc
// Tested on MAC terminal with several commands including ls, cat, grep, head, date, man, file and cd …etc.  
//
*/

#include <stdio.h>
#include <string.h> // strcmp, strtok
#include <stdlib.h> // free, malloc, realloc, exit(EXIT_SUCCESS, EXIT_FAILURE)
#include <unistd.h> // fork
#include <sys/wait.h>

// Read a line from stdin
char *ReadLine(){
	int index = 0, bufsize = 1024;
	char *buffer = (char*) malloc(sizeof(char) * bufsize);
	int c;

	while (1) {
		c = getchar();
		
		if(c == '\n' || c == EOF){
			buffer[index] = '\0';
			return buffer;
		}
		else{
			buffer[index] = c;
		}
		index++;

		if (index >= bufsize) {
      		bufsize += bufsize;
      		buffer = realloc(buffer, bufsize);
      			if (!buffer) {
        			perror("The following error occured: ");
        			exit(EXIT_FAILURE);
      			}
        }
    }
}

// Split the line into tokens 
char **SplitLine(char *line){
	int index = 0, bufsize = 64;
	char *token;
	char **tokens = (char**) malloc(sizeof(char) * bufsize);

	token = strtok(line, " "); 
	while(token != NULL){
		tokens[index] = token;
		index++;
		if(index >= bufsize){
			bufsize += bufsize;
			tokens = realloc(tokens, bufsize);
				if(!tokens){
					perror("The following error occured: ");
					exit(EXIT_FAILURE);
				}
		}
		token = strtok(NULL, " \t\r\n\a");
	}
	tokens[index] = NULL;
	return tokens;
}

// Start create new process and execute 
int LaunchProcess(char **args){
	pid_t pid; 

	if(strcmp(args[0], "cd") == 0){
		if(args[1] == NULL){
			perror("The following error occured: ");
		}
		else{
			if(chdir(args[1]) != 0){
				perror("The following error occured: ");
			}
		}
		return 1;
	}
	else if(strcmp(args[0], "exit") == 0){
		return EXIT_SUCCESS;
	}
    else{
		pid = fork(); // create child process
		if(pid == 0){
			if(execvp(args[0], args) == -1){
				perror("The following error occured: ");
			}
			exit(EXIT_FAILURE);
		}
		else if(pid < 0){
			perror("The following error occured: ");
		}
		else{
			// parent waits until the child process is done 
			waitpid(-1, NULL, 0);
		}
	    return 1;
	}
}

int main(int argc, char *argv[]){
	
	char *line;
	char **arguments;
	int status;

	do{
		printf("$< ");
		line = ReadLine();
		arguments = SplitLine(line);
		status = LaunchProcess(arguments);
		free(line);
		free(arguments);
	} while(status);
}
