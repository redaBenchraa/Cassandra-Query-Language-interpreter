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
		"user",
		"users",
		"password",
		"describe",
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
/* Cette fonction s'assure qu'il n'y'a aucun problème au niveau de l'instruction (create,describe,etc.)
   et renvoit à la fonction correspondante à l'instruction
*/
void interpret(){
	if(CURRENT_PARENT_INST != USE_TOKEN && CURRENT_PARENT_INST != LIST_TOKEN 
		&& !(CURRENT_PARENT_INST == CREATE_TOKEN && CURRENT_CHILD_INST == KEYSPACE_TOKEN)
		&& !(CURRENT_PARENT_INST == DESCRIBE_TOKEN && CURRENT_CHILD_INST == KEYSPACE_TOKEN)
		&& !(CURRENT_PARENT_INST == CREATE_TOKEN && CURRENT_CHILD_INST == USER_TOKEN)
		&& !(CURRENT_PARENT_INST == DROP_TOKEN && CURRENT_CHILD_INST == USER_TOKEN)
		&& !(CURRENT_PARENT_INST == ALTER_TOKEN && CURRENT_CHILD_INST == USER_TOKEN)
		&& !(CURRENT_PARENT_INST == DROP_TOKEN && CURRENT_CHILD_INST == KEYSPACE_TOKEN) 
		&& current_keyspace == NULL)
	{
		char * result = malloc(256); 
		sprintf(result,"%s","Il faut d'abord specifier la base de donees\n");
		pushResult(result,sizeofString(result)+1);
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
	        case LIST_TOKEN		: list_interpreter(); 	 break;
	        case DESCRIBE_TOKEN	: describe_interpreter(); 	 break;
	        default             : break;
		}
	}
}
//----------------------------------------------------
/*
Cette fonction concerne l'instruction use keyspace, si le keyspace existe, on l'utilise,
sinon on déclare l'erreur et on remet le keyspace courant à NULL
*/
void use_interpreter(){
	if(checkKeyspace(current_keyspace) == FOLDER_NOT_EXISTS){
        char * result = malloc(256); 
        sprintf(result,"%s","Le keyspace n'existe pas\n");
        pushResult(result,sizeofString(result)+1);
		free(current_keyspace);
		current_keyspace = NULL;
	}else{
		char * result = malloc(256); 
		sprintf(result,"Le Keyspace courant est %s\n",current_keyspace);
		pushResult(result,sizeofString(result)+1);
	}
}
//----------------------------------------------------
/*
Cette fonction concerne l'instruction describe, si on décrit une table, elle nous renvoie à interpret_describeTable(),
Sinon si on décrit un keyspace, elle nous renvoie à interpret_describeKeyspace()
*/
void describe_interpreter(){
	switch(CURRENT_CHILD_INST){
		case TABLE_TOKEN : interpret_describeTable();break;
		case KEYSPACE_TOKEN :  interpret_describeKeyspace();break;
    	default : printf("%s\n",table_name);break;
	}
}
//---------------------------------------------------
/*
Cette fonction décrit une table, elle s'assure qu'elle existe, puis on stock ses informations dans une variable t de type TableConfig
après on adapte le contenu dans la chaine "result" qu'on affiche et qu'on vide régulièrement.
*/
void interpret_describeTable(){
	if(checkTable(current_keyspace,table_name) == FOLDER_EXISTS){
		TableConfig * t = readTableConfig(current_keyspace,table_name);
		char * result = malloc(256); 
	    sprintf(result,"==========La table : %s===========\n",table_name);
	    pushResult(result,sizeofString(result)+1);
	    result = malloc(256); 
	    sprintf(result,"Le nombre des columns : %d\n",t->columnCount);
	    pushResult(result,sizeofString(result)+1);
	    node *col = t->columnNames;
	    node *types = t->columnTypes;
	    node *pks = t->primaryKeys;
		while(col != NULL){
			result = malloc(256); 
	    	sprintf(result,"%s\t%s\n",(char*) col->data,(char*) types->data);
	    	pushResult(result,sizeofString(result)+1);	
	    	col = col->next;
	    	types = types->next;
		}
		result = malloc(256); 
	    sprintf(result,"%d Primary keys :",t->pkCount);
	    pushResult(result,sizeofString(result)+1);
	    while(pks != NULL){
			result = malloc(256); 
			int index = *(int*)pks->data;
	    	sprintf(result,"%s ",getColumnName(current_keyspace,table_name,index));
	    	pushResult(result,sizeofString(result)+1);	
	    	pks = pks->next;
	    }
	    result = malloc(256); 
	    sprintf(result,"\n==================================\n",table_name);
	    pushResult(result,sizeofString(result)+1);
	}else{

		char * result = malloc(256); 
	    sprintf(result,"La table '%s' n'existe pas dans '%s'\n",table_name,current_keyspace);
	    pushResult(result,sizeofString(result)+1);
	}
}
/*
Cette fonction se base sur le même principe que la précédente, sauf que celle-ci utilise keyspaceConfig au lieu de TableConfig
*/
void interpret_describeKeyspace(){
	if(checkKeyspace(describe_keyspace) != FOLDER_NOT_EXISTS){
		char * result = malloc(256); 
	    sprintf(result,"=========== Keyspace : %s===========\n",describe_keyspace);
	    pushResult(result,sizeofString(result)+1);
		keyspaceConfig *config =readKeyspaceConfig(describe_keyspace);
		node * tables = config->tableNames ;
		while(tables!=NULL){
			result = malloc(256); 
			sprintf(result,"%s\n",(char*)tables->data);
			pushResult(result,sizeofString(result)+1);
			tables = tables->next;
		}
		result = malloc(256); 
	    sprintf(result,"\n==================================\n",table_name);
	    pushResult(result,sizeofString(result)+1);
	}else{
		char * result = malloc(256); 
        sprintf(result,"%s","Le keyspace n'existe pas\n");
        pushResult(result,sizeofString(result)+1);
		free(describe_keyspace);
		describe_keyspace = NULL;
	}
	
}
//---------------------------------------------------
/*
Cette fonction nous permet de créer soit une table, un keyspace ou un utilisateur, et elle nous renvoit à interpret_createTable(), interpret_createKeyspace() et interpret_createUser()
*/
void create_interpreter(){	
	switch(CURRENT_CHILD_INST){
		case TABLE_TOKEN : interpret_createTable();break;
		case KEYSPACE_TOKEN :  interpret_createKeyspace();break;
		case USER_TOKEN : interpret_createUser();break;
    	default : break;
	}
}
/*
Cette fonction se charge de la création d'une table. Si la table existe déjà ou bien que le keyspace n'existe pas, elle affiche erreur.
Sinon, on s'assure, dans le cas où il y'aurait des Primary Keys, que ces colonnes existent réellement, dans le cas contraire, on affiche l'erreur
*/
void interpret_createTable(){
	if(checkTable(current_keyspace,table_name) != FOLDER_EXISTS){
	    if(current_keyspace == NULL){
        	char * result = malloc(256); 
	        sprintf(result,"%s","keyspace NULL\n");
	        pushResult(result,sizeofString(result)+1);	    	
	      
	    }
	    else{
	        TableConfig *config = malloc(sizeof(TableConfig));
	        config->columnNames = columnNames;
	        config->columnTypes = columnTypes;
	        config->primaryKeys = primaryKeys;
	        config->columnCount = columnCount;
	        if(primaryKeys != NULL){
	        	node *pks = primaryKeys;
	        	while ( pks != NULL )
	        	{
	        		printf("PK : %d\n",*(int*)pks->data);
	        		bool found = false;
	        	 	node* columns = columnNames;
					int i= 0;
	        	 	while(columns != NULL){
	        	 		int index1 = *(int*) pks->data;
	        	 		if(index1 == i) found = true;
	        	 		columns = columns->next;
	        	 		i++;
	        	 	}
	        	 	if(found == false){
	        	 		char * result = malloc(256); 
	        			sprintf(result,"primary keys list isn't consistant\n");
	        			pushResult(result,sizeofString(result)+1);
	        	 		return;
	        	 	}
	        	 	pks = pks->next;

	        	}
	        } 
	        config->pkCount = pkCount;
	        int i = createTable(current_keyspace,table_name,config);
			char * result = malloc(	256); 
	        sprintf(result,"%s\n",results[i]);
	        pushResult(result,sizeofString(result)+1);
	    }
	}else {
		char * result = malloc(256); 
	    sprintf(result,"La table '%s' existe deja dans '%s'\n",table_name,current_keyspace);
	    pushResult(result,sizeofString(result)+1);
		
	}

}
/*
Cette fonction crée le keyspace demandé
*/
void interpret_createKeyspace(){
	keyspaceConfig config;
	config.tableCount = 0;
	config.tableNames =  NULL;
	int i = createKeyspace(current_keyspace,&config);
	int j = saveKeySpaceConfig(current_keyspace,&config);
	char * result = malloc(256); 
	sprintf(result,"%s \n%s\n",results[i],results[j]);
	pushResult(result,sizeofString(result)+1);
}
//----------------------------------------------------
/*
Cette fonction nous renvoie à interpret_alterTable(),interpret_alterKeyspace() ou bien interpret_alterUser(),
selon qu'on veuille modifier une table, un keyspace ou bien un utilisateur.
*/
void alter_interpreter(){
	switch(CURRENT_CHILD_INST){
		case TABLE_TOKEN : interpret_alterTable();break;
		case KEYSPACE_TOKEN :  interpret_alterKeyspace();break;
		case USER_TOKEN :  interpret_alterUser();break;
    	default : break;
	}	
}
/*
Cette fonction nous permet d'ajouter, supprimer et renommer une colonne, selon l'instruction qui suit le "alter"
*/
void interpret_alterTable(){
	int res;
	switch(CURRENT_DESCENDANT_INST){
		case ADD_TOKEN: 
			res = alterTableAddColumn(current_keyspace,table_name,column_name1,column_type1);
		break;
		case DROP_TOKEN: 
			res = alterTableDropColumn(current_keyspace,table_name,column_name1);
		break;
		case RENAME_TOKEN: 
			res = alterTableRenameColumn(current_keyspace,table_name,column_name1,column_name2);
		break;
		case USER_TOKEN : interpret_alterUser();break;
	  	default : break;
	}
	char * result = malloc(256); 
	sprintf(result,"%s\n",results[res]);
	pushResult(result,sizeofString(result)+1);
	
}
void interpret_alterKeyspace(){
	// Nothing to do here since we don't use nodes and clusters.
}
//----------------------------------------------------
/*
Cette fonction nous permet de supprimer une table, un keyspace ou un utilisateur
*/
void drop_interpreter(){
	switch(CURRENT_CHILD_INST){
		case TABLE_TOKEN : interpret_dropTable();break;
		case KEYSPACE_TOKEN :  interpret_dropKeyspace();break;
		case USER_TOKEN : interpret_dropUser();break;
    	default : break;
	}	
}
/*
Ces deux fonctions qui suivent reposent sur le même principe. Elles suppriment respectivement une table et un keyspace.
*/
void interpret_dropTable(){
	int res = deleteTable(current_keyspace,table_name);
	char * result = malloc(256); 
	sprintf(result,"%s\n",results[res]);
	pushResult(result,sizeofString(result)+1);
}
void interpret_dropKeyspace(){
	int res = deleteKeyspace(column_name1);
	char * result = malloc(256); 
	sprintf(result,"%s\n",results[res]);
	pushResult(result,sizeofString(result)+1);
}
//----------------------------------------------------
void insert_interpreter(){
	if(checkTable(current_keyspace,table_name) == FOLDER_EXISTS){
	if( all_columns() == MISSING_COLUMN ) {
		char * result = malloc(256); 
		sprintf(result,"%s\n",results[all_columns()]);
		pushResult(result,sizeofString(result)+1);
		//printf("%s\n",results[all_columns()] ); 
		return;
	}
	int typeCheck = checkTypes(columnNames,types);
	if(typeCheck == INCOMPATIBLE_TYPES) {
		char * result = malloc(256); 
		sprintf(result,"%s\n",results[typeCheck]);
		pushResult(result,sizeofString(result)+1);
		return;
	}
	if(columnNames == NULL){
		int res = insertRow(current_keyspace,table_name,row,NULL);
		char * result = malloc(256); 
		sprintf(result,"%s\n",results[res]);
		pushResult(result,sizeofString(result)+1);

		//printf("%s\n",results[res]);
	}else{
		int prim = all_primary(columnNames);
		if (prim == MISSING_PRIMARY ) {
			char * result = malloc(256); 
			sprintf(result,"%s\n",results[prim]);
			pushResult(result,sizeofString(result)+1);		
			//printf("%s\n",results[prim]);
			return;
		}
		adapt_row();
		int res = insertRow(current_keyspace,table_name,newRow,NULL);
		char * result = malloc(256); 
		sprintf(result,"%s\n",results[res]);
		pushResult(result,sizeofString(result)+1);

		//printf("%s\n",results[res]);
		}
	}else{
		char * result = malloc(256); 
		sprintf(result,"%s n'existe pas dans la liste des tables\n",table_name);
		pushResult(result,sizeofString(result)+1);
	}
}
//----------------------------------------------------
void update_interpreter(){
	if(checkTable(current_keyspace,table_name) == FOLDER_EXISTS){
		bool error = false;
		node * in = NULL;
		int typesCheck;
		int i;
		tableData * dataTable = readTableData(current_keyspace,table_name);
		TableConfig *t = readTableConfig(current_keyspace,table_name);
		typesCheck = checkTypes(indexes,types);
		node * indexedTemp = indexes;
		while ( indexedTemp != NULL )
		{
		  indexedTemp = indexedTemp->next;
		  TypesWhere = TypesWhere->next;
		}
		while(indexes != NULL)
		{
			char*n=(char*)indexes->data;
			int index = getColumnIndex(current_keyspace,table_name,n);
			if(index < 0) {
				char * result = malloc(256); 
				sprintf(result,"%s n'existe pas dans la liste des columns\n",n);
				pushResult(result,sizeofString(result)+1);
				printf("%s n'existe pas dans la liste des columns\n",n);
				error = true;
			}
	    	pushToList(&in,&index,sizeof(int));	
			indexes = indexes->next;
		}
		if(error == false){
			if(typesCheck == INCOMPATIBLE_TYPES) {
			char * result = malloc(256); 
			sprintf(result,"%s\n",results[typesCheck]);
			pushResult(result,sizeofString(result)+1);	
			//printf("%s\n",results[typesCheck]); 
			return;
			}
			if(selectWhere(dataTable,t) != -1){
				int res = updateRows(current_keyspace,table_name,dataTable,in,newValues);
				char * result = malloc(256); 
				sprintf(result,"%s\n",results[ROW_UPDATED]);
				pushResult(result,sizeofString(result)+1);		
				//printf("%s\n",results[ROW_UPDATED]);
			}	
		}
	}else {
		char * result = malloc(256); 
		sprintf(result,"%s n'existe pas dans la liste des tables\n",table_name);
		pushResult(result,sizeofString(result)+1);
		//printf("%s n'existe pas dans la liste des tables\n",table_name);
	}
}
//----------------------------------------------------
void delete_interpreter(){
	if(checkTable(current_keyspace,table_name) == FOLDER_EXISTS){
		tableData * dataTable = readTableData(current_keyspace,table_name);
		TableConfig *t = readTableConfig(current_keyspace,table_name);
		node * in = NULL;
		bool error = false;
		int i;
		while(indexes != NULL)
		{
			char*n=(char*)indexes->data;
			int index = getColumnIndex(current_keyspace,table_name,n);
			if(index < 0) {
				char * result = malloc(256); 
				sprintf(result,"%s n'existe pas dans la liste des columns\n",n);
				pushResult(result,sizeofString(result)+1);
				//printf("%s n'existe pas dans la liste des columns\n",n);
				error = true;
			}
	    	pushToList(&in,&index,sizeof(int));	
			indexes = indexes->next;
		}
		if(error == false){
			if(selectWhere(dataTable,t) != -1){
				printf("Size of delete : %d \n", getLinkedListSize(dataTable->rowData));
				node *l= getKeysFromData(dataTable);
				int res = deleteListRows(current_keyspace,table_name,l);
				char * result = malloc(256); 
				sprintf(result,"%s\n",results[ROW_DELETED]);
				pushResult(result,sizeofString(result)+1);		
				//printf("%s\n",results[ROW_DELETED]);
			}
		}
	}else {
		char * result = malloc(256); 
		sprintf(result,"%s n'existe pas dans la liste des tables\n",table_name );
		pushResult(result,sizeofString(result)+1);
		//printf("%s n'existe pas dans la liste des tables\n",table_name);
	}
}
//----------------------------------------------------
void select_interpreter(){
	if(checkTable(current_keyspace,table_name) == FOLDER_EXISTS){
		tableData * dataTable = readTableData(current_keyspace,table_name);
		TableConfig *t = readTableConfig(current_keyspace,table_name);
		bool error = false;
		node * in = NULL;
		int i;
		printf("==============================================================\n");
		while(indexes != NULL)
		{
			char*n=(char*)indexes->data;
			int index = getColumnIndex(current_keyspace,table_name,n);
			if(index < 0){
				char * result = malloc(256); 
				sprintf(result,"%s n'existe pas dans la liste des columns\n",n);
				pushResult(result,sizeofString(result)+1);
				printf("%s n'existe pas dans la liste des columns\n",n);
				error = true;
			}
	    	pushToList(&in,&index,sizeof(int));	
			indexes = indexes->next;
		}
		if(error == false){
			if(selectWhere(dataTable,t) != -1){
				if(limit > 0 ) limitData(dataTable->rowData,limit);
				if(orderBy != -1) {
					int order;
					if(orderBy == ASC_TOKEN) order = ORDER_BY_ASC;else order = ORDER_BY_DESC;
					SortRows(dataTable->rowData,getColumnIndex(current_keyspace,table_name,column_name1),order);
				}
				if(count_flag == true){
					if(asNames != NULL){
						char * result = malloc(256); 
						sprintf(result,"%s\n==========================================\n  %d\n",(char*)asNames->data,getLinkedListSize(dataTable->rowData)  );
						pushResult(result,sizeofString(result)+1);
						//printf("%s\n----------------------------\n  %d\n",(char*)asNames->data,getLinkedListSize(dataTable->rowData) );
					} 
					else{
						char * result = malloc(256); 
						sprintf(result,"COUNT(*)\n==========================================\n  %d\n",getLinkedListSize(dataTable->rowData));
						pushResult(result,sizeofString(result)+1);
						//printf("COUNT(*)\n----------------------------\n  %d\n",getLinkedListSize(dataTable->rowData));
					} 					
					return;
				}
			}else error = true;
			if(error == false) showSelectResult(current_keyspace,table_name,in,asNames,dataTable);
		}
	}else {
		char * result = malloc(256); 
		sprintf(result,"%s n'existe pas dans la liste des tables\n",table_name);
		pushResult(result,sizeofString(result)+1);
		//printf("%s n'existe pas dans la liste des tables\n",table_name);
	}
}
//----------------------------------------------------
void batch_interpreter(){}
//----------------------------------------------------
void using_interpreter(){}
//----------------------------------------------------

void interpret_createUser(){
	int res = createUser(row);
	char * result = malloc(256); 
	sprintf(result,"%s\n",results[res]);
	pushResult(result,sizeofString(result)+1);
}

void interpret_alterUser(){
	int res = alterUser(row);
	char * result = malloc(256); 
	sprintf(result,"%s\n",results[res]);
	pushResult(result,sizeofString(result)+1);}

void interpret_dropUser(){
	int res =dropUser(userName);
	char * result = malloc(256); 
	sprintf(result,"%s\n",results[res]);
	pushResult(result,sizeofString(result)+1);}

void list_interpreter(){
	node * users = getUsers();
	printf(" USERS \n");
	printf("-------\n");
	char * result = malloc(256); 
	sprintf(result,"%s","USERS\n################\n");
	pushResult(result,sizeofString(result)+1);
	while(users!=NULL){
		char * result = malloc(256); 
		sprintf(result,"%s\n",(char*)users->data );
		pushResult(result,sizeofString(result)+1);
		users = users->next->next;
	}
}

//---------------------------------------------------
int selectWhere(tableData * dataTable,TableConfig *t){
	tableData * dataTableAnd;
	int i;
	while(indexesWhere != NULL){
		char*op=(char*)opsWhere->data;
		char*n=(char*)indexesWhere->data;
		int index = getColumnIndex(current_keyspace,table_name,n);
		if(index < 0) {
			char * result = malloc(256); 
			sprintf(result,"%s n'existe pas dans la liste des columns\n",n);
			pushResult(result,sizeofString(result)+1);
			printf("%s n'existe pas dans la liste des columns\n",n);
			indexesWhere = indexesWhere->next;
			return -1;
		}
		int value;
		int ops;
		float fvalue;
		if(strcasecmp(op,"in") == 0){
			i=0;
			while ( inValues != NULL )
			{
				printf("TYPES IN WHERE : %s %s \n",KEYWORDS1[*(int*)TypesWhere->data],getColumnType(t,index) );
				if(strcmp(KEYWORDS1[*(int*)inTypes->data],getColumnType(t,index)) != 0 ) {
					char * result = malloc(256); 
					sprintf(result,"If faut assinger un %s à %s\n",getColumnType(t,index),n );
					pushResult(result,sizeofString(result)+1);
					return -1;
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
			printf("TYPES IN WHERE : %s(%s) %s \n",KEYWORDS1[*(int*)TypesWhere->data],nn,getColumnType(t,index) );
			if(strcmp(KEYWORDS1[*(int*)TypesWhere->data],getColumnType(t,index)) != 0 ) {
				char * result = malloc(256); 
				sprintf(result,"If faut assinger un %s à %s\n",getColumnType(t,index),n );
				pushResult(result,sizeofString(result)+1);
				return -1;
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
					}else if(strcmp((char*) opsWhere->data,"<>") == 0){
						ops = OPS_DIFF;
					}
					filterNumbers(dataTable,t,ops,value,index);
				break;
				case FLOAT_TOKEN: 
					fvalue = atof(nn);
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
					}else if(strcmp((char*) opsWhere->data,"<>") == 0){
						ops = OPS_DIFF;
					}
					filterFloats(dataTable,t,ops,fvalue,index);
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
	return 1;
}

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
