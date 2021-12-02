#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>

int main(){
	int pid = fork();
	if(pid == 0){
		printf("Child is executing.\n");
	}
	else{
		wait(NULL);
		printf("Parent is executing.\n");
	}
}

