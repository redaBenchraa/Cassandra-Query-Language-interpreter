#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<stdio.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include <fcntl.h>
#define MAX_RECV_BUF 256
#define MAX_SEND_BUF 256

// Rewriting puts() function so it takes in considitation ';' as the end of the request and any string that starts with '@' as a file 
void getQuery(char *buf)
{
	//printf("CQL# ");
	int c;
	int breakChar = ';'; 
	char *s = buf;
	while((c = getchar()) != breakChar){
		if((c=='\b' && s!='\0')) s--;
		if(c == '@'){
			breakChar = '\n';
		}
		if (c == EOF) break;
		else *s++ = c;
	}
	if( breakChar == ';') *s++ = ';';
	*s = 0;
}
int main(){

	//Variables 
	char* reqt = calloc(1024,sizeof(char));	
	char* reps = calloc(1024,sizeof(char));
	char* serverCharAddr = calloc(100,sizeof(char));
	char* serverCharPort = calloc(100,sizeof(char));
	int serverIntPort = 0;
	char* serverCharNom = calloc(100,sizeof(char));
	char* serverCharPass = calloc(100,sizeof(char));
	FILE *f = NULL;  

	while(1){
	// Server's Addr and Port to connect throught sockets and Username/Pass for DB
   	printf("Entrer l'addresse du serveur : ");
    scanf("%s",serverCharAddr);
    printf("Entrer le port du serveur : ");
    scanf("%s",serverCharPort);
    printf("Entrer le nom d'utilisateur : ");
    scanf("%s",serverCharNom);
    printf("Entrer le mot de passe : ");
    scanf("%s",serverCharPass);
  	serverIntPort = strtoul (serverCharPort, NULL, 0);


  	/*serverIntPort = 1234;
  	serverCharAddr = "127.0.0.1";
  	serverCharNom = "reda";
  	serverCharPass ="ben";*/

  	// Init socket variables
    struct sockaddr_in addserv;
    addserv.sin_family = AF_INET;
    inet_aton(serverCharAddr,&(addserv.sin_addr));
    addserv.sin_port = htons(serverIntPort);
    memset(&(addserv.sin_zero),'0',8);

    //Create socket descriptor
	int ds = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	//Connect to server
	if(connect(ds,(struct sockaddr*) &addserv,sizeof(struct sockaddr))!=-1){

		//Send username and password
		send(ds,serverCharNom,256,0);
		send(ds,serverCharPass,256,0);
		if(recv(ds,reps,256,0)>0){
			// Wrong username and password
			if(strcmp(reps,"rejected") == 0){
				printf("\nWrong username or password, retry ! \n");
				close(ds);
				continue;
			}
		}
		if(fork()==0){
			while(1){

				// Get query from input
				getQuery(reqt);
				if(strcmp(reqt,"clear;") == 0 || strcmp(reqt,"\nclear;") == 0) 
				{
					system("clear");
				}		
				else if(strcmp(reqt,"exit;") == 0 || strcmp(reqt,"\nexit;") == 0) 
				{
					exit(0);
				}
				else
				{
	    			if(reqt[0] == '\n'){
						reqt++;
					}

					// Check if it's a file
					if(reqt[0] == '@'){
						reqt++;
						f = fopen(reqt,"r");
						if(f == NULL){
							printf("File does not exist\n\n");
						}else{	
						fseek(f, 0L, SEEK_END);
						int sz = ftell(f);
						// Send file size
						sprintf(reps, "%d\n",sz);
						send(ds,reps,256,0);
						fclose(f);
						// Send file
						send_file(ds, reqt);
						printf("Query Result\n####################################\n\n");
						}
					}else{

						// Create a tmp file to put the request in
						f = fopen("queryTmp","w");
						fprintf(f, "%s\n",reqt );
						fseek(f, 0L, SEEK_END);
						int sz = ftell(f);
						// Send file size
						sprintf(reqt, "%d\n",sz);
						send(ds,reqt,256,0);
						fclose(f);
						// Send file
						send_file(ds, "queryTmp");
						printf("Query Result\n####################################\n\n");
					}
					//close(ds);
				}
	            reqt = calloc(1024,sizeof(char));	
			}
		}else{
			while(recv(ds,reps,500,0)>0){
			 	printf("%s",reps);	
			}
		}
	}else printf("\nCouldn't connect to %s:%d\n",serverCharAddr,serverIntPort);
	close(ds);
	}
    return 0;
}
int send_file(int sock, char *file_name)
{
    int sent_count;
    ssize_t read_bytes, 
    sent_bytes,
    sent_file_size;
    char send_buf[MAX_SEND_BUF];
    int f;
    sent_count = 0;
    sent_file_size = 0;
    if( (f = open(file_name, O_RDONLY)) < 0)
    {
        perror(file_name);
    }
    else
    {
        while( (read_bytes = read(f, send_buf, MAX_RECV_BUF)) > 0 )
        {
        if( (sent_bytes = send(sock, send_buf, read_bytes, 0)) < read_bytes )
        {
        	perror("send error");
        	return -1;
        }
        sent_count++;
        sent_file_size += sent_bytes;
        }
        close(f);
    }
    return sent_count;
}