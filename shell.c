#include<stdio.h> 
#include<string.h> 
#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>
#include<limits.h>
#define LINE_BUFFER_SIZE 1024
#define TOKEN_BUFFER_SIZE 64
#define TOKEN_DELIMITER " \t\r\n\a"

int sh_cd(char **args);
int sh_help(char **args);
int sh_exit(char **args);

int sh_cd(char **args){
	if(args[1] == NULL){
		fprintf(stderr, "nomsh: expected argument to \"cd\"\n");
	}
	else{
		if(chdir(args[1]) != 0){
			perror("nomsh");
		}
	}
	return 1;
}

int sh_help(char **args){
	printf("NOMSH\n");
	printf("Type command followed by arguments and hit enter.\n");
	printf("cd, help, and exit are currently supported.\n");
	return 1;
}

int sh_exit(char ** args){
	return 0;
}


char *read_line(){
	int buffer_size = LINE_BUFFER_SIZE;
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
			buffer_size += LINE_BUFFER_SIZE;
			buffer = realloc(buffer, buffer_size);
			if(!buffer){
				fprintf(stderr, "nomsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}


char **split_line(char *line){
	int buffer_size = TOKEN_BUFFER_SIZE;
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
			buffer_size += TOKEN_BUFFER_SIZE;
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

int sh_launch(char **args){
	pid_t pid, wid;
	int status;

	pid = fork();
	if(pid == 0){
		if(execvp(args[0], args) == -1){
			perror("nomsh");
		}
		exit(EXIT_FAILURE);
	}
	else if(pid < 0){
		perror("nomsh");
	}
	else{
		do{
			wid = waitpid(pid, &status, WUNTRACED);
		}
		while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	
	return 1;
}

int sh_execute(char **args){
	if(args[0] == NULL){
		return 1;
	}
	if(strcmp("cd", args[0]) == 0){
		return sh_cd(args);
	}
	else if(strcmp("help", args[0]) == 0){
		return sh_help(args);
	}
	else if(strcmp("exit", args[0]) == 0){
		return sh_exit(args);
	}

	return sh_launch(args);
}



void loop(){
	char *line;
	char **args;
	int status = 1;

	while(status){
		printf("> ");
		line = read_line();
		args = split_line(line);
		status = sh_execute(args);
	
		free(line);
	        free(args);
	}
}

int main(){
	loop();
	return 0;
}
