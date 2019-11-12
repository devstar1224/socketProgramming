#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#define PORT 10000

void sig_handler();
void message(int c_socket);
char * delsp(char * dest, char * cmd);
// char rwFile(char *fileName, char *mode, char *content);
int cmp(char *str);
char buffer[100] = "Hi, I'm server.\n";
char rcvBuffer[100];
int num = 0;
int main(){
	int c_socket, s_socket;
	struct sockaddr_in s_addr, c_addr;
	int len;
	int n;
	signal(SIGCHLD, sig_handler);
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
	int pid;
	while(1){
		len = sizeof(c_addr);
		printf("Wait connect client\n");
		c_socket = accept(s_socket, (struct sockaddr *)&c_addr, &len); 
		num++;
		if((pid = fork())> 0){
			printf("/client is connected now connected => %d\n", num);
			close(c_socket);
			continue;
		}else if(pid == 0){
			message(c_socket);
			close(s_socket);
			exit(0);
		}
	}
	close(s_socket);
	return 0;	
}

void message(int c_socket){
	int n;
	while(1){
			n=read(c_socket, rcvBuffer, sizeof(rcvBuffer));
			printf("rcvBuffer: %s\n", rcvBuffer);

			if(!strncmp(rcvBuffer, "hello", 5)){
				char rcvBf[100] = "Hello, nice meet you";
				write(c_socket, rcvBf, strlen(rcvBf));	
			}

			else if(!strncmp(rcvBuffer, "what is your name?", 18)){
				char rcvBf[100] = "My name is Linux!";
				write(c_socket, rcvBf, strlen(rcvBf));
			}

			else if(!strncmp(rcvBuffer, "how old are you?", 16)){
				char rcvBf[100] = "I'm 59 years old";
				write(c_socket, rcvBf, strlen(rcvBf));
			}

			else if(!strncmp(rcvBuffer, "quit", 4)|| !strncmp(rcvBuffer, "kill server", 11)){
				break;
			}

			else if(!strncmp(rcvBuffer,"strlen", 6)){
				char * bf = delsp(rcvBuffer, "strlen");
				char chlen_str[100];
				sprintf(chlen_str,"%d\n",strlen(bf)-2);
				strcpy(bf, chlen_str);
				write(c_socket, bf, strlen(bf));
			}
			else if(!strncmp(rcvBuffer,"strcmp", 6)){
				char * bf = delsp(rcvBuffer, "strcmp");
				int last = cmp(bf);
				sprintf(bf, " %d\n", last);
				write(c_socket, bf, strlen(bf));		
			}else if(!strncmp(rcvBuffer, "readfile ", 9)){
				char * token;
				token = strtok(rcvBuffer, " ");
				token = strtok(NULL, " ");
				FILE * fp = fopen(token, "r");
				char sendBf[100];
				char bf[100];
				if(fp){
					memset(sendBf, 0, 100);
					while(fgets(bf, 100, (FILE *) fp)){
						strcat(sendBf, bf);
					}
				}
				fclose(fp);
				write(c_socket, sendBf, strlen(sendBf));
			}else if(!strncmp(rcvBuffer, "exec ", 5)){
				char * command;
				char * token;
				token = strtok(rcvBuffer," ");
				strtok(NULL, "\0");
				printf("command : %s", command);
				int result = system(command);
				if(!result){
					sprintf(buffer, "[%s] Execute finish", command);	
				}else{
					sprintf(buffer, "[%s] Execute failed", command);
				}
				write(c_socket, buffer, strlen(buffer));
			}
			else{
			 write(c_socket, "Error", strlen(rcvBuffer)); 
			}
		}

}

char * delsp(char * dest, char * cmd){
	int n = 0;
	while( * dest == ' ' || *cmd == ' '){
		n++;
		dest++;
		cmd++;
	}
	while(*dest!='\0' && *cmd != '\0' && *dest == *cmd){
		n++;
		dest++;
		cmd++;
	}
	return dest;
}

int cmp(char *str){
	int result;
	char * t1 = strtok(str," ");
	str = strtok(NULL, "\n");
	result = strcmp(t1, str);
	return result;
}

void sig_handler(int signo){
	int pid;
	int status;
	pid = wait(&status);
	printf("pid[%d] is terminated.status = %d\n", pid, status);
	num--;
	printf("now connected client : %d \n", num);
}
