#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <jansson.h>
#include "parser.h"
#include "interpreter.h"
#include "resultHandler.h"

#define MAX_RECV_BUF 256
#define MAX_SEND_BUF 256
int main(){

    // Setting addr and port for socket server 
    int PORT = 1111;
    char* ADDR = "127.0.0.1";
    printf("Enter your IP : \n");
    scanf("%s",ADDR);
    printf("Enter your port : \n");
    scanf("%d",&PORT);
    
    //Variables
    FILE *f = NULL;
	int dss,lgAdeClt,i;
    struct sockaddr_in addserv,addClt;
	char* reqt = calloc(1024,sizeof(char));	
	char* reps = calloc(1024,sizeof(char));
    
    //Username and password for the user
    char*nom[50],pass[50];

    //Init socket server
    addserv.sin_family = AF_INET;
    addserv.sin_port = htons(PORT);
    inet_aton(ADDR,&(addserv.sin_addr));
    memset(&(addserv.sin_zero),'0',8);
    dss = socket(AF_INET,SOCK_STREAM,6); 
    FILE *sortie;
    int zz;

    //Bind server
	if(bind(dss,(struct sockaddr*) &addserv,sizeof(struct sockaddr)) != -1){
        //Listen
		if(listen(dss,10) !=-1){
			while(1){

                //Accept new clients
                dsc = accept(dss,(struct sockaddr *) &addClt, (socklen_t *) &lgAdeClt);
                printf("\n== New Client ==\n");

                //Recieve username and password from the client
                recv(dsc, nom, MAX_RECV_BUF, 0);
                recv(dsc, pass, MAX_RECV_BUF, 0);
                node *user=NULL;

                //test if user exist
                user = getUser(nom);
                if(user != NULL){
                    //Test password
                    if(strcmp(pass,(char*)user->next->data) == 0){

                        //Send acccepted to client
                        send(dsc,"accepted",256,0);
                        //MutiClient handling
                        if(fork() ==0){ 
                            while ( 1 )
                            {
                                // Client disconnected
                                if(recv_file(dsc, "queryFile") == -1){ 
                                    close(dsc); 
                                    printf("\n== Client Disconnected == \n");
                                    exit(-1);
                                }else{
                                    // Execute the file recieved.
                                    ExecuteFile("queryFile",dsc);
                                }
                            }
                        }
                    }else{
                        //Send rejected to client and close socket connection with client
                        send(dsc,"rejected",256,0); 
                        close(dsc);
                    }
                }else{
                    send(dsc,"rejected",256,0); 
                    close(dsc);
                }
            }
		}else printf("\nCan't listen");
	}else printf("\nCan't bind to %s:%d\n",ADDR,PORT);
	printf("\nClosing CQL Server\n");
    return 0;
}


//Executing the file sent by the client
void ExecuteFile(char*fn,int sock){
    tokenList = tokenize(fn);
    if(tokenList == NULL) printf("Le fichier est vide ou n'existe pas.\n");
    parse();
    freeTokens(tokenList);
    if( tokenList != NULL) tokenList = NULL;
}

int recv_file(int sock, char* file_name)
{
    int  sizeOfFILE;
    char send_str [MAX_SEND_BUF];
    int f;
    ssize_t sent_bytes, rcvd_bytes, rcvd_file_size;
    int recv_count;
    char recv_str[MAX_RECV_BUF];
    size_t send_strlen;

    recv(sock, recv_str, MAX_RECV_BUF, 0);

    sizeOfFILE = atoi(recv_str);
    if(sizeOfFILE == 0){
        return -1;
    }
    //printf("SIZE IS %d\n",sizeOfFILE);
    sprintf(send_str, "%s\n", file_name); 
    send_strlen = strlen(send_str);
    system("rm -rf queryFile");
    if ( (f = open(file_name,O_WRONLY|O_CREAT, 0644)) < 0 )
    {
        perror("error creating file");
        return -1;
    }

    recv_count = 0;
    rcvd_file_size = 0;

    while ( (rcvd_bytes = recv(sock, recv_str, MAX_RECV_BUF, 0)) >= 0)
    {
        recv_count++;
        rcvd_file_size += rcvd_bytes;

        if (write(f, recv_str, rcvd_bytes) < 0 )
        {
            perror("error writing to file");
            return -1;
        }
        if(rcvd_file_size >= sizeOfFILE){
            break;
        }
    }
    close(f);
    //printf("Client Received: %d bytes in %d recv(s)\n", rcvd_file_size,recv_count);
    return rcvd_file_size;
}