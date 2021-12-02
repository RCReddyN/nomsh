#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>

int main(){
	fork();
	printf("Hello fork!\n");
	return 0;
}
