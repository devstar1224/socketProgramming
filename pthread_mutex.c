#include <stdio.h>
#include <pthread.h>

int ncount = 0;

void *do_sum2(void *data){
	int i ;
	int n = *((int * ) data);
	for(i=0;i<n;i++){
		printf("[%x] sum : %d \n " ,pthread_self(), ncount);
		ncount++;
	}

}

void main(){
	pthread_t thread_id[3];
	int status[3];
	int a = 5;
	int b = 10;
	int c = 20;
	status[0] = pthread_create(&thread_id[0], NULL, do_sum2, (void *) &a);
	status[1] = pthread_create(&thread_id[0], NULL, do_sum2, (void *) &b);
	status[2] = pthread_create(&thread_id[0], NULL, do_sum2, (void *) &c);
	
	pthread_join(thread_id[0], (void **)&status[0]);
	pthread_join(thread_id[1], (void **)&status[1]);
	pthread_join(thread_id[2], (void **)&status[2]);

	printf("Main thread\n");	
}
