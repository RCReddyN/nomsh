#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<limits.h>
#include<string.h>
#include<fcntl.h>
#include<sys/wait.h>
#include "trie.h"

#define BUFFER_SIZE 1024
#define TOKEN_SIZE 64
#define TOKEN_DELIMITER " \t\r\n\a"

struct Trie* builtins;

void changeDirectoryCommand(char **args){
    	if(args[1] == NULL){
        	fprintf(stderr, "nomsh: expected argument for %s\n", args[0]);
    	}	
    	else{
        	if(chdir(args[1]) != 0){
            		perror("nomsh");
        	}
    	}
}

void buildBuiltinCommandTrie(){
	builtins = createTrieNode();
	insert(builtins, "cd");
	insert(builtins, "history");
	insert(builtins, "jobs");
	insert(builtins, "kill");
	insert(builtins, "exit");
}

int isBuiltinCommand(char* cmd){
	return search(builtins, cmd);
}

void executeBuiltinCommand(char** args){
	if(strcmp("cd", args[0]) == 0){
		changeDirectoryCommand(args);
	}
}

void runexec(char **args, int output_redirection){
	int pid = fork();
	if(pid == 0){
		if(output_redirection > 0){
			char *outputfile = (char*) malloc(sizeof(char)*64);
			strcpy(outputfile, args[output_redirection+1]);
			int fd = open(outputfile, O_CREAT |  O_TRUNC | O_RDWR, S_IRWXU | S_IRWXG);
			if(fd < 0){
				fprintf(stderr, "nomsh: Error opening file\n");
			}
			dup2(fd, 1);
			close(fd);
			args[output_redirection] = NULL;
		}
		if(execvp(args[0], args) == -1){
			perror("nomsh");
		}
	}
    	else if(pid < 0){
	    	perror("nomsh");
    	}
    	else{    
        	while(wait(NULL) > 0);
    	}
}

int isOutputRedirected(char **args){
	int index = -1;
	for(int i=0; args[i] != NULL; i++){
		if(strcmp(">", args[i]) == 0){
			index = i;
			break;
		}
	}
	return index;
}

int countArguments(char **args){
	int c = 0;
	for(int i=0; args[i] != NULL; i++){
		c++;
	}
	return c;
}

void executeCommand(char **args){
	if(args[0] == NULL){
		return;
	}
	
	int argNum = countArguments(args);
	int output_redirection = isOutputRedirected(args);

	if(output_redirection != -1 && argNum - output_redirection != 2){
		fprintf(stderr, "nomsh: Multiple files found after >\n");
		return;
	}

    	else if(isBuiltinCommand(args[0])){
		executeBuiltinCommand(args);
	}
	else{
		runexec(args, output_redirection);
	}
}

char *read_line(){
	int buffer_size = 1024;
	int position = 0 ;
	char *buffer = malloc(sizeof(char) * buffer_size);
	char ch;

	while((ch = getchar()) != '\n' && ch != EOF){
		buffer[position] = ch;
		position++;
	}
	buffer[position] = '\n';
	buffer[position+1] = '\0';
	return buffer;
}


char **split_line(char *line){
	int token_size = 64;
	int max_tokens = 64;
	char **tokens = (char **)malloc(sizeof(char*) * token_size);
	char *token = (char *)malloc(sizeof(char) * max_tokens);
	int tokenIndex =0;
	int tokenNum = 0;

	for(int i=0; i<strlen(line); i++){
		char ch = line[i];
		if(ch == ' ' || ch == '\t' || ch == '\v' || ch == '\a' || ch == '\n'){
			token[tokenIndex] = '\0';
			if(tokenIndex != 0){
				tokens[tokenNum] = (char*)malloc(sizeof(char)*token_size);
				strcpy(tokens[tokenNum++], token);
				tokenIndex = 0;
			}
		}
		else{
			token[tokenIndex++] = ch;
		}
	}
	free(token);
	tokens[tokenNum] = NULL;
	return tokens;
}

void printPrompt(){
    	printf("$ ");
}

int main(){
    
	char *line;
    	char **args;
	buildBuiltinCommandTrie();    
    	while(1){
        	printPrompt();
        	line = read_line();
        	args = split_line(line);
        	executeCommand(args);
        	free(line);
        	free(args);
    	}
}
