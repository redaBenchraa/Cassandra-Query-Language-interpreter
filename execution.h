#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <jansson.h>
#include <stdarg.h>


#ifdef _WIN32
#define DELETECMD "rmdir -/S "
#define DELIMITER "\\"
#endif

#ifdef linux
#define DELETECMD "rm -rf "
#define DELIMITER "/"
#endif

typedef int bool;
#define true 1
#define false 0
//Linked list fuctions
typedef struct node
{
    void  *data;
    struct node *next;
}node;

extern char *results[150];
//*****************----------****************
enum EXEC_CODES{
  OPEN_ERROR,
  KEYSPACE_EXISTS,
  KEYSPACE_NOT_EXISTS,
  KEYSPACE_CREATED,
  KEYSPACE_CONFIG_CREATED,
  KEYSPACE_CONFIG_NOT_CREATED,
  KEYSPACE_CONFIG_NOT_EXSISTS,
  FOLDER_NOT_EXISTS,
  FOLDER_EXISTS,
  ROOT_EXISTS,
  ROOT_CREATED,
  TABLE_EXISTS,
  TABLE_NOT_EXISTS,
  TABLE_CREATED,
  TABLE_CONFIG_CREATED,
  TABLE_CONFIG_NOT_CREATED,
  TABLE_ERROR_READING_DATA,
  TABLE_ERROR_READING_CONFIG,
  TABLE_TRUNCATED,
  FOLDER_ERROR_CREATE,
  FOLDER_DELETED,
  FOLDER_ERROR_DELETE,
  ROW_EXISTS,
  ROW_NULL,
  ROW_UPDATED,
  ROW_DELETED,
  ROW_INSERTED,
  ROW_NOT_EXISTS,
  COLUMN_VALUE_UPDATED,
  COLUMN_NOT_FOUND,
  COLUMN_DELETED,
  COLUMN_IS_PRIMARY,
  COLUMN_ALTERED,
  COLUMN_ALREADY_EXISTS,
  COLUMN_ADDED,
  OPS_INF,
  OPS_SUP,
  OPS_INFE,
  OPS_SUPE,
  OPS_EQ,
  OPS_DIFF,
  ORDER_BY_ASC,
  ORDER_BY_DESC,
  INDEX_AND_VALUES_NOT_SAME_SIZE,
  INDEX_NOT_FOUND,
  INCOMPATIBLE_TYPES,
  VALID_ENTERY,
  ALL_PRIMARY_HERE,
  MISSING_PRIMARY,
  MISSING_COLUMN,
  ALL_COLUMNS
};
typedef struct Cell{
  int size;
  char *value;
}Cell;

typedef struct Row{
  int state;
  char *key;
  node* cells;
}Row;

typedef struct tableData{
  int rowCount;
  node* rowData;
}tableData;

typedef struct primaryKey{
  int idKey;
  int columnIdCount;
  node *columnIds;
}primaryKey;

typedef struct TableConfig{
  int columnCount;
  node* columnNames;
  node* columnTypes;
  int pkCount;
  node* primaryKeys;
}TableConfig;

typedef struct Table{
  TableConfig config;
  tableData data;
}Table;

typedef struct keyspaceConfig{
  int tableCount;
  node *tableNames;
}keyspaceConfig;

typedef struct Keyspace{
  keyspaceConfig config;
  Table *tables;
}Keyspace;

typedef struct rootConfig{
  int a;
}rootConfig;

typedef struct root{
  rootConfig config;
  Keyspace *keyspaces;
}root;

/*Create root folder
FOLDER_ERROR_CREATE
ROOT_CREATED
*/
int createRoot();
/*Create keyspace folder
FOLDER_ERROR_CREATE
KEYSPACE_CREATED
KEYSPACE_EXISTS
*/
int createKeyspace(char *name,keyspaceConfig *config);
/*Create table folder
returns
TABLE_CREATED
FOLDER_ERROR_CREATE
KEYSPACE_NOT_EXISTS
TABLE_EXISTS
*/
int createTable(char *keyspaceName, char *tableName, TableConfig *config);
/*Delete keyspace folder with all tables
returns
FOLDER_DELETED
FOLDER_ERROR_DELETE
*/
int deleteKeyspace(char *name);
/*
Delete table folder
returns
FOLDER_NOT_EXISTS
FOLDER_DELETED
FOLDER_ERROR_DELETE
*/
int deleteTable(char *keyspaceName,char *name);
/*Delete root folder
returns
FOLDER_DELETED
FOLDER_ERROR_DELETE
*/
int deleteAll();
/*Insert row into a table
retuen
ROW_EXISTS
ROW_INSERTED
*/
int insertRow(char* keyspaceName,char *tableName,Row *rowData,char* keyValue);
/*
returns
TABLE_NOT_EXISTS
OPEN_ERROR
ROW_DELETED
*/
int deleteRow(char* keyspaceName,char *tableName,char*key);
/*
returns
COLUMN_NOT_FOUND
ROW_NULL
COLUMN_VALUE_UPDATED
*/
int updateRowValue(Row* row,int index, char*newValue);
/*
returns
ROW_NOT_EXISTS
ROW_UPDATED
*/
int updateRowInJSON(char* keyspaceName,char *tableName,Row* rowData);
/* retruns
COLUMN_IS_PRIMARY
TABLE_ERROR_READING_CONFIG
COLUMN_NOT_FOUND
COLUMN_DELETED
TABLE_ERROR_READING_DATA
deleteTable() errors
createTable() errors
*/
int alterTableDropColumn(char * keyspaceName,char * tableName,char * columnName);
/*
  return
  deleteTable() errors
  createTavle() errors
  TABLE_TRUNCATED
*/
int truncateTable(char* keySpaceName,char* tableName);
/*
COLUMN_ALREADY_EXISTS
COLUMN_ADDED
deleteTable() error
createTable() error
*/
int alterTableAddColumn(char * keyspaceName,char * tableName,char * columnName,char *columnType);
/*
createTableConfig() error
COLUMN_ALTERED
COLUMN_NOT_FOUND
*/
int alterTableRenameColumn(char * keyspaceName,char * tableName,char * columnName,char *newColumnName);
/*
KEYSPACE_CONFIG_CREATED
KEYSPACE_CONFIG_NOT_CREATED
*/
int saveKeySpaceConfig(char * keyspaceName,keyspaceConfig *kc);
/* Check if folder exists
return
FOLDER_EXISTS
FOLDER_NOT_EXISTS
*/
int checkFolder(char *name);
/*
TABLE_CONFIG_CREATED
TABLE_CONFIG_NOT_CREATED
*/
int createTableConfig(char* keyspaceName,char* tableName,TableConfig *config);
/*
returns
ROW_NOT_EXISTS
INDEX_AND_VALUES_NOT_SAME_SIZE
ROW_UPDATED
*/
int updateRow(char* keyspaceName,char *tableName,Row* row,node *listIndexes,node *listValues);
tableData* readTableData(char* keyspaceName,char* tableName);
TableConfig* readTableConfig(char *keyspaceName,char *tableName);
keyspaceConfig * readKeyspaceConfig(char * keyspaceName);
int compareRowsPK(Row* r1,Row* r2,int nbr,node *primaryKeys);
node* getKeysFromData(tableData*data);
void testupdate(char * keyspaceName,char * tableName);


void pushToList(struct node** head_ref, void *new_data, size_t data_size);
void printList(struct node *node, void (*fptr)(void *));
int getLinkedListSize(node *n);
void printInt(void *n);
void printString(void *c);
void printCell(void *c);
int sizeofString(char *c);
char* concat(const char *s1, const char *s2);
Cell * initCell(char*val);
void removeDuplicates(node *start,TableConfig * t);
int compareIndex(Row *n1,Row *n2,int index);
void SortRows(node *start,int index,int order);
void swap(struct node *a, struct node *b);
void concatRows(node *n1,node *n2,TableConfig * t);
void limitData(node *n1,int limit);
int checkTable(char*keyspaceName,char*tableName);
int showSelectResult(char*keyspaceName,char*tableName,node*listColumn,node*asNames,tableData *dataTable);