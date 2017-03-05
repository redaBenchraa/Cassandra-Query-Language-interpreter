#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "resultHandler.h"

//This files is required for storing and sending results to the client

//Results linked list
resultNode *resultList;

// Socket description
int dsc;

// Add a string to the results list
void pushResult(char* new_data,int data_size){
	resultNode *last = (struct resultNode*)malloc(sizeof(struct resultNode));
    last->data = malloc(data_size);
    last->next = NULL;
    memcpy(last->data, new_data, data_size);
    if (resultList == NULL) {
        resultList = last;
    } else {
        resultNode *temp = resultList;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = last;
    };
}

// Send the result via socket to the client
void sendResult(){
    while(resultList != NULL){
        send(dsc,resultList->data,500,0);
        //printf("Data sending : %s\n",resultList->data);
        resultList = resultList->next;
    }
    resultList = NULL;
}