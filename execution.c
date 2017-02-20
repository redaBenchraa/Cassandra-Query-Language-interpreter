#include "execution.h"
char *rootName = "node";
char *results[150]={
	"OPEN_ERROR",
	"KEYSPACE_EXISTS",
	"KEYSPACE_NOT_EXISTS",
	"KEYSPACE_CREATED",
	"KEYSPACE_CONFIG_CREATED",
	"KEYSPACE_CONFIG_NOT_CREATED",
	"KEYSPACE_CONFIG_NOT_EXSISTS",
	"FOLDER_NOT_EXISTS",
	"FOLDER_EXISTS",
	"ROOT_EXISTS",
	"ROOT_CREATED",
	"TABLE_EXISTS",
	"TABLE_NOT_EXISTS",
	"TABLE_CREATED",
	"TABLE_CONFIG_CREATED",
	"TABLE_CONFIG_NOT_CREATED",
	"TABLE_ERROR_READING_DATA",
	"TABLE_ERROR_READING_CONFIG",
	"TABLE_TRUNCATED",
	"FOLDER_ERROR_CREATE",
	"FOLDER_DELETED",
	"FOLDER_ERROR_DELETE",
	"ROW_EXISTS",
	"ROW_NULL",
	"ROW_UPDATED",
	"ROW_DELETED",
	"ROW_INSERTED",
	"ROW_NOT_EXISTS",
	"COLUMN_VALUE_UPDATED",
	"COLUMN_NOT_FOUND",
	"COLUMN_DELETED",
	"COLUMN_IS_PRIMARY",
	"COLUMN_ALTERED",
	"COLUMN_ALREADY_EXISTS",
	"COLUMN_ADDED",
	"OPS_INF",
	"OPS_SUP",
	"OPS_INFE",
	"OPS_SUPE",
	"OPS_EQ",
	"OPS_DIFF",
	"ORDER_BY_ASC",
	"ORDER_BY_DESC",
	"INDEX_AND_VALUES_NOT_SAME_SIZE",
	"INDEX_NOT_FOUND",
	"INCOMPATIBLE_TYPES",
  	"VALID_ENTERY",
  	"ALL_PRIMARY_HERE",
  	"MISSING_PRIMARY",
  	"MISSING_COLUMN",
  	"ALL_COLUMNS"
};
int getColumnIndex1(node *columns, char*columnName){
	int i=0;
	while(columns != NULL){
		char *cn = (char*)columns->data;
		if(strcmp(cn,columnName) == 0 ) return i;
		i++;
		columns = columns->next;
	}
	return -1;
}
int checkTable(char*keyspaceName,char*tableName){
	char* keySpacePath = concat(rootName,concat("/",keyspaceName));
	char* tablePath = concat(keySpacePath,concat("/",tableName));
	return checkFolder(tablePath);
}
int checkKeyspace(char*keyspaceName,char*tableName){
	char* keySpacePath = concat(rootName,concat("/",keyspaceName));
	return checkFolder(keySpacePath);
}
void pushToList(struct node** head_ref, void *new_data, size_t data_size){
    node *last = (struct node*)malloc(sizeof(struct node));
    last->data = malloc(data_size);
    last->next = NULL;
    memcpy(last->data, new_data, data_size);
    if ((*head_ref) == NULL) {
        (*head_ref) = last;
    } else {
        node *temp = (*head_ref);
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = last;
    };
}
void printList(struct node *node, void (*fptr)(void *)){
    while (node != NULL)
    {
        (*fptr)(node->data);
        node = node->next;
    }
    printf("\n");
}
int getLinkedListSize(node *n){
	node* node =n;
	int i = 0;
    while (node != NULL)
    {
        node = node->next;
        i++;
    }
    return i;
}
void printInt(void *n){
   printf(" %d", *(int *)n);
}
void printString(void *c)
{
   printf("%s\t", (char*)c);
}
void printCell(void *c){
  Cell *cell =c;
  printf("%d -- %s\n",cell->size,cell->value);
}
int sizeofString(char *c){
    int count=0;
    int i=0;
    while(c[i] !='\0'){
      count++;
      i++;
    }
    return count;
}

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1+1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

Cell * initCell(char*val){
	Cell *cell = (Cell*) malloc(sizeof(Cell));
    cell->value = (char*) malloc(sizeofString(val)*sizeof(char));
    strcpy(cell->value,val);
    cell->size = sizeofString(val);
    return cell;
}

/* Function to remove duplicates from a unsorted linked list */
void removeDuplicates(node *start,TableConfig * t)
{
  	node *ptr1, *ptr2, *dup;
  	ptr1 = start;
   	while(ptr1->next != NULL)
  	{
     	ptr2 = ptr1->next;
     	if(compareRowsPK((Row*) ptr1->data,(Row*) ptr2->data,1,t->primaryKeys) == t->pkCount){
     		ptr1->next  = ptr2->next;
     	}
     	if(ptr1->next == NULL) return;
     	node *nCurrt  = ptr2->next;
     	node *nPrev = ptr2;
      	while(nCurrt != NULL)
     	{
	       	if(compareRowsPK((Row*) ptr1->data,(Row*) nCurrt->data,1,t->primaryKeys) == t->pkCount)
	       	{
				nPrev->next = nCurrt->next;
				node*tmp = nCurrt;
				free(tmp);
				nCurrt = nPrev->next;
			}else{
				nPrev = nCurrt;
				nCurrt = nCurrt->next;
	    	}
	    }
    	ptr1 = ptr1->next;
  	}
}
int compareIndex(Row *n1,Row *n2,int index){
	node *cells1 = n1->cells;
	node *cells2 = n2->cells;	
	int i=0;
	while ( cells1 !=NULL )
	{
		if(index == i){
			Cell *c1 = (Cell*)cells1->data;
			Cell *c2 = (Cell*)cells2->data;
			char cg1[1000],cg2[1000];
			strcpy(cg1,c1->value);
			strcpy(cg2,c2->value);
			return strcmp(cg1,cg2);
		}
	 	cells1 = cells1->next;
	  	cells2 = cells2->next;
	  	i++;
	}
	return INDEX_NOT_FOUND;
}
/*  sort the given linked lsit */
void SortRows(node *start,int index,int order)
{
	if(start == NULL) return;
    int swapped, i,comp;
    struct node *ptr1;
    struct node *lptr = NULL;
    if (ptr1 == NULL)
        return;
 
    do
    {
        swapped = 0;
        ptr1 = start;
 
        while (ptr1->next != lptr)
        {
        	comp = compareIndex((Row*)ptr1->data,(Row*)ptr1->next->data,index);
            switch(order){
            	case ORDER_BY_ASC: 
	            	if (comp > 0)
		            { 
		                swap(ptr1, ptr1->next);
		                swapped = 1;
		            }
            	break;
            	case ORDER_BY_DESC: 
	            	if (comp < 0)
		            { 
		                swap(ptr1, ptr1->next);
		                swapped = 1;
		            }
            	break;
            	default: 
	            	if (comp > 0)
		            { 
		                swap(ptr1, ptr1->next);
		                swapped = 1;
		            }
            	break;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    }
    while (swapped);
}
 void swap(struct node *a, struct node *b)
{
    int temp = a->data;
    a->data = b->data;
    b->data = temp;
}
/* Function to concat two linked lists */
void concatRows(node *n1,node *n2,TableConfig * t){
	if(n1 == NULL) {
		n1 = n2;
		return;
	}
	if(n2 == NULL) return;
	node *ptr1 = n1;
	while(ptr1->next != NULL) ptr1 = ptr1->next;
	ptr1->next = n2;
	removeDuplicates(n1,t);
}

void limitData(node *n1,int limit){
	int i=1;
	if(n1 == NULL) return;
	node *ptr1 = n1;
  	while ( ptr1 != NULL && i++<limit) ptr1 = ptr1->next;
  	if ( i > limit )
  	{
  		ptr1->next = NULL;
  		node *freed = ptr1->next;
  		while (freed != NULL)
	    { 
	        node* temp = freed; 
	        free(temp);
	        temp = NULL;
	        freed = freed -> next;
	    }
  	}
}

/* Check if folder exists
return
FOLDER_EXISTS
FOLDER_NOT_EXISTS
*/
int checkFolder(char *name){
    struct stat sb;
	if (stat(name, &sb) == 0 && S_ISDIR(sb.st_mode))
    {
       	return FOLDER_EXISTS;
    }
    else
    {
        return FOLDER_NOT_EXISTS;
    }
}

/*Create root folder 
FOLDER_ERROR_CREATE
ROOT_CREATED
*/
int createRoot(){
	if(checkFolder(rootName) == FOLDER_EXISTS){
		return ROOT_EXISTS;
	}else{
		mkdir(rootName,0777);
		if(checkFolder(rootName) == FOLDER_NOT_EXISTS){
			return FOLDER_ERROR_CREATE;
		}else{
			return ROOT_CREATED;
		}

	}
}

/*
TABLE_CONFIG_CREATED
TABLE_CONFIG_NOT_CREATED
*/
int createTableConfig(char* keyspaceName,char* tableName,TableConfig *config){
  json_t *root = json_object();
  json_t *json_arr_names = json_array();
  json_t *json_arr_types = json_array();
  json_t *json_arr_pk = json_array();
  
  json_object_set_new( root, "columnCount", json_integer(config->columnCount) );
  json_object_set_new( root, "columnNames", json_arr_names);
  json_object_set_new( root, "columnTypes", json_arr_types);
  json_object_set_new( root, "pkCount", json_integer(config->pkCount) );
  json_object_set_new( root, "primaryKeys", json_arr_pk);
  
  while (config->columnNames != NULL)
    {
      json_array_append_new( json_arr_names, json_string(config->columnNames->data));
      config->columnNames = config->columnNames->next;
    }
  while (config->columnTypes != NULL)
    {
      json_array_append_new( json_arr_types, json_string(config->columnTypes->data));
      config->columnTypes = config->columnTypes->next;
    }
  while (config->primaryKeys != NULL)
    {
      json_array_append_new( json_arr_pk, json_integer(*(int *)config->primaryKeys->data));
      config->primaryKeys = config->primaryKeys->next;
    }
  char *fileName = concat(concat(concat(concat(concat(concat(rootName,"/"),keyspaceName),"/"),tableName),"/"),"config");
  int i = json_dump_file(root,fileName, 0);
  json_decref(root);
  if(i == 0){
  	return TABLE_CONFIG_CREATED;
  }else{
    return TABLE_CONFIG_NOT_CREATED;
  }

}

/*Create keyspace folder
FOLDER_ERROR_CREATE
KEYSPACE_CREATED
KEYSPACE_EXISTS
*/
int createKeyspace(char *name,keyspaceConfig *config){
	if(createRoot() == FOLDER_ERROR_CREATE){
		return FOLDER_ERROR_CREATE;
	}
	char *keyspaceName = concat(concat(rootName,"/"),name);
	if(checkFolder(keyspaceName) == FOLDER_NOT_EXISTS){
		mkdir(keyspaceName,0777);
		if(checkFolder(keyspaceName) == FOLDER_NOT_EXISTS){
			return FOLDER_ERROR_CREATE;
		}else{
			return KEYSPACE_CREATED;
		}
	}else{
		return KEYSPACE_EXISTS;
	}
}

/*Create table folder 
returns 
TABLE_CREATED 
FOLDER_ERROR_CREATE
KEYSPACE_NOT_EXISTS
TABLE_EXISTS
*/
int createTable(char *keyspaceName, char *tableName, TableConfig *config){
	char* keySpacePath = concat(rootName,concat("/",keyspaceName));
	char* tablePath = concat(keySpacePath,concat("/",tableName));
	char* dataPath = concat(tablePath,"/data");
	if(checkFolder(keySpacePath) != FOLDER_EXISTS) return KEYSPACE_NOT_EXISTS;
	if(checkFolder(tablePath) == FOLDER_EXISTS) return TABLE_EXISTS;
	mkdir(tablePath,0777);
	if(checkFolder(tablePath) == FOLDER_EXISTS){
		createTableConfig(keyspaceName,tableName,config);
		FILE *data = fopen(dataPath,"w");
		fprintf(data, "{}");
		fclose(data);
		keyspaceConfig *ksConfig = readKeyspaceConfig(keyspaceName);
		if(ksConfig == NULL ) return KEYSPACE_CONFIG_NOT_EXSISTS;
		ksConfig->tableCount++;
		pushToList(&ksConfig->tableNames,tableName,sizeofString(tableName)+1);
		saveKeySpaceConfig(keyspaceName,ksConfig);
		return TABLE_CREATED;	
	}else return FOLDER_ERROR_CREATE;
}

/*Delete keyspace folder with all tables
returns
FOLDER_DELETED
FOLDER_ERROR_DELETE
CHANGE
*/
int deleteKeyspace(char *name){

	char* folderPath = concat(concat(rootName,DELIMITER),name);
	char* deleteCmd = concat(DELETECMD,folderPath);
	system(deleteCmd);
	if(checkFolder(folderPath) == FOLDER_NOT_EXISTS){
		return FOLDER_DELETED;
	}else{
		return FOLDER_ERROR_DELETE;
	}
	//delete it also form root config
}

/*
Delete table folder  
returns 
FOLDER_NOT_EXISTS 
FOLDER_DELETED 
FOLDER_ERROR_DELETE
CHANGE
*/
int deleteTable(char *keyspaceName,char *name){
	if(checkFolder(rootName) == FOLDER_NOT_EXISTS ) return FOLDER_NOT_EXISTS;
	char* folderPath = concat(concat(concat(concat(rootName,DELIMITER),keyspaceName),DELIMITER),name);
	char* deleteCmd = concat(DELETECMD,folderPath);
	system(deleteCmd);
	if(checkFolder(folderPath) == FOLDER_NOT_EXISTS){
		keyspaceConfig *config = readKeyspaceConfig(keyspaceName);
		config->tableCount--;
		if(strcmp((char*)config->tableNames->data,name)==0){
			config->tableNames = config->tableNames->next;
		}
		else{
			node * previous= config->tableNames;
			node * current = config->tableNames->next;
			while(current != NULL){

				if(strcmp((char*)current->data,name)==0){
					node * temp = current;
					previous->next = current->next;
					free(temp);
					current = previous->next;
				}
				else{
					previous = previous->next;
					current = current->next;
				}
			}
			saveKeySpaceConfig(keyspaceName,config); 
		}
		return FOLDER_DELETED;
	}else{
		return FOLDER_ERROR_DELETE;
	}
}

/*Delete root folder
returns 
FOLDER_DELETED
FOLDER_ERROR_DELETE
*/
int deleteAll(){
	char* deleteCmd = concat(DELETECMD,rootName);
	system(deleteCmd);
	if(checkFolder(rootName) == FOLDER_NOT_EXISTS){
		return FOLDER_DELETED;
	}else{
		return FOLDER_ERROR_DELETE;
	}
}

int checkInPK(int index,node *primaryKeys){
	node *node = primaryKeys;
    while (node != NULL)
    {
        int i= *(int *)node->data;
        if(index == i){
			return 1;
        } 
        node = node->next;
    }
    return 0;
}
int compareRowsPK(Row* r1,Row* r2,int nbr,node *primaryKeys){
	if(nbr > 0){
		int i=0;
		int rep=0;
		node *cell1 = r1->cells;
		node *cell2 = r2->cells;
		while (cell1 != NULL && cell2 != NULL)
		{
			Cell * c1 = (Cell*) cell1->data;
			Cell * c2 = (Cell*) cell2->data;
			if(checkInPK(i,primaryKeys)){
				if(strcmp(c1->value,c2->value) == 0 ) {
					rep++;
				}
			}
			cell1 = cell1->next;
			cell2 = cell2->next;
			i++;
		}
		return rep;
	}else return 0;
}
/*Insert row into a table 
retuen
ROW_EXISTS
ROW_INSERTED
*/
int insertRow(char* keyspaceName,char *tableName,Row *rowData,char* keyValue){
    char *fileName = concat(concat(concat(concat(concat(concat(rootName,"/"),keyspaceName),"/"),tableName),"/"),"data");
	json_error_t error;
	json_t *row = json_object();
	json_t *root;
	FILE * f = fopen(fileName,"r");
	if(f== NULL){
		root = json_object();
	}else{
		root = json_load_file(fileName,0,&error);
		if(!root){
			root = json_object();
		}
	}
	fclose(f);
	
	TableConfig * t = readTableConfig(keyspaceName,tableName);
	tableData * dataTable = readTableData(keyspaceName,tableName);

	if(dataTable->rowCount > 0 ){
	    while (dataTable->rowData != NULL)
	    {
	    	Row *r = (Row*) dataTable->rowData->data;
		   	if(compareRowsPK(r,rowData,1,t->primaryKeys) ==  t->pkCount) {
		   		return ROW_EXISTS;
		   	}
	      	dataTable->rowData= dataTable->rowData->next;
	    }
	}
	dataTable->rowCount = dataTable->rowCount+1;
 	json_t *json_arr_data = json_array();
 	char * key = malloc(30 * sizeof(char));
 	if(keyValue == NULL) {
 		sprintf(key, "%u", (unsigned)time(NULL)); 
 	}else strcpy(key,keyValue);
 	json_object_set_new( root, key , row);
 	json_object_set_new( row, "state", json_integer(rowData->state));
 	json_object_set_new( row, "data", json_arr_data);
 	node * cells = rowData->cells;
	while (cells != NULL)
    {
    	Cell * c = (Cell*) cells->data;
      	json_array_append_new( json_arr_data, json_string(c->value));
      	cells = cells->next;
    }
  	json_dump_file(root,fileName, 0);

  	return ROW_INSERTED;
}

int deleteListRows(char* keyspaceName,char *tableName,node *keys){
    while (keys != NULL)
    {
	    char *key = (char*)keys->data;
	    deleteRow(keyspaceName,tableName,key);
        keys = keys->next;
    }
}
/*
returns 
TABLE_NOT_EXISTS
OPEN_ERROR
ROW_DELETED
*/
int deleteRow(char* keyspaceName,char *tableName,char*key){
	char * Path = concat(concat(concat(concat(rootName,DELIMITER),keyspaceName),DELIMITER),tableName);
	if(checkFolder(Path) == FOLDER_NOT_EXISTS){
		return TABLE_NOT_EXISTS;
	}
	json_t *root,*value,*data,*state;
	json_error_t error;
	const char *k;
	char *fileName = concat(concat(concat(concat(concat(concat(rootName,"/"),keyspaceName),"/"),tableName),"/"),"data");
	root = json_load_file(fileName,0,&error);
	if(!root){
	    return OPEN_ERROR;
	}
	json_object_del(root,key);
	json_dump_file(root,fileName, 0);
  	json_decref(root);	
	return ROW_DELETED;
}

Row* getRow(char* keyspaceName,char *tableName,char *keyValue){
	char * Path = concat(concat(concat(concat(rootName,DELIMITER),keyspaceName),DELIMITER),tableName);
	if(checkFolder(Path) == FOLDER_NOT_EXISTS){
		return NULL;
	}
	json_t *root,*value,*data,*state;
	const char *key;
	json_error_t error;
	size_t index;
	char *fileName = concat(concat(concat(concat(concat(concat(rootName,"/"),keyspaceName),"/"),tableName),"/"),"data");
	root = json_load_file(fileName,0,&error);
	if(!root){
	    return NULL;
	}
	Row* row;
	row = NULL;
	bool found = false;
	json_object_foreach(root, key, value) {
		if(strcmp(key,keyValue) == 0){
			found = true;
			data = json_object_get(value, "data");
			state = json_object_get(value, "state");
			if(!json_is_integer(state) || !json_is_array(data) )
	        {
				return NULL;
	        }
			row = (Row*)malloc(sizeof(Row));
			row->state = json_integer_value(state);
			if(row->state  == 0 ){
				return NULL;
			}
			row->cells = NULL;
	        for(index = 0; index < json_array_size(data) && (value = json_array_get(data, index)); index++){
	       		pushToList(&row->cells,  initCell(json_string_value(value)), sizeof(Cell));
			}
		}
	}
	if(found == false) return NULL;
	return row;
}

//CHANGE
int getColumnIndex(char* keyspaceName,char *tableName,char*columnName){
	TableConfig * t = readTableConfig(keyspaceName,tableName);
	int i=0;
	if(t != NULL){
		node*d = t->columnNames;
		while(d!= NULL){
			char *cn = (char*)d->data;
			if(strcmp(cn,columnName) == 0 ) return i;
			i++;
			d = d->next;
		}
		return -1;
	}else return -1;
}

/*
returns
COLUMN_NOT_FOUND
ROW_NULL
COLUMN_VALUE_UPDATED
*/
int updateRowValue(Row* row,int index, char*newValue){
	int i=0;
	if(row == NULL) return ROW_NULL;
		Row *r = row;
		node *cell = r->cells;
	    while (cell != NULL)
	    {
	    	if(i==index){
				Cell * c = (Cell*) cell->data;
				strcpy(c->value,newValue);
	    		return COLUMN_VALUE_UPDATED;
	    	}
	    	i++;
	      	cell = cell->next;
	    }
	    return COLUMN_NOT_FOUND;	
}
/*
returns 
ROW_NOT_EXISTS
ROW_UPDATED
*/
int updateRowInJSON(char* keyspaceName,char *tableName,Row* rowData){
	if(rowData == NULL) return ROW_NULL;
	if(getRow(keyspaceName,tableName,rowData->key) == NULL){
		return ROW_NOT_EXISTS;
	}
	char *fileName = concat(concat(concat(concat(concat(concat(rootName,"/"),keyspaceName),"/"),tableName),"/"),"data");
	json_error_t error;
	json_t *row = json_object();
	json_t *root;
	FILE * f = fopen(fileName,"r");
	if(f== NULL){
		root = json_object();
	}else{
		root = json_load_file(fileName,0,&error);
		if(!root){
			root = json_object();
		}
	}
	fclose(f);
 	json_t *json_arr_data = json_array();

 	json_object_set_new( root, (char*)rowData->key , row);
 	json_object_set_new( row, "state", json_integer(rowData->state));
 	json_object_set_new( row, "data", json_arr_data);
 	node *cells = rowData->cells;
	while (cells != NULL)
    {
    	Cell * c = (Cell*) cells->data;
      	json_array_append_new( json_arr_data, json_string(c->value));
      	cells = cells->next;
    }
  	json_dump_file(root,fileName, 0);
  	json_decref(root);
  	return ROW_UPDATED;
}
/*
returns 
ROW_NOT_EXISTS
INDEX_AND_VALUES_NOT_SAME_SIZE
ROW_UPDATED
*/
int updateRow(char* keyspaceName,char *tableName,Row* row,node *listIndexes,node *listValues){
	if(getLinkedListSize(listIndexes) != getLinkedListSize(listValues)) return INDEX_AND_VALUES_NOT_SAME_SIZE;
	while(listIndexes != NULL){
		updateRowValue(row,*(int*)listIndexes->data,(char*)listValues->data);
		listIndexes = listIndexes->next;
		listValues = listValues->next;
	}
	return updateRowInJSON(keyspaceName,tableName,row);
}
void updateRows(char* keyspaceName,char *tableName,tableData *data,node *listIndexes,node *listValues){
	node *n = data->rowData;
	while(n != NULL){
		updateRow(keyspaceName,tableName,(Row*)n->data,listIndexes,listValues);
		n = n->next;
	}
}

tableData* readTableData(char* keyspaceName,char* tableName){
	char * Path = concat(concat(concat(concat(rootName,DELIMITER),keyspaceName),DELIMITER),tableName);
	if(checkFolder(Path) == FOLDER_NOT_EXISTS){
		return NULL;
	}
	json_t *root,*value,*data,*state;
	const char *key;
	json_error_t error;
	size_t index;
	tableData *dataTable = malloc(sizeof(tableData));
	dataTable->rowData = NULL;
	char *fileName = concat(concat(concat(concat(concat(concat(rootName,"/"),keyspaceName),"/"),tableName),"/"),"data");
	root = json_load_file(fileName,0,&error);
	if(!root){
	    return NULL;
	}
	int*count = (int*) malloc(sizeof(int));
	*count = 0;
	Row* row;
	json_object_foreach(root, key, value) {
		data = json_object_get(value, "data");
		state = json_object_get(value, "state");
		if(!json_is_integer(state) || !json_is_array(data) )
        {
            return NULL;
        }
		row = (Row*)malloc(sizeof(Row));
		row->cells = NULL;
		row->key = malloc(10*sizeof(char));
		row->state = json_integer_value(state);
		strcpy(row->key,key);
        for(index = 0; index < json_array_size(data) && (value = json_array_get(data, index)); index++){
       		pushToList(&row->cells,  initCell(json_string_value(value)), sizeof(Cell));
		}
		if(json_integer_value(state) == 1 ){
			pushToList(&dataTable->rowData, row , sizeof(Row));
			*count = *count +1;
		}
	}
	memcpy(&dataTable->rowCount,count,sizeof(int));
	return dataTable;
}
//Get a table's configuration information
TableConfig* readTableConfig(char *keyspaceName,char *tableName){
	char * Path = concat(concat(concat(concat(rootName,DELIMITER),keyspaceName),DELIMITER),tableName);
	if(checkFolder(Path) == FOLDER_NOT_EXISTS){
		return NULL;
	}else{
	    json_t *root, *columnCount,*columnNames,*columnTypes,*pkCount,*primaryKeys;
		json_error_t error;
		size_t index;
		json_t *value;
		TableConfig * config =  malloc(sizeof(TableConfig));
		char *fileName = concat(concat(concat(concat(concat(concat(rootName,"/"),keyspaceName),"/"),tableName),"/"),"config");
		root = json_load_file(fileName,0,&error);
		if(!root)
	    {
	        fprintf(stderr, "error : JSON : on line %d: %s\n", error.line, error.text);
	        return NULL;
	    }
		columnCount = json_object_get(root, "columnCount");
		columnNames = json_object_get(root, "columnNames");
		columnTypes = json_object_get(root, "columnTypes");
		pkCount = json_object_get(root, "pkCount");
		primaryKeys = json_object_get(root, "primaryKeys");
		config->columnNames = NULL;
		config->columnTypes = NULL;
		config->primaryKeys = NULL;
		if(!json_is_integer(columnCount) 
			|| !json_is_integer(pkCount) 
			|| !json_is_array(columnNames) 
			|| !json_is_array(columnTypes) 
			|| !json_is_array(primaryKeys))
        {
            fprintf(stderr, "error: JSON %d: table json file is not a corrupted\n");
            return NULL;
        }
        config->columnCount = json_integer_value(columnCount);
        config->pkCount = json_integer_value(pkCount);

        for(index = 0; index < json_array_size(columnNames) && (value = json_array_get(columnNames, index)); index++){
       		pushToList(&config->columnNames, json_string_value(value), sizeofString(json_string_value(value))+1);
		}
      	for(index = 0; index < json_array_size(columnTypes) && (value = json_array_get(columnTypes, index)); index++){
       		pushToList(&config->columnTypes, json_string_value(value), sizeofString(json_string_value(value))+1);
		}
        for(index = 0; index < json_array_size(primaryKeys) && (value = json_array_get(primaryKeys, index)); index++){
        	int val = json_integer_value(value);
       		pushToList(&config->primaryKeys, &val, sizeof(val));
		}

		return config;
	}
}

/*
KEYSPACE_CONFIG_CREATED
KEYSPACE_CONFIG_NOT_CREATED
*/
int saveKeySpaceConfig(char * keyspaceName,keyspaceConfig *kc){
	json_t *config = json_object();
	json_t *tables = json_array(); 
	char * stringJSON;

	json_object_set_new(config,"tableCount", json_integer(kc->tableCount));
	 while (kc->tableNames != NULL)
    {
        json_array_append_new( tables, json_string(kc->tableNames->data));
        kc->tableNames = kc->tableNames->next;
    }

	json_object_set_new(config,"tableNames", tables);
	char *fileName = concat(concat(concat(concat(rootName,"/"),keyspaceName),"/"),"config");
	int i = json_dump_file(config,fileName,0);
	if(i == 0 ){
		return KEYSPACE_CONFIG_CREATED;
	}else{
		return KEYSPACE_CONFIG_NOT_CREATED;
	}
}


keyspaceConfig * readKeyspaceConfig(char * keyspaceName){

	json_t * config;
	json_error_t error;
	char *path = concat(concat(rootName,"/"),keyspaceName);
	char *fileName = concat(concat(path,"/"),"config");
	
	if(checkFolder(path) == FOLDER_NOT_EXISTS){
		printf("KEYSPACE_NOT_EXISTS\n");
		return NULL;
	}
	else{
		json_t *root, *tableCount,*tableNames;
		json_error_t error;
		size_t index;
		json_t *value;
		keyspaceConfig * config =  malloc(sizeof(keyspaceConfig));
		root = json_load_file(fileName,0,&error);
		if(!root)
	    {
	        fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
	        return NULL;
	    }
		tableNames = json_object_get(root, "tableNames");
		tableCount = json_object_get(root, "tableCount");
		config->tableNames = NULL;
		if(		!json_is_integer(tableCount) 
			||	!json_is_array(tableNames) )
        {
            fprintf(stderr, "error: JSON %d: table json file is not a corrupted\n");
            return NULL;
        }
        config->tableCount = json_integer_value(tableCount);
        for(index = 0; index < json_array_size(tableNames) && (value = json_array_get(tableNames, index)); index++){
       		pushToList(&config->tableNames, json_string_value(value), sizeofString(json_string_value(value))+1);
		}
		return config;
	}		
}


bool isPrimary(char *columnName,TableConfig *t){
	
	int i = 0;
	node *names = t->columnNames;
	node *keys = t->primaryKeys;
	if(names != NULL){
		while(names != NULL && strcmp((char *)(names->data),columnName) != 0){
			i++;
			names = names->next;
		}
		if( names == NULL ){
			return false;
		}
		else{
			while(keys != NULL && *(int *)(keys->data) != i){
				keys = keys->next;
			}
			if(keys == NULL){
				return false;
			}
			else{
				return true;
			}
		}
		
	}

}
int compareKeyWithList(char*keyspaceName,char*tableName,char*keyVal,node*listKeys){
	node *keys = listKeys;
    while (keys != NULL)
    {
	    char *key = (char*)keys->data;
	    if(strcmp(key,keyVal) == 0) return 0;
        keys = keys->next;
    }
    return 1;
}
int compareKeyWithList2(int keyVal,node*listColumns){
	node *keys = listColumns;
    while (keys != NULL)
    {
	    int key = *(int *)keys->data;
	    if(key == keyVal){
	    	return 0;	
	    } 
        keys = keys->next;
    }
    return 1;
}

char* getColumnName(char* keyspaceName,char *tableName,int index){
	TableConfig * t = readTableConfig(keyspaceName,tableName);
	int i=0;
	node *d =t->columnNames;
	while(d != NULL){
		char *cn = (char*)d->data;
		if(i == index ) return cn;
		i++;
		d = d->next;
	}
	return NULL;

}
printSelectHeader(char*keyspaceName,char*tableName,node*columns,bool flag){
	node *keys = columns;
    while (keys != NULL)
    {
    	if(flag == true){
	    int key = *(int *)keys->data;
	    printf("%s\t",getColumnName(keyspaceName,tableName,key));
    	}else{
	    char* key = (char*)keys->data;
	    printf("%s\t",key);
    	}
        keys = keys->next;
    }
    printf("\n------------------------------------------\n");
}
int showSelectResult(char*keyspaceName,char*tableName,node*listColumn,node*asNames,tableData *dataTable){
	TableConfig *t = readTableConfig(keyspaceName,tableName);
    printf("\n------------------------------------------\n");
	if(listColumn == NULL) printSelectHeader(keyspaceName,tableName,t->columnNames,false);
	else printSelectHeader(keyspaceName,tableName,asNames,false);
	node *n = dataTable->rowData;
	while (n!= NULL){
		Row *r = (Row*) n->data;
		printRow(r,listColumn);
		n= n->next;
	}
}

int filterNumbers(tableData*data,TableConfig*t,int ops,int value,int column){
	if(data == NULL || t == NULL || value == NULL || data->rowData == NULL) return;
	Row *r = (Row*) data->rowData->data;
	node *c = r->cells;
	int test = 0;
	bool first = false;
	int i=0;
	while (c != NULL)
    {
    	Cell * cell = c->data;
    	if(column == i){
    		int dataValue = atoi((char*)cell->value);
    		switch(ops){
	    		case OPS_INF : 
	    		if(!(dataValue < value)){  
	    			test = 1;
	    		};
	    		 break;
				case OPS_INFE : 
				if(!(dataValue <= value)){  
					test = 1;
				};
				 break;
				case OPS_SUP : 
				if(!(dataValue > value)){ 
					test = 1;
				};
				  break;
				case OPS_SUPE : 
				if(!(dataValue >= value)){ 
					test = 1;
				};
				  break;
				case OPS_EQ : 
				if(!(dataValue == value)){  
					test = 1;
				};
				 break;
				case OPS_DIFF : 
				if(!(dataValue != value)){  
					test = 1;
				};
				 break; 
				default : test = 0;break;
			}
		}
    	c = c->next;
    	i++;
    }
    if(test == 1) {
    	first = true;
    }
	node *rowCurrent = data->rowData->next;
	node *rowPrev = data->rowData;
    test = 0;
	while(rowCurrent != NULL){
		Row *r = (Row*) rowCurrent->data;
		node *c = r->cells;
		i=0;
		while (c != NULL)
	    {
    		Cell * cell = c->data;
	    	if(column == i){
	    		int dataValue = atoi((char*)cell->value);
	    		switch(ops){
		    		case OPS_INF : 
		    		if(!(dataValue < value)){  
		    			test = 1;
		    		};
		    		 break;
					case OPS_INFE : 
					if(!(dataValue <= value)){  
						test = 1;
					};
					 break;
					case OPS_SUP : 
					if(!(dataValue > value)){ 
						test = 1;
					};
					  break;
					case OPS_SUPE : 
					if(!(dataValue >= value)){ 
						test = 1;
					};
					  break;
					case OPS_EQ : 
					if(!(dataValue == value)){  
						test = 1;
					};
					 break;
					case OPS_DIFF : 
					if(!(dataValue != value)){  
						test = 1;
					};
					 break; 
					default : test = 0;break;
				}
			}
	    	c = c->next;
	    	i++;
	    }
		if(test == 1){
			rowPrev->next = rowCurrent->next;
			node*tmp = rowCurrent;
			free(tmp);
			rowCurrent = rowPrev->next;
		}else{
			rowPrev = rowCurrent;
			rowCurrent = rowCurrent->next;
		}
		test = 0;
	}
	if(first == true){
		data->rowData = data->rowData->next;
	}

}
int filterString(tableData*data,TableConfig*t,char*value,int column){
	if(data == NULL || t == NULL || value == NULL || data->rowData == NULL) return;
	Row *r = (Row*) data->rowData->data;
	node *c = r->cells;
	int test = 0;
	bool first = false;
	int i=0;
	while (c != NULL)
    {
    	Cell * cell = c->data;
    	if(column == i){
    		char c1[50],c2[50];
    		strcpy(c1,(char*)cell->value);
    		strcpy(c2,value);
    		if(strcmp(c1, c2)!=0) 
			{
				test = 1;
			}
    	}
    	c = c->next;
    	i++;
    }
    if(test == 1) {
    	first = true;
    }
	node *rowCurrent = data->rowData->next;
	node *rowPrev = data->rowData;
    test = 0;
	while(rowCurrent != NULL){
		Row *r = (Row*) rowCurrent->data;
		node *c = r->cells;
		i=0;
		while (c != NULL)
	    {
    		Cell * cell = c->data;
	    	if(column == i) {
				char c1[50],c2[50];
    			strcpy(c1,(char*)cell->value);
    			strcpy(c2,value);
    			if(strcmp(c1, c2) != 0) {
    				test = 1;
    			}
	    	}
	    	c = c->next;
	    	i++;
	    }
		if(test == 1){
			rowPrev->next = rowCurrent->next;
			node*tmp = rowCurrent;
			free(tmp);
			rowCurrent = rowPrev->next;
		}else{
			rowPrev = rowCurrent;
			rowCurrent = rowCurrent->next;
		}
		test = 0;
	}
	if(first == true){
		data->rowData = data->rowData->next;
	}

}
/* retruns
COLUMN_IS_PRIMARY 
TABLE_ERROR_READING_CONFIG 
COLUMN_NOT_FOUND 
COLUMN_DELETED 
TABLE_ERROR_READING_DATA
deleteTable() errors
createTable() errors
*/
int alterTableDropColumn(char * keyspaceName,char * tableName,char * columnName){
	TableConfig * t = readTableConfig(keyspaceName,tableName);
	if(t == NULL)  return TABLE_ERROR_READING_CONFIG;
	if(isPrimary(columnName,t)){
		return COLUMN_IS_PRIMARY;
	}
	bool found = false;
	if(strcmp((char *)t->columnNames->data,columnName)==0){
		found = true;
		t->columnNames = t->columnNames->next;
		t->columnTypes = t->columnTypes->next;
	}
	node *namesCurrent = t->columnNames->next;
	node *typesCurrent = t->columnTypes->next;
	node *namesPrev = t->columnNames;
	node *typesPrev = t->columnTypes;
	while(namesCurrent != NULL){
		if(strcmp((char *)namesCurrent->data,columnName)==0){
			found = true;
			namesPrev->next = namesCurrent->next;
			typesPrev->next = typesCurrent->next;
			t->columnCount--;
		}
		namesCurrent = namesCurrent->next;
		typesCurrent = typesCurrent->next;
		namesPrev = namesPrev->next;
		typesPrev = typesPrev->next;
	}
    if (found == false ) return  COLUMN_NOT_FOUND;

	tableData * dataTable = readTableData(keyspaceName,tableName);
	if(dataTable == NULL) return TABLE_ERROR_READING_DATA; 
	int index = getColumnIndex(keyspaceName,tableName,columnName);
	int i = deleteTable(keyspaceName,tableName);
	if(i != FOLDER_DELETED ) return i;
	i = createTable(keyspaceName,tableName,t);
	if( i != TABLE_CREATED)  return i;
	while (dataTable->rowData != NULL)
    {
		Row *r = (Row*) dataTable->rowData->data;
		r->state =1;
		node *c = r->cells;
		if(index == 0){
			r->cells = r->cells->next;
		}
		node *cellCurrent = c->next;
		node *cellPrev = c;
		int i = 1;
	    while (cellCurrent != NULL)
	    {
	    	if(i == index) {
	    		cellPrev->next = cellCurrent->next;
	    	}
	    	cellCurrent = cellCurrent->next;
	    	cellPrev = cellPrev->next;
	    	i++;
	    }
		insertRow(keyspaceName,tableName,r,r->key);
      	dataTable->rowData= dataTable->rowData->next;
    }
    return COLUMN_DELETED;
}

/*
createTableConfig() error
COLUMN_ALTERED
COLUMN_NOT_FOUND
*/
int alterTableRenameColumn(char * keyspaceName,char * tableName,char * columnName,char *newColumnName){

	TableConfig * t = readTableConfig(keyspaceName,tableName);
	TableConfig * new = malloc(sizeof(TableConfig));
	new->columnCount = t->columnCount;
	new->pkCount = t->pkCount;
	new->primaryKeys = t->primaryKeys;
	new->columnNames = NULL;
	new->columnTypes = t->columnTypes;
	bool found;
	while( t->columnNames != NULL){
		if(strcmp((char *)t->columnNames->data,columnName)!=0){	
			pushToList(&new->columnNames, t->columnNames->data, sizeofString((char *)t->columnNames->data)+1);
			found = true;
		}
		else{
			pushToList(&new->columnNames, newColumnName, sizeofString(newColumnName)+1);
		}
		t->columnNames = t->columnNames->next;
	}
	if(found){
		int i = createTableConfig(keyspaceName,tableName,new);
		if(i != TABLE_CONFIG_CREATED) return i;
		return COLUMN_ALTERED;
	}
	else{
		return COLUMN_NOT_FOUND;
	}
}

/*
COLUMN_ALREADY_EXISTS
COLUMN_ADDED
deleteTable() error
createTable() error
*/
int alterTableAddColumn(char * keyspaceName,char * tableName,char * columnName,char *columnType){

	TableConfig * t = readTableConfig(keyspaceName,tableName);
	node * head = t->columnNames;
	bool found = false;
	while( head != NULL ){

		if(strcmp((char *)(head->data),columnName)==0){
			found = true;
		}
		head  = head->next;

	}
	if(found){
		return COLUMN_ALREADY_EXISTS;
	}
	else{
		pushToList(&t->columnNames,columnName , sizeofString(columnName)+1);
		pushToList(&t->columnTypes,columnType , sizeofString(columnType)+1);
		t->columnCount ++;
		tableData * dataTable = readTableData(keyspaceName,tableName);
		int i = deleteTable(keyspaceName,tableName);
		if(i != FOLDER_DELETED ) return i;
		i = createTable(keyspaceName,tableName,t);
		if( i != TABLE_CREATED)  return i;
		while (dataTable->rowData != NULL)
		    {
		    	Row *r = (Row*) dataTable->rowData->data;
		    	Row *row =(Row *) malloc(sizeof(Row));
		    	row->state = 1;
		    	row->cells = NULL;
			    while (r->cells != NULL)
			    {
		    		Cell * c = (Cell*) r->cells->data;
		    		pushToList(&row->cells, initCell(c->value),sizeof(Cell));
		      		r->cells = r->cells->next;
			    }
			    pushToList(&row->cells, initCell("null") ,sizeof(Cell));
				insertRow(keyspaceName,tableName,row,r->key);		      	
		      	dataTable->rowData= dataTable->rowData->next;
		    }
		return COLUMN_ADDED;
	}
}
/*
	return
	deleteTable() errors
	createTavle() errors
	TABLE_TRUNCATED
*/
int truncateTable(char* keyspaceName,char* tableName){
	TableConfig * t = readTableConfig(keyspaceName,tableName);
	int i = deleteTable(keyspaceName,tableName);
	if(i != FOLDER_DELETED ) return i;
	i = createTable(keyspaceName,tableName,t);
	if( i != TABLE_CREATED)  return i;
	return TABLE_TRUNCATED;
}

void printRow(Row* row, node *listColumn){
	Row *r = row;
	node *cell = r->cells;
	int i=0;
	if(listColumn == NULL){
		while (cell != NULL)
	    {
			Cell * c = (Cell*) cell->data;
	    	printf("%s\t",c->value);
	      	cell = cell->next;
	      	i++;
	    }
	    printf("\n------------------------------------------\n");
	    return;
	}
	node *cols = listColumn;
	while(cols != NULL){
		r = row;
		cell = r->cells;
		i=0;
	    while (cell != NULL)
	    {
			Cell * c = (Cell*) cell->data;
			if(i == *(int*) cols->data){
	    		printf("%s\t",c->value);
	    		break;
			}
	      	cell = cell->next;
	      	i++;
	    }
		cols = cols->next;
	}

    printf("\n------------------------------------------\n");
}

node* getKeysFromData(tableData*data){
	node*n=data->rowData;
	node *list = NULL;
	while(n!=NULL){
   		Row *r = n->data;
   		pushToList(&list, r->key, sizeofString(r->key)+1);
		n = n->next;
	}
	return list;
}
char *getColumnType(TableConfig *tc,int index){
	node *t = tc->columnTypes;
	int i=0;
	while ( t!=NULL )
	{
		if(index == i++ )
			return (char*)t->data;
	  	t = t->next;
	}
	return NULL;

}

/*
	TEST FUNCTIONS
*/


void testDeleteRows(char* keyspaceName,char *tableName){
	node *list = NULL;
    pushToList(&list, "14861732152", sizeofString("14861732152"));
    pushToList(&list, "14861732153", sizeofString("14861732153"));
    pushToList(&list, "14861732154", sizeofString("14861732154"));
    deleteListRows(keyspaceName,tableName,list);

}
void testreadTableConfig(char* keyspaceName,char* tableName){
	TableConfig * t = readTableConfig(keyspaceName,tableName);
	printf("%d\n",t->columnCount );
	printf("%d\n",t->pkCount );
	printList(t->columnNames,printString);
	printList(t->columnTypes,printString);
	printList(t->primaryKeys,printInt);
}
void testAddTable(char*keyspaceName,char*tableName){
	TableConfig config;
	config.columnNames = NULL;
	config.columnTypes = NULL;
	config.primaryKeys = NULL;
	config.columnCount = 5;
	config.pkCount = 2;

    pushToList(&config.columnTypes, "red1", sizeofString("reda"));
    pushToList(&config.columnTypes, "red2", sizeofString("reda"));
    pushToList(&config.columnTypes, "red3", sizeofString("reda"));
    pushToList(&config.columnTypes, "red4", sizeofString("reda"));

    pushToList(&config.columnNames, "red1", sizeofString("reda"));
    pushToList(&config.columnNames, "red2", sizeofString("reda"));
    pushToList(&config.columnNames, "red3", sizeofString("reda"));
    pushToList(&config.columnNames, "red4", sizeofString("reda"));

   	int a = 7;
	pushToList(&config.primaryKeys, &a, sizeof(int));
	a = 2;
	pushToList(&config.primaryKeys, &a, sizeof(int));
	createTable(keyspaceName,tableName,&config);
}
void testReadKeyspaceConfig(){
	keyspaceConfig config;
	char *keyspaceName = "key";
	char * tableName = "table1";
	//createKeyspace(keyspaceName,&config);
	//deleteTable(keyspaceName,tableName);
	TableConfig * t = readTableConfig(keyspaceName,tableName);
	if(t == NULL){
		printf("Couldn't parse\n");
	}
	else{
		printf("%d\n",t->columnCount );
		printf("%d\n",t->pkCount );
		printList(t->columnNames,printString);
		printList(t->columnTypes,printString);
		printList(t->primaryKeys,printInt);
		printf("\nParsed\n");
	}
	return;

}
void testInsert(char* keyspaceName,char* tableName){
	Row row;
	row.state = 1;
	row.cells = NULL;
    pushToList(&row.cells, initCell("0"),sizeof(Cell));
    pushToList(&row.cells, initCell("0"),sizeof(Cell));
    pushToList(&row.cells, initCell("0"),sizeof(Cell));
    pushToList(&row.cells, initCell("0"),sizeof(Cell));
    pushToList(&row.cells, initCell("10/02/1220"),sizeof(Cell));
	insertRow(keyspaceName,tableName,&row,NULL);
}

void testAddKeyspace(char* keyspaceName){
	keyspaceConfig config;
	config.tableCount = 2;
	config.tableNames = NULL;
	int i;
	for (i=4; i>=0; i--)
      	pushToList(&config.tableNames, "table", sizeofString("table"));
	createKeyspace(keyspaceName,&config);
	//saveKeySpaceConfig(keyspaceName,&config);
}
void testreadTableData(char* keyspaceName,char* tableName){
	tableData * dataTable = readTableData(keyspaceName,tableName);
	printf("\nCount : %d\n", dataTable->rowCount);
	while (dataTable->rowData != NULL)
    {
    	Row *r = (Row*) dataTable->rowData->data;
	    while (r->cells != NULL)
	    {

			Cell * c = (Cell*) r->cells->data;
			printf("%s\n",c->value);
	      	r->cells = r->cells->next;
	    }
      	dataTable->rowData= dataTable->rowData->next;
    }
}

void testalterTableAddColumn(char * keyspaceName,char * tableName,char * columnName,char *columnType){
	int i = alterTableAddColumn(keyspaceName,tableName,columnName,columnType);
	printf("%d\n", i);
}

void testalterTableRenameColumn(char * keyspaceName,char * tableName,char * columnName,char *newColumnName){
	int i = alterTableRenameColumn(keyspaceName,tableName,columnName,newColumnName);
	printf("%d\n", i);
}



void testIsPrimary(char *columnName, char * keyspaceName,char * tableName){
	TableConfig  *t = readTableConfig(keyspaceName,tableName);
	bool state = isPrimary(columnName,t);
	printf("%d\n",state );
}
void testShowSelect(char *keyspaceName, char * tableName){
	node *columns=NULL;
	node *keys=NULL;
	int aa =0,a = 1,b=2,c=3,d=4;
    pushToList(&columns, &aa, sizeof(int));
    pushToList(&columns, &a, sizeof(int));
    pushToList(&columns, &b, sizeof(int));
    pushToList(&columns, &c, sizeof(int));
    pushToList(&columns, &d, sizeof(int));
    tableData * dataTable = readTableData(keyspaceName,tableName);
    //TableConfig *t = readTableConfig(keyspaceName,tableName);
    //tableData * dataTable1 = readTableData(keyspaceName,tableName);
    //filterString(dataTable1,t,"10/02/1210",4);
    //filterString(dataTable,t,"10/02/1210",4);
    //concatRows(dataTable->rowData,dataTable1->rowData,t);
   	//limitData(dataTable->rowData,2);
	//showSelectResult(keyspaceName,tableName,columns,dataTable);
   	SortRows(dataTable->rowData,0,ORDER_BY_DESC);
	//showSelectResult(keyspaceName,tableName,columns,dataTable);
	//deleteListRows(keyspaceName,tableName,getKeysFromData(dataTable));
}
void testupdateRows(char * keyspaceName,char * tableName){
	node *indexes=NULL;
	node *newValues=NULL;
	node *columns=NULL;
	int a = 1,b=3;
	pushToList(&indexes,&a,sizeof(int));
	pushToList(&indexes,&b,sizeof(int));
	int a1 =0,a2 = 1,a3=2,a4=3,a5=4;
    pushToList(&columns, &a1, sizeof(int));
    pushToList(&columns, &a2, sizeof(int));
    pushToList(&columns, &a3, sizeof(int));
    pushToList(&columns, &a4, sizeof(int));
    pushToList(&columns, &a5, sizeof(int));

	pushToList(&newValues,"uuuuu",sizeofString("uuuuu"));
	pushToList(&newValues,"zzzz",sizeofString("zzzz"));
	tableData * dataTable = readTableData(keyspaceName,tableName);
	TableConfig *t = readTableConfig(keyspaceName,tableName);
    filterString(dataTable,t,"10/02/1210",4);
	updateRows(keyspaceName,tableName,dataTable,indexes,newValues);
	//showSelectResult(keyspaceName,tableName,columns,dataTable);
}
void testlimitData(){
	node *columns=NULL;
	int a=1,b=2,c=3,d=4,e=5,f=6;
    pushToList(&columns, &a, sizeof(int));
    pushToList(&columns, &b, sizeof(int));
    pushToList(&columns, &c, sizeof(int));
    pushToList(&columns, &d, sizeof(int));
    pushToList(&columns, &e, sizeof(int));
    pushToList(&columns, &f, sizeof(int));
    printList(columns,printInt);
    limitData(columns,3);
    printList(columns,printInt);
}
void testConcatRow(){
	node *columns=NULL;
	node *columns1=NULL;
	int a=1,b=2,c=3,d=4,e=5,f=6;
    pushToList(&columns, &a, sizeof(int));
    pushToList(&columns, &b, sizeof(int));
    pushToList(&columns, &c, sizeof(int));
    pushToList(&columns1, &d, sizeof(int));
    pushToList(&columns1, &e, sizeof(int));
    pushToList(&columns1, &f, sizeof(int));
    printList(columns,printInt);
    //concatRows(columns,columns1);
    printList(columns,printInt);
}

/*
	MAIN

int main(){
	//testInsert("key","table1");
	//testreadTableData("key","table1");
	//testreadTableData("key","table1");
	//testAddTable("key","ta");
	//alterTableDropColumn("key","table1","cin");
	//testIsPrimary("date","key","table1");
	//testalterTableRenameColumn("key","table1","lname","lastName");
	//testalterTableAddColumn("key","table1","mark","integer");
	//testDeleteRows("key","table1");
	//testShowSelect("key","table1");
	//testupdateRows("key","table1");
	//testlimitData();

	return 1;
}*/