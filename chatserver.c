#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>

#define IPADDR "127.0.0.1"
#define PORT 9000
#define CHATDATA 1024
#define MAX_CLIENT 10
#define INVALID_SOCK -1

typedef struct s_user{
	int c_socket;
	char nickname[20];
	char room[20];
}user;

void *doChat(void *);
int pushClient(int);
int popClient(int);

pthread_t th;
pthread_mutex_t mutex;

user listClient[MAX_CLIENT];
char greeting[] = "SERVER REQ : Welcome\n";
char CODE200[] = "SERVER REQ : FULL\n";

int main(int argc, int *argv[]){
	int c_socket, s_socket;
	struct sockaddr_in s_addr, c_addr;
	int len;
	int i;
	int th_id;
	int res;

	s_socket = socket(PF_INET, SOCK_STREAM, 0);

	memset(&s_addr, 0, sizeof(s_addr));
	s_addr.sin_addr.s_addr = inet_addr(IPADDR);
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(PORT);

	if(bind(s_socket, (struct sockaddr *) &s_addr, sizeof(s_addr))==-1){
		printf("[ERROR] Can not bind.\n");
	}
	if(listen(s_socket, 5)==-1){
		printf("[ERROR] Listen fail.\n");
	}

	for(i=0; i<MAX_CLIENT; i++){
		listClient[i].c_socket = INVALID_SOCK;
	}
	while(1){
		len = sizeof(c_addr);
		c_socket = accept(s_socket, (struct sockaddr *) &c_addr, &len);
		res = pushClient(c_socket);
		if(res < 0){
			write(c_socket, CODE200, strlen(CODE200));
			close(c_socket);
		}
		else{
			write(c_socket, greeting, strlen(greeting));
			th_id = pthread_create(&th, NULL, doChat, (void*)&c_socket);
		}
	}
}

void *doChat(void *arg){
	int c_socket = *((int *)arg);
	char nickname[20];
	char room[20] = "all";
	char chatData[CHATDATA];
	char writeData[CHATDATA];
	int i, n;
	char *ptr;
	char target[20];
	for(i=0; i<MAX_CLIENT; i++){
		if(listClient[i].c_socket==c_socket){
			strcpy(nickname, listClient[i].nickname);
		}
	}
	while(1){
		memset(chatData, 0, sizeof(chatData));
		if((n = read(c_socket, chatData, sizeof(chatData)))>0){
			sprintf(writeData, "[room:%s] %s : %s", room, nickname, chatData);
			if(strstr(chatData, "/exit") != NULL){
				popClient(c_socket);
				break;
			}
			else if(strncasecmp(chatData, "/list", 5)==0){
				sprintf(writeData, "===connected list===\n");
				write(c_socket, writeData, strlen(writeData));
				for(i=0; i<MAX_CLIENT; i++){
					if(listClient[i].c_socket!=INVALID_SOCK){
						sprintf(writeData, "[%s] now : %s\n", listClient[i].nickname, listClient[i].room);
						write(c_socket, writeData, strlen(writeData));
					}
				}
			}
			else if(strncasecmp(chatData, "/w", 2)==0){
				strtok(chatData, " ");
				strcpy(target, strtok(NULL, " "));
				strcpy(chatData, strtok(NULL, "\0"));
				sprintf(writeData, "[%s wisper] %s", nickname, chatData);
				for(i=0; i<MAX_CLIENT; i++){
					if(strcasecmp(target, listClient[i].nickname)==0){
						write(listClient[i].c_socket, writeData, strlen(writeData));
					}
				}
			}
			else if(strncasecmp(chatData, "/join", 5)==0){
				strtok(chatData, " ");
				strcpy(room, strtok(NULL, "\n"));
				sprintf(writeData, "[%s is connect room %s.]\n", nickname, room);
				for(i=0; i<MAX_CLIENT; i++){
					if(listClient[i].c_socket==c_socket){
						strcpy(listClient[i].room, room);
					}
				}
				for(i=0; i<MAX_CLIENT; i++){
					if(strcasecmp(listClient[i].room, room)==0){
						write(listClient[i].c_socket, writeData, strlen(writeData));
					}
				}
			}
			else{
				for(i=0; i<MAX_CLIENT; i++){
					if(listClient[i].c_socket!=INVALID_SOCK){
						if(strcasecmp(listClient[i].room, room)==0){
							write(listClient[i].c_socket, writeData, strlen(writeData));
						}
					}
				}
			}
		}
	}
}

int pushClient(int c_socket){
	int i, j;
	char buf[20];
	char writeData[CHATDATA];

	memset(buf, 0, sizeof(buf));
	read(c_socket, buf, sizeof(buf));
	for(i=0; i<MAX_CLIENT; i++){
		if(listClient[i].c_socket==INVALID_SOCK){
			listClient[i].c_socket = c_socket;
			memset(listClient[i].nickname, 0, sizeof(listClient[i].nickname));
			strcpy(listClient[i].nickname, buf);
			memset(listClient[i].room, 0, sizeof(listClient[i].room));
			strcpy(listClient[i].room, "all");

			sprintf(writeData, "Hello sir [%s]!\n", buf);
			for(j=0; j<MAX_CLIENT; j++){
				if(listClient[j].c_socket!=INVALID_SOCK){
				write(listClient[j].c_socket, writeData, strlen(writeData));
				}
			}
			return 0;
		}
	}

	return -1;
}

int popClient(int c_socket){
	int i, j;
	char writeData[CHATDATA];
	for(i=0; i<MAX_CLIENT; i++){
		if(listClient[i].c_socket==c_socket){
			sprintf(writeData, "Disconnect [%s]\n", listClient[i].nickname);
			for(j=0; j<MAX_CLIENT; j++){
				if(listClient[j].c_socket!=INVALID_SOCK){
					write(listClient[j].c_socket, writeData, strlen(writeData));
				}
			}
			listClient[i].c_socket = INVALID_SOCK;
			strcpy(listClient[i].nickname, "\0");
			strcpy(listClient[i].room, "\0");

		}
	}
	close(c_socket);
}

