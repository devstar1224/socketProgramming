#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

#define PORT 10000

char buffer[100] = "Hi, I'm server.\n";
char rcvBuffer[100];
int main(){
	int c_socket, s_socket;
	struct sockaddr_in s_addr, c_addr;
	int len;
	int n;

	s_socket = socket(PF_INET, SOCK_STREAM, 0);
	
	memset(&s_addr, 0, sizeof(s_addr)); 
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(PORT);


	if(bind(s_socket,(struct sockaddr *) &s_addr, sizeof(s_addr)) == -1){ 
		printf("Cannot Bind\n");
		return -1;
	}
	
	if(listen(s_socket, 5) == -1){
		printf("listen Fail\n");
		return -1;
	}
	while(1){
		len = sizeof(c_addr);
		printf("Wait connect client\n");
		c_socket = accept(s_socket, (struct sockaddr *)&c_addr, &len); 
		printf("/client is connected\n");
		while(1){
			n = read(c_socket, rcvBuffer, sizeof(rcvBuffer));
			printf("rcvBuffer: %s\n", rcvBuffer);
			
			if(!strncasecmp(rcvBuffer, "hello", 5)){
				char rcvBf[100] = "Hello, nice meet you";
				write(c_socket, rcvBf, strlen(rcvBf));	
			}else if(!strncasecmp(rcvBuffer, "what is your name?", 18)){
				char rcvBf[100] = "My name is Linux!";
				write(c_socket, rcvBf, strlen(rcvBf));
			}else if(!strncasecmp(rcvBuffer, "How old are you?", 16)){
				char rcvBf[100] = "I'm 59 years old";
				write(c_socket, rcvBf, strlen(rcvBf));
			}else if(!strncasecmp(rcvBuffer, "quit", 4)|| !strncasecmp(rcvBuffer, "kill server", 11)){
				break;
			}else
			 write(c_socket, "Error", 5); 
		}

		close(c_socket);
		if (strncasecmp(rcvBuffer, "kill server", 11) == 0)
			break;
	}
	close(s_socket);
	return 0;	
}
