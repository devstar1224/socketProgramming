#include <stdio.h>

int main(){
	int a = 10;
	int b = 100;
	a = a + 10;
	int pid = fork();
	//fork is clone now work process
	//create to fork() function, make child process
	//if return result 0 that is child process
	//if return result child process pid value that is parent process.
	//if fail to fork, return -1 value
	

	if(pid > 0){
		printf("parent\n");
		a = a + 10;
		printf("[parent] a = %d b = %d \n", a, b);
	} else if(pid == 0){
		printf("child\n");
		b = b * 10;
		printf("[child] a = %d b = %d \n", a ,b);
	}else{
		printf("fail to fork");
	}

}
