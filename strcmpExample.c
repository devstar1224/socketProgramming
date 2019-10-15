#include <stdio.h>
#include <string.h>

void main(){
	char buffer[100] = "strcmp str1 str2";
	char *token;
	char  *str[3];
	int idx = 0;
	token = strtok(buffer," ");
	while(token != NULL){
		str[idx] = token;
		printf("str[%d] = %s\n", idx, str[idx]);
		idx++;
		token = strtok(NULL, " ");
	}
	if(idx < 3){
		strcpy(buffer,"need more argument");
	}else if (!strcmp(str[1], str[2])){
		sprintf(buffer, "%s equal %s",str[1], str[2]);
	}else{
		sprintf(buffer, "%s not equal %s", str[1], str[2]);
	}
	printf("%s\n", buffer);
}
