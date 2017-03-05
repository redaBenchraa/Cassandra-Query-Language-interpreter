#ifndef resultHandler
#define resultHandler
typedef struct resultNode{
	char *data;
	struct resultNode *next;
}resultNode;
extern struct resultNode *resultList;
extern int dsc;
void  pushResult(char* new_data,int data_size);
void sendResult();
#endif