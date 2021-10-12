#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<limits.h>
#include<string.h>
#include<sys/wait.h>

#define BUFFER_SIZE 1024
#define TOKEN_SIZE 64
#define TOKEN_DELIMITER " \t\r\n\a"

int changeDirectoryCommand(char **args){
    if(args[1] == NULL){
        fprintf(stderr, "nomsh: expected argument to cd.\n");
    }
    else{
        if(chdir(args[1]) != 0){
            perror("nomsh");
        }
    }
    return 1;
}

int echoCommand(char **args){
    int arglen = sizeof(args)/sizeof(char*);
    for(int i=1; i<arglen-2; i++){
        printf("%s ", args[i]);
    }
    printf("%s\n", args[arglen-2]);
}

int helpCommand(){
    printf("NOMSH\n");
	printf("Type command followed by arguments and hit enter.\n");
	printf("cd, echo, mkdir, cwd, rmdir, help, and exit are currently supported.\n");
	return 1;
}

int currentWorkingDirectoryCommand(){
    char *cwd;
    char buff[PATH_MAX + 1];

    cwd = getcwd(buff, PATH_MAX + 1);
    if(cwd != NULL){
        printf("Currently working in %s\n", cwd);
    }
    return 1;
}


int exitShell(){
    return 0;
}

int launchProcess(char **args){
    int pid = fork();
    int status, wpid;

    if(pid == 0){
        if(execvp(args[0], args) == -1){
            fprintf(stderr, "nomsh: child process unsuccessfully executed.\n");
        }
    }
    else if(pid < 0){
        fprintf(stderr, "nomsh: forking error.\n");
    }
    else{
        do{
            wpid = waitpid(pid, &status, WUNTRACED);
        }    
        while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

int executeCommand(char **args){
    if(args[0] == NULL){
        return 1;
    }
    else if(strcmp("cd", args[0]) == 0){
        return changeDirectoryCommand(args);
    }
    
    else if(strcmp("echo", args[0]) == 0){
        return echoCommand(args);
    }

    else if(strcmp("help", args[0]) == 0){
        return helpCommand(args);
    }


    else if(strcmp("cwd", args[0]) == 0){
        return currentWorkingDirectoryCommand(args);
    }


    else if(strcmp("exit", args[0]) == 0){
        return exitShell(args);
    }
    return launchProcess(args);
}

char *read_line(){
	int buffer_size = BUFFER_SIZE;
	int position = 0 ;
	char *buffer = malloc(sizeof(char) * buffer_size);
	int c =0;

	if(!buffer){
		fprintf(stderr, "nomsh: allocation error\n");
		exit(EXIT_FAILURE);
	}
	
	while(1){
		c = getchar();
		if(c == EOF || c == '\n'){
			buffer[position] = '\0';
			return buffer;
		}
		buffer[position] = c;
		position++;

		if(position >= buffer_size){
			buffer_size += BUFFER_SIZE;
			buffer = realloc(buffer, buffer_size);
			if(!buffer){
				fprintf(stderr, "nomsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}


char **split_line(char *line){
	int buffer_size = TOKEN_SIZE;
	int position = 0;
	char **tokens = malloc(sizeof(char*) * buffer_size);
	char *token;

	if(!tokens){
		fprintf(stderr, "nomsh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, TOKEN_DELIMITER);
	while(token != NULL){
		tokens[position] = token;
		position++;

		if(position >= buffer_size){
			buffer_size += TOKEN_SIZE;
			tokens = realloc(tokens, buffer_size * sizeof(char*));
			if(!tokens){
				fprintf(stderr, "nomsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, TOKEN_DELIMITER);
	}
	tokens[position] = NULL;
	return tokens;
}

void printPrompt(){
    printf("> ");
}

void loop(){
    char *line;
    char **args;
    int status = 1;
    
    while(status){
        printPrompt();
        line = read_line();
        args = split_line(line);
        status = executeCommand(args);

        free(line);
        free(args);
    }
}

int main(int argc, char*argv[]){
    loop();
    return EXIT_SUCCESS;
}