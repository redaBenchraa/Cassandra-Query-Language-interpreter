#include "interpreter.h"
#include "parser.h"
Row *newRow;
char KEYWORDS1[][50] = {
    //reserved words
		"add",
		"allow",
		"alter",
		"and",
		"or",
		"apply",
		"asc",
		"batch",
		"begin",
		"by",
		"clustering",
		"columnfamily",
		"compact",
		"count",
		"counter",
		"create",
		"custom",
		"delete",
		"desc",
		"drop",
		"exists",
		"false",
		"filtering",
		"from",
		"if",
		"in",
		"to",
		"index",
		"insert",
		"into",
		"key",
		"keyspace",
		"limit",
		"list",
		"map",
		"not",
		"on",
		"options",
		"order",
		"primary",
		"select",
		"set",
		"static",
		"storage",
		"table",
		"timestamp",
		"true",
		"truncate",
		"ttl",
		"type",
		"unlogged",
		"update",
		"use",
		"using",
		"values",
		"where",
		"with",
		"writetime",
		"distinct",
		"keys",
		"entries",
		"full",
		"default",
		"unset",
		"as",
		"token",
		"contains",
		"rename",
    // Operators

    "+",
    "-",
    "*",
    "/",
    "=",
    "<",
    ">",
    ".",
    ",",
    "(",
    ")",
    "[",
    "]",
    "{",
    "}",
    ":",
    ";",
    "^",
    "@",
    "$",
    "#",
    "&",
    "%",
    "<=",
    ">=",
	"<>",
    ":=",
    "+=",
    "-=",
    "*=",
    "/=",
	"'",
	"\"",
	"\\",
	"?",

	//Comments

	"//",
	"--",
	"/*",
	"*/",

	//NOTE: keep "END_MULTILINE_COMMENT" the last searchable item
	//Other types

	"idf",
	"string",
	"integer",
	"float",
	"uuid",
	"blob",
	"boolean",
	"hex",
	"END",
	"error"
};
void interpret(){
	if(CURRENT_PARENT_INST != USE_TOKEN 
		&& !(CURRENT_PARENT_INST == CREATE_TOKEN && CURRENT_CHILD_INST == KEYSPACE_TOKEN)
		&& !(CURRENT_PARENT_INST == DROP_TOKEN && CURRENT_CHILD_INST == KEYSPACE_TOKEN) 
		&& current_keyspace == NULL)
	{
		printf("Il faut d'abord specifier la base de donees\n");
	}else{
		switch(CURRENT_PARENT_INST){
	        case USE_TOKEN      : use_interpreter();     break;
			case CREATE_TOKEN   : create_interpreter();  break;
	        case USING_TOKEN    : using_interpreter();   break;
	        case ALTER_TOKEN    : alter_interpreter();   break;
	        case DROP_TOKEN     : drop_interpreter();    break;
	        case INSERT_TOKEN   : insert_interpreter();  break;
	        case UPDATE_TOKEN   : update_interpreter();  break;
	        case SELECT_TOKEN   : select_interpreter();  break;
	        case BEGIN_TOKEN    : batch_interpreter();   break;
	        case DELETE_TOKEN   : delete_interpreter();  break;
	        default             : break;//print_error_token(current_token.code);
		}
	}
}
//----------------------------------------------------

void use_interpreter(){
	if(checkKeyspace(current_keyspace) == FOLDER_NOT_EXISTS){
		printf("Le keyspace n'existe pas\n");
		free(current_keyspace);
		current_keyspace = NULL;
	}
}
//----------------------------------------------------
void create_interpreter(){	
	switch(CURRENT_CHILD_INST){
		case TABLE_TOKEN : interpret_createTable();break;
		case KEYSPACE_TOKEN :  interpret_createKeyspace();break;
    	default : break;
	}
}
void interpret_createTable(){
	if(checkTable(current_keyspace,table_name) != FOLDER_EXISTS){
	    if(current_keyspace == NULL){
	        printf("keyspace NULL\n");
	    }
	    else{
	        TableConfig *config = malloc(sizeof(TableConfig));
	        config->columnNames = columnNames;
	        config->columnTypes = columnTypes;
	        config->primaryKeys = primaryKeys;
	        config->columnCount = columnCount;
	        config->pkCount = pkCount;
	        int i = createTable(current_keyspace,table_name,config);
	        printf("%d",results[i]);
	    }
	}else printf("La table '%s' existe deja dans '%s'\n",table_name,current_keyspace);

}
void interpret_createKeyspace(){
	keyspaceConfig config;
	config.tableCount = 0;
	config.tableNames =  NULL;
	int i = createKeyspace(current_keyspace,&config);
	int j = saveKeySpaceConfig(current_keyspace,&config);
	printf("%s \n%s\n",results[i],results[j]);
}
//----------------------------------------------------
void alter_interpreter(){
	switch(CURRENT_CHILD_INST){
		case TABLE_TOKEN : interpret_alterTable();break;
		case KEYSPACE_TOKEN :  interpret_alterKeyspace();break;
    	default : break;
	}	
}
void interpret_alterTable(){
	int result;
	switch(CURRENT_DESCENDANT_INST){
		case ADD_TOKEN: 
			result = alterTableAddColumn(current_keyspace,table_name,column_name1,column_type1);
		break;
		case DROP_TOKEN: 
			result = alterTableDropColumn(current_keyspace,table_name,column_name1);
		break;
		case RENAME_TOKEN: 
			result = alterTableRenameColumn(current_keyspace,table_name,column_name1,column_name2);
		break;
	  	default : break;
	}
	printf("%s\n",results[result]);
}
void interpret_alterKeyspace(){
	// Nothing to do here since we don't use nodes and clusters.
}
//----------------------------------------------------
void drop_interpreter(){
	switch(CURRENT_CHILD_INST){
		case TABLE_TOKEN : interpret_dropTable();break;
		case KEYSPACE_TOKEN :  interpret_dropKeyspace();break;
    	default : break;
	}	
}
void interpret_dropTable(){
	int result = deleteTable(current_keyspace,table_name);
	printf("%s\n",results[result]);
}
void interpret_dropKeyspace(){
	int result = deleteKeyspace(column_name1);
	printf("%s\n",results[result]);
}
//----------------------------------------------------
void insert_interpreter(){
	if( all_columns() == MISSING_COLUMN ) {printf("%s\n",results[all_columns()] ); return;}
	int typeCheck = checkTypes(columnNames,types);
	if(typeCheck == INCOMPATIBLE_TYPES) {printf("%s\n",results[typeCheck] ); return;}
	if(columnNames == NULL){
		int result = insertRow(current_keyspace,table_name,row,NULL);
		printf("%s\n",results[result]);
	}else{
		int prim = all_primary(columnNames);
		if (prim == MISSING_PRIMARY ) {printf("%s\n",results[prim]);return;}
		adapt_row();
		int result = insertRow(current_keyspace,table_name,newRow,NULL);
		printf("%s\n",results[result]);
	}
}
//----------------------------------------------------
void update_interpreter(){
	bool error = false;
	node * in = NULL;
	int typesCheck;
	tableData * dataTableAnd;
	int i;
	if(checkTable(current_keyspace,table_name) == FOLDER_EXISTS){
		typesCheck = checkTypes(indexes,types);
		if(typesCheck == INCOMPATIBLE_TYPES) {printf("%s\n",results[typesCheck]); return;}
		while(indexes != NULL)
		{
			char*n=(char*)indexes->data;
			int index = getColumnIndex(current_keyspace,table_name,n);
			if(index < 0) {
				printf("%s n'existe pas dans la liste des columns\n",n);
				error = true;
			}
	    	pushToList(&in,&index,sizeof(int));	
			indexes = indexes->next;
		}
		if(error == false){
			tableData * dataTable = readTableData(current_keyspace,table_name);
			TableConfig *t = readTableConfig(current_keyspace,table_name);
			
			while(indexesWhere != NULL){
				char*op=(char*)opsWhere->data;
				char*n=(char*)indexesWhere->data;
				int index = getColumnIndex(current_keyspace,table_name,n);
				int value;
				int ops;
				if(strcasecmp(op,"in") == 0){
					i=0;
					while ( inValues != NULL )
					{
						if(strcmp(KEYWORDS1[*(int*)inTypes->data],getColumnType(t,index)) != 0 ) {
							printf("If faut assinger un %s à %s\n",getColumnType(t,index),n );
							return;
						}
						if(i == 0){
							filterString(dataTable,t,inValues->data,index);
						} else {
							dataTableAnd = readTableData(current_keyspace,table_name); 
							filterString(dataTableAnd,t,inValues->data,index);
							if(dataTable->rowData == NULL) 
								dataTable->rowData = dataTableAnd->rowData;
							else if(dataTableAnd->rowData != NULL)
								concatRows(dataTable->rowData,dataTableAnd->rowData,t);
						}
						i++;
						inTypes = inTypes->next;
						inValues = inValues->next;
					}

				}else{
				char*nn=(char*)valuesWhere->data;
					if(strcmp(KEYWORDS1[*(int*)TypesWhere->data],getColumnType(t,index)) != 0 ) {
						printf("If faut assinger un %s à %s\n",getColumnType(t,index),n );
						//return;
					}
					switch(*(int*)TypesWhere->data){
						case INT_TOKEN: 
							value = atoi(nn);
							if(strcmp((char*) opsWhere->data,"<") == 0){
								ops = OPS_INF;
							}else if(strcmp((char*) opsWhere->data,">") == 0){
								ops = OPS_SUP;
							}else if(strcmp((char*) opsWhere->data,"<=") == 0){
								ops = OPS_INFE;
							}else if(strcmp((char*) opsWhere->data,">=") == 0){
								ops = OPS_SUPE;
							}else if(strcmp((char*) opsWhere->data,"=") == 0){
								ops = OPS_EQ;
							}else if(strcmp((char*) opsWhere->data,"!=") == 0){
								ops = OPS_DIFF;
							}
							filterNumbers(dataTable,t,ops,value,index);
						break;
						case STRING_TOKEN: 
								filterString(dataTable,t,nn,index);
						break;
						default:break;
					}
				}
				indexesWhere = indexesWhere->next;
				valuesWhere = valuesWhere->next;
				TypesWhere = TypesWhere->next;
				opsWhere = opsWhere->next;
			}

			int result = updateRows(current_keyspace,table_name,dataTable,in,newValues);
			printf("%s\n",results[ROW_UPDATED]);
		}
	}else printf("%s n'existe pas dans la liste des tables\n",table_name);
}
//----------------------------------------------------
void delete_interpreter(){
	if(checkTable(current_keyspace,table_name) == FOLDER_EXISTS){
		tableData * dataTable = readTableData(current_keyspace,table_name);
		TableConfig *t = readTableConfig(current_keyspace,table_name);
		node * in = NULL;
		tableData * dataTableAnd;
		bool error = false;
		int i;
		while(indexes != NULL)
		{
			char*n=(char*)indexes->data;
			int index = getColumnIndex(current_keyspace,table_name,n);
			if(index < 0) {
				printf("%s n'existe pas dans la liste des columns\n",n);
				error = true;
			}
	    	pushToList(&in,&index,sizeof(int));	
			indexes = indexes->next;
		}
		if(error == false){
			while(indexesWhere != NULL){
				char*op=(char*)opsWhere->data;
				char*n=(char*)indexesWhere->data;
				int index = getColumnIndex(current_keyspace,table_name,n);
				int value;
				int ops;
				if(strcasecmp(op,"in") == 0){
					i=0;
					while ( inValues != NULL )
					{
						if(strcmp(KEYWORDS1[*(int*)inTypes->data],getColumnType(t,index)) != 0 ) {
							printf("If faut assinger un %s à %s\n",getColumnType(t,index),n );
							return;
						}
						if(i == 0){
							filterString(dataTable,t,inValues->data,index);
						} else {
							dataTableAnd = readTableData(current_keyspace,table_name); 
							filterString(dataTableAnd,t,inValues->data,index);
							if(dataTable->rowData == NULL) 
								dataTable->rowData = dataTableAnd->rowData;
							else if(dataTableAnd->rowData != NULL)
								concatRows(dataTable->rowData,dataTableAnd->rowData,t);
						}
						i++;
						inTypes = inTypes->next;
						inValues = inValues->next;
					}

				}else{
				char*nn=(char*)valuesWhere->data;
					if(strcmp(KEYWORDS1[*(int*)TypesWhere->data],getColumnType(t,index)) != 0 ) {
						printf("If faut assinger un %s à %s\n",getColumnType(t,index),n );
						//return;
					}
					switch(*(int*)TypesWhere->data){
						case INT_TOKEN: 
							value = atoi(nn);
							if(strcmp((char*) opsWhere->data,"<") == 0){
								ops = OPS_INF;
							}else if(strcmp((char*) opsWhere->data,">") == 0){
								ops = OPS_SUP;
							}else if(strcmp((char*) opsWhere->data,"<=") == 0){
								ops = OPS_INFE;
							}else if(strcmp((char*) opsWhere->data,">=") == 0){
								ops = OPS_SUPE;
							}else if(strcmp((char*) opsWhere->data,"=") == 0){
								ops = OPS_EQ;
							}else if(strcmp((char*) opsWhere->data,"!=") == 0){
								ops = OPS_DIFF;
							}
							filterNumbers(dataTable,t,ops,value,index);
						break;
						case STRING_TOKEN: 
								filterString(dataTable,t,nn,index);
						break;
						default:break;
					}
				}
				indexesWhere = indexesWhere->next;
				valuesWhere = valuesWhere->next;
				TypesWhere = TypesWhere->next;
				opsWhere = opsWhere->next;
			}
			node *l= getKeysFromData(dataTable);
			int result = deleteListRows(current_keyspace,table_name,l);
			printf("%s\n",results[ROW_DELETED]);
		}
	}else printf("%s n'existe pas dans la liste des tables\n",table_name);
}
//----------------------------------------------------
void select_interpreter(){
	if(checkTable(current_keyspace,table_name) == FOLDER_EXISTS){
		tableData * dataTableAnd;
		tableData * dataTable = readTableData(current_keyspace,table_name);
		TableConfig *t = readTableConfig(current_keyspace,table_name);
		bool error = false;
		node * in = NULL;
		int i;
		while(indexes != NULL)
		{
			char*n=(char*)indexes->data;
			int index = getColumnIndex(current_keyspace,table_name,n);
			if(index < 0) {
				printf("%s n'existe pas dans la liste des columns\n",n);
				error = true;
			}
	    	pushToList(&in,&index,sizeof(int));	
			indexes = indexes->next;
		}
		if(error == false){

			while(indexesWhere != NULL){
				char*op=(char*)opsWhere->data;
				char*n=(char*)indexesWhere->data;
				int index = getColumnIndex(current_keyspace,table_name,n);
				int value;
				int ops;
				if(strcasecmp(op,"in") == 0){
					i=0;
					while ( inValues != NULL )
					{
						if(strcmp(KEYWORDS1[*(int*)inTypes->data],getColumnType(t,index)) != 0 ) {
							printf("If faut assinger un %s à %s\n",getColumnType(t,index),n );
							return;
						}
						if(i == 0){
							filterString(dataTable,t,inValues->data,index);
						} else {
							dataTableAnd = readTableData(current_keyspace,table_name); 
							filterString(dataTableAnd,t,inValues->data,index);
							if(dataTable->rowData == NULL) 
								dataTable->rowData = dataTableAnd->rowData;
							else if(dataTableAnd->rowData != NULL)
								concatRows(dataTable->rowData,dataTableAnd->rowData,t);
						}
						i++;
						inTypes = inTypes->next;
						inValues = inValues->next;
					}

				}else{
				char*nn=(char*)valuesWhere->data;
					if(strcmp(KEYWORDS1[*(int*)TypesWhere->data],getColumnType(t,index)) != 0 ) {
						printf("If faut assinger un %s à %s\n",getColumnType(t,index),n );
						//return;
					}
					switch(*(int*)TypesWhere->data){
						case INT_TOKEN: 
							value = atoi(nn);
							if(strcmp((char*) opsWhere->data,"<") == 0){
								ops = OPS_INF;
							}else if(strcmp((char*) opsWhere->data,">") == 0){
								ops = OPS_SUP;
							}else if(strcmp((char*) opsWhere->data,"<=") == 0){
								ops = OPS_INFE;
							}else if(strcmp((char*) opsWhere->data,">=") == 0){
								ops = OPS_SUPE;
							}else if(strcmp((char*) opsWhere->data,"=") == 0){
								ops = OPS_EQ;
							}else if(strcmp((char*) opsWhere->data,"!=") == 0){
								ops = OPS_DIFF;
							}
							filterNumbers(dataTable,t,ops,value,index);
						break;
						case STRING_TOKEN: 
								filterString(dataTable,t,nn,index);
						break;
						default:break;
					}
				}
				indexesWhere = indexesWhere->next;
				valuesWhere = valuesWhere->next;
				TypesWhere = TypesWhere->next;
				opsWhere = opsWhere->next;
			}
			if(limit > 0 ) limitData(dataTable->rowData,limit);
			if(orderBy != -1) {
				int order;
				if(orderBy == ASC_TOKEN) order = ORDER_BY_ASC;else order = ORDER_BY_DESC;
				SortRows(dataTable->rowData,getColumnIndex(current_keyspace,table_name,column_name1),order);
			}
			if(count_flag == true){
				if(asNames != NULL) 
					printf("%s\n----------------------------\n  %d\n",(char*)asNames->data,getLinkedListSize(dataTable->rowData) );
				else 					
					printf("COUNT(*)\n----------------------------\n  %d\n",getLinkedListSize(dataTable->rowData));
				return;
			}
			showSelectResult(current_keyspace,table_name,in,asNames,dataTable);
		}
	}else printf("%s n'existe pas dans la liste des tables\n",table_name);
}
//----------------------------------------------------
void batch_interpreter(){}
//----------------------------------------------------
void using_interpreter(){}
//----------------------------------------------------



void adapt_row(){
	TableConfig *t = readTableConfig(current_keyspace,table_name);
	node * configNames = t->columnNames;
	node * names = columnNames;
	node * r = row->cells;
	//Row * newRow;
	newRow = (Row*) malloc(sizeof(Row));
    newRow->state = row->state;
    newRow->cells = NULL;
	while(configNames!=NULL){
		names = columnNames;
		r = row->cells;
		while(names != NULL && strcmp((char*)names->data,(char*)configNames->data)!=0){
			names = names->next;
			r = r->next;
		}
		if(names!=NULL){
			Cell *c = (Cell*) r->data;
			pushToList(&newRow->cells, initCell(c->value),sizeof(Cell));
		}
		else{
			pushToList(&newRow->cells, initCell("null"),sizeof(Cell));
		}
		configNames = configNames->next;
	}
}
int all_columns(){
	node *columns = row->cells;
	node *names;
	int colValCount = 0,colCount = 0;
	TableConfig *t = readTableConfig(current_keyspace,table_name);
	while ( columns != NULL ){
		    colValCount++;
		    columns = columns->next;
	}
	if( columnNames == NULL){
		if( colValCount != t->columnCount) return MISSING_COLUMN;
	}
	else{
		names = columnNames;
		while( names != NULL){
			colCount++;
			names = names->next;
		}
		if(colCount != colValCount) return MISSING_COLUMN;
	}

	return ALL_COLUMNS;
}
int checkTypes(node *columns,node *types){

	TableConfig * config = readTableConfig(current_keyspace,table_name);
	node *col = NULL;
	bool state;
	if(config != NULL){
		if(columns == NULL) columns = config->columnNames; 
		if(types == NULL) types = config->columnTypes;
		col = columns;
		while(col != NULL){		
			state = checkType(col,KEYWORDS1[*(int*)types->data],config->columnNames,config->columnTypes);
			if(state == true) types = types->next;
			else return INCOMPATIBLE_TYPES;
			col = col->next;
		}
	}
	return VALID_ENTERY;
}

bool checkType(node* columns,char* type,node* configColumns,node* configTypes){
	node* cTypes = configTypes;
	int j = getColumnIndex1(configColumns,(char*)columns->data);
	int i = 0;
	if(j != -1){
		while(i != j){
			i++;
			cTypes = cTypes->next;
		}
		if(strcmp((char*)cTypes->data,type)==0) return true;
		else return false;
	}
}

int all_primary(node *columns){
	TableConfig *t = readTableConfig(current_keyspace,table_name);
	int count=0,index;

	node *col,*pks;
	col = columns;
	int i=0;
	while(col != NULL){
		pks = t->primaryKeys;
		index = getColumnIndex1(t->columnNames,(char*)col->data);
		if(index < 0 ) return COLUMN_NOT_FOUND;
		while(pks != NULL){
			if(index == *(int*)pks->data){
				count++;
			}
			pks = pks->next;
		}

		col = col->next;
	}
	if(count == t->pkCount) return ALL_PRIMARY_HERE;
	else return MISSING_PRIMARY;
}