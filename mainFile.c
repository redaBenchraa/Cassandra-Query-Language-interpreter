#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <jansson.h>
#include <stdarg.h>
#include "parser.h"
#include "interpreter.h"
void getQuery(char *buf)
{
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

int main(int argc, char ** argv){
	char *request = malloc(100);
	while ( 1 )
	{
		
		printf("CQL# ");
		getQuery(request);
		if(strcmp(request,"clear;") == 0 || strcmp(request,"\nclear;") == 0) 
		{
				system("clear");
		}		
		else if(strcmp(request,"exit;") == 0 || strcmp(request,"\nexit;") == 0) 
		{
				exit(0);
		}
		else
		{	
			tokenList = tokenize(request);
			if(tokenList == NULL) printf("Le fichier est vide ou n'existe pas.\n");
			parse();
			freeTokens(tokenList);
			if( tokenList != NULL) tokenList = NULL;
		}
		free(request);
		if( request != NULL) request = NULL;
		request = malloc(100);
	}
}