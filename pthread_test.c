#include <stdio.h>
#include <stdlib.h>

void * func_thread(void *);

void main(){
	int status;
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, func_thread,NULL);
	printf("thread %x is created\n", thread_id);
	printf("main thread END \n");

}

void * func_thread(void * argv){
	int i ;
	while(i<10){
		i++;
		printf("thread %d executing\n", i);
	}
}
