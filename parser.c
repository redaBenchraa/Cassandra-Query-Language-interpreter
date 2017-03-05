#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "resultHandler.h"

resultNode *resultList;
int dsc;

Token * tokenList;
Token current_token;
Token last_token;
bool no_error = true;
int CURRENT_PARENT_INST;
int CURRENT_CHILD_INST;
int CURRENT_DESCENDANT_INST;
char *current_keyspace;
char *table_name;
char *column_name1;
char *column_name2;
char *column_type1;
node *indexes;
node *inValues;
node *inTypes;
node *newValues;
node *indexesWhere;
node *valuesWhere;
node *TypesWhere;
node *opsWhere;
Row *row;
int pkCount;
int columnCount;
node* columnNames;
node* columnTypes;
node* primaryKeys;
node* types;
node* asNames;
node* andOrIn;
bool if_exist;
bool count_flag;
int limit;
int orderBy;
bool ready_for_execution;
bool in_flag = true;
char lastOp[5];
char *userName;
char* describe_keyspace;
void getToken(){
    if(tokenList != NULL){
        memcpy(&last_token,&current_token,sizeof(Token));
        memcpy(&current_token,tokenList,sizeof(Token));
        tokenList = tokenList->next;
    }
}
int search_column(char* colName){
        node * columns = columnNames;
        while ( columns!= NULL){
                if(strcmp(colName,(char*)columns->data) == 0){
                    return COLUMN_ALREADY_EXISTS;
                }
            columns = columns->next;
        }
        return COLUMN_NOT_FOUND;
}

void print_error_token(int code)
{
    if( ready_for_execution == true ){
        char * result = malloc(256); 
        if( code == ERROR_TOKEN)
        {
                sprintf(result,"Erreur Lexical [%s] dans la  Ligne [%d] Column [%d]\n",current_token.value,current_token.row,current_token.col);
                pushResult(result,sizeofString(result)+1);
        }
        else
        {
                sprintf(result,"Erreur Syntaxique [%s] dans la Ligne [%d] Column [%d]\n",current_token.value, current_token.row, current_token.col);
                pushResult(result,sizeofString(result)+1);
        }
        ready_for_execution = false;
    }
}
void parse()
{
    getToken();
    _program();
    if(current_token.code == END_TOKEN && no_error == true)
    {
        //printf("\nLe programme a parsé les requetes avec success\n");
    }
    else
    {
        char*result = malloc(250);
        sprintf(result,"%s","\nErreur en parsing\n");
        pushResult(result,sizeofString(result)+1);
    }
}

void _program()
{
    while(current_token.code != END_TOKEN)
    {
        userName = (char*)malloc(20*sizeof(char));
        describe_keyspace = (char*)malloc(20*sizeof(char));
        resultList = NULL;
        indexes = NULL;
        newValues = NULL;
        indexesWhere = NULL;
        valuesWhere = NULL;
        TypesWhere = NULL;
        opsWhere = NULL;
        types = NULL;
        asNames = NULL;
        columnNames = NULL;
        columnTypes = NULL;
        primaryKeys = NULL;
        inTypes = NULL;
        inValues = NULL;
        columnCount = 0;
        pkCount = 0;
        limit = 0;
        orderBy = -1;
        count_flag = false;
        in_flag = false;
        ready_for_execution = true;
        node* andOrIn;
        CURRENT_PARENT_INST = current_token.code;
        row = (Row*) malloc(sizeof(Row));
        row->state = 1;
        row->cells = NULL;
        switch(current_token.code)
        {
        case CREATE_TOKEN   : _create_statement();  break;
        case USE_TOKEN      : _use_statement();     break;
        case USING_TOKEN    : _using_statement();   break;
        case ALTER_TOKEN    : _alter_statement();   break;
        case DROP_TOKEN     : _drop_statement();    break;
        case INSERT_TOKEN   : _insert_statement();  break;
        case UPDATE_TOKEN   : _update_statement();  break;
        case SELECT_TOKEN   : _select_statement();  break;
        case BEGIN_TOKEN    : _batch_statement();   break;
        case DELETE_TOKEN   : _delete_statement();  break;
        case DESCRIBE_TOKEN   : _describe_statement();  break;
        case LIST_TOKEN     : _list_statement();    break;
        //REVOKE_TOKEN   : _revoke_statement();  break;
        //case GRANT_TOKEN    : _grant_statement();   break;
        default             :  print_error_token(current_token.code);
        }
        _test_symbol(PV_TOKEN);
        if(ready_for_execution == true) 
            interpret();
        //reset_states();
        sendResult();

    }
}


void _describe_statement()
{
    _test_symbol(DESCRIBE_TOKEN);
    CURRENT_CHILD_INST = current_token.code ;
    switch(current_token.code)
    {
    case KEYSPACE_TOKEN     : getToken(); _describe_keyspace_statement();         break;
    case TABLE_TOKEN        : getToken(); _describe_table_statement();            break;
    default : print_error_token(current_token.code); getToken();
    }
}


void _describe_keyspace_statement()
{
    _test_symbol(IDENTIFIER_TOKEN);
    strcpy(describe_keyspace, last_token.value);
}
void _describe_table_statement()
{   
    isTableName();
}


void _create_statement()
{
    _test_symbol(CREATE_TOKEN);
    CURRENT_CHILD_INST = current_token.code ;
    switch(current_token.code)
    {
    case KEYSPACE_TOKEN     : getToken(); _create_keyspace_statement();         break;
    case TABLE_TOKEN        : getToken(); _create_table_statement();            break;
    case COLUMNFAMILY_TOKEN : getToken(); _create_table_statement();            break;
    case INDEX_TOKEN        : getToken(); _create_index_statement();            break;
    case TYPE_TOKEN         : getToken(); _create_type_statement();             break;
    case USER_TOKEN         : getToken(); _create_user_statement();             break;
    case CUSTOM_TOKEN       : getToken();_test_symbol(INDEX_TOKEN); _create_index_statement(); break;
    //case MATERIALIZED_TOKEN : getToken(); _create_materialized_statement();     break;
    //case ROLE_TOKEN         : getToken(); _create_role_statement();             break;
    //case TRIGGER_TOKEN      : getToken(); _create_trigger_statement();          break;
    //case FUNCTION_TOKEN     : getToken(); _create_function_statement();         break;
    //case AGGREGATE_TOKEN    : getToken(); _create_aggregate_statement();        break;

    /*case OR_TOKEN           :
        getToken();
        _test_symbol(REPLACE_TOKEN);
        switch(current_token.code)
        {
        case FUNCTION_TOKEN  : getToken(); _create_function_statement();  break;
        case AGGREGATE_TOKEN : getToken(); _create_aggregate_statement(); break;
        default              : print_error_token(current_token.code); getToken();
        }
        break;
    */

    default : print_error_token(current_token.code); getToken();
    }
}

void _alter_statement()
{
    _test_symbol(ALTER_TOKEN);
    CURRENT_CHILD_INST = current_token.code;
    switch (current_token.code)
    {
    case KEYSPACE_TOKEN  :  getToken();      _alter_keyspace_statement();    break;
    case TABLE_TOKEN     :  getToken();      _alter_table_statement();       break;
    case TYPE_TOKEN      :  getToken();      _alter_type_statement();        break;
    case USER_TOKEN      :  getToken();      _alter_user_statement();        break;
    //case ROLE_TOKEN      :  getToken();      _alter_role_statement();        break;
    default:    print_error_token(current_token.code);  getToken();          break;
    }
}

void _drop_statement()
{
    _test_symbol(DROP_TOKEN);
    CURRENT_CHILD_INST = current_token.code;
    switch (current_token.code) {
    case KEYSPACE_TOKEN  :  getToken();      _drop_keyspace_statement();      break;
    case TABLE_TOKEN     :  getToken();      _drop_table_statement();         break;
    case INDEX_TOKEN     :  getToken();      _drop_index_statement();         break;
    case TYPE_TOKEN      :  getToken();      _drop_type_statement();          break;
    case USER_TOKEN      :  getToken();      _drop_user_statement();          break;
    //case ROLE_TOKEN      :  getToken();      _drop_role_statement();          break;
    ///case FUNCTION_TOKEN  :  getToken();      _drop_function_statement();      break;
    //case AGGREGATE_TOKEN :  getToken();      _drop_aggregate_statement();     break;
    //case TRIGGER_TOKEN   :  getToken();      _drop_trigger_statement();       break;
    //case MATERIALIZED_TOKEN:getToken();      _drop_materialized_statement();  break;
    default:  print_error_token(current_token.code); break;
    }
}

void _create_keyspace_statement()
{
    _test_symbol(IDENTIFIER_TOKEN);
    current_keyspace = malloc(100*sizeof(1));
    strcpy(current_keyspace, last_token.value);
    _if_not_exists();
    if( current_token.code == WITH_TOKEN){
        getToken();
        _options();
    }
}

void _create_table_statement()
{   
    _if_not_exists();
    isTableName();

    _test_symbol(PO_TOKEN);
    _column_defenition();
    while(current_token.code == VIR_TOKEN)
    {
        getToken();
        if(current_token.code == IDENTIFIER_TOKEN) _column_defenition();
    }
    _primary_key_ox_para();
    _test_symbol(PF_TOKEN);
    if(current_token.code == WITH_TOKEN)
    {
        getToken();
        _table_options();
    }
}


void _create_index_statement()
{
    _if_not_exists();
    if(current_token.code == IDENTIFIER_TOKEN) getToken();
    _test_symbol(ON_TOKEN);
    isTableName();
    _test_symbol(PO_TOKEN);
    _index_identifier();
    _test_symbol(PF_TOKEN);
    if(current_token.code == USING_TOKEN) _using_statement();
}



void _create_type_statement()
{
 _if_not_exists();
 isTableName();
 _test_symbol(PO_TOKEN);
 _test_symbol(IDENTIFIER_TOKEN);
 _cql_type();
 while(current_token.code == VIR_TOKEN )
 {
     getToken();
     _test_symbol(IDENTIFIER_TOKEN);
     _cql_type();
 }
  _test_symbol(PF_TOKEN);

}


void _using_statement()
{
    _test_symbol(USING_TOKEN);
    _test_symbol(STRING_TOKEN);
    if(current_token.code == WITH_TOKEN)
    _index_option();
}

void _alter_keyspace_statement()
{

    _test_symbol(IDENTIFIER_TOKEN);
    _test_symbol(WITH_TOKEN);
    _options();
}

void _alter_table_statement()
{
    isTableName();
    _alter_table_instruction();

}

void _alter_type_statement()
{
    isTableName();
    _alter_type_modification();
}

void _drop_keyspace_statement()
{
    _if_exist();
    column_name1 = malloc(100 * sizeof(char));
    strcpy(column_name1, current_token.value);
    _test_symbol(IDENTIFIER_TOKEN);
}

void _drop_table_statement()
{
    _if_exist();
    isTableName();
}

void _drop_index_statement()
{
    _if_exist();
    _test_symbol(IDENTIFIER_TOKEN);
}


void _drop_type_statement()
{
    _drop_table_statement();
}



void _truncate_statement()
{
    _test_symbol(TRUNCATE_TOKEN);
    _exist_aux();
    isTableName();
}

void _use_statement() {
    _test_symbol(USE_TOKEN);
    _test_symbol(IDENTIFIER_TOKEN);
    current_keyspace = malloc(100*sizeof(char));
    strcpy(current_keyspace, last_token.value);
}

void _select_statement() {
    _test_symbol(SELECT_TOKEN);
    if(current_token.code==DISTINCT_TOKEN) getToken();
    if(current_token.code==ETOILE_TOKEN){
        getToken();
    }
    else _select_clause();

    _test_symbol(FROM_TOKEN);
    isTableName();
    switch(current_token.code){
    case WHERE_TOKEN: getToken(); _where_clause(); break;
    case ORDER_TOKEN: getToken(); _test_symbol(BY_TOKEN); _order_by_clause(); break;
    case LIMIT_TOKEN: getToken(); _option_type(); break;
    case ALLOW_TOKEN: _test_symbol(FILTERING_TOKEN); break;
    //case PER_TOKEN  : _test_symbol(PARTITION_TOKEN); _test_symbol(LIMIT_TOKEN); _option_type(); break;
    //case GROUP_TOKEN: _test_symbol(BY_TOKEN); _group_by_clause(); break;
    }
}

void _insert_statement(void){
    _test_symbol(INSERT_TOKEN);
    _test_symbol(INTO_TOKEN);
    isTableName();
    _insert_statement_aux();
    _if_not_exists();
    _option_using();
}

void _delete_statement(void) {
    _test_symbol(DELETE_TOKEN);
    _option_selection();
    _test_symbol(FROM_TOKEN);
    isTableName();
    _option_using();
    _test_symbol(WHERE_TOKEN);
    _relation();
    while(current_token.code==AND_TOKEN){
        getToken();
        _relation();
    }
    _if_exist();

}

void _batch_statement(void){
    _test_symbol(BEGIN_TOKEN);
    _option_batch();
    _test_symbol(BATCH_TOKEN);
    _option_using();
    _modification_statement();
    while(current_token.code==PV_TOKEN){
        getToken();
        if(current_token.code==INSERT_TOKEN
                || current_token.code==UPDATE_TOKEN
                || current_token.code==DELETE_TOKEN) _modification_statement();
       else;
    }

    _test_symbol(APPLY_TOKEN);
    _test_symbol(BATCH_TOKEN);
}

void _update_statement(void){
    _test_symbol(UPDATE_TOKEN);
    isTableName();
    if(current_token.code==USING_TOKEN){
        _option_using();
    }
    _test_symbol(SET_TOKEN);
    _assignment();

    while(current_token.code==VIR_TOKEN){
        getToken();
        _assignment();
    }
    indexesWhere = NULL;
    opsWhere = NULL;
    valuesWhere = NULL;

    _test_symbol(WHERE_TOKEN);
    _relation();
    while(current_token.code==AND_TOKEN){
        getToken();
        _relation();
    }
    _if_exist();
}



void _test_symbol(int code)
{
    if(current_token.code == code  || (code == IDENTIFIER_TOKEN  &&  current_token.code >= 62 ) );
    else  print_error_token(current_token.code);
    getToken();
}

void isTableName()
{
    if(current_token.code == IDENTIFIER_TOKEN)
    {
        table_name = malloc(100*sizeof(1));
        strcpy(table_name,current_token.value);
    }
    else{
        print_error_token(current_token.code);
    }
    getToken();
}

void _alter_table_instruction()
{
    CURRENT_DESCENDANT_INST = current_token.code;
    switch (current_token.code)
    {
        case ALTER_TOKEN :
            getToken();
            _test_symbol(IDENTIFIER_TOKEN);
            _test_symbol(TYPE_TOKEN);
            _cql_type();
            break;
        case ADD_TOKEN :
            getToken();
            column_name1 = malloc(100*sizeof(char));
            strcpy(column_name1,current_token.value);
            _test_symbol(IDENTIFIER_TOKEN);
            _cql_type();
            while(current_token.code == VIR_TOKEN)
            {
                getToken();
                _test_symbol(IDENTIFIER_TOKEN);
                _cql_type();
            }
            break;
        case DROP_TOKEN :
            getToken();
            column_name1 = malloc(100*sizeof(char));
            strcpy(column_name1,current_token.value);
            _test_symbol(IDENTIFIER_TOKEN);
            while(current_token.code == IDENTIFIER_TOKEN)
                getToken();
            break;
        case RENAME_TOKEN :
            getToken();
            column_name1 = malloc(100*sizeof(char));
            strcpy(column_name1,current_token.value);
            _test_symbol(IDENTIFIER_TOKEN);
            _test_symbol(TO_TOKEN);
            column_name2 = malloc(100*sizeof(char));
            strcpy(column_name2,current_token.value);
            _test_symbol(IDENTIFIER_TOKEN);
            break;
        case WITH_TOKEN :
            getToken();
            _options();
            break;

        default:  print_error_token(current_token.code); break;
    }
}

void _alter_type_modification()
{
    switch (current_token.code) {
        case ALTER_TOKEN :
            getToken();
            _test_symbol(IDENTIFIER_TOKEN);
            _test_symbol(TYPE_TOKEN);
            _cql_type();
            break;

        case ADD_TOKEN :
            getToken();
            _test_symbol(IDENTIFIER_TOKEN);
            _cql_type();
            break;

        case RENAME_TOKEN :
            getToken();
            _test_symbol(IDENTIFIER_TOKEN);
            _test_symbol(TO_TOKEN);
            _test_symbol(IDENTIFIER_TOKEN);
            while(current_token.code == IDENTIFIER_TOKEN)
            {
                getToken();
                _test_symbol(TO_TOKEN);
                _test_symbol(IDENTIFIER_TOKEN);
            }
            break;
        default: print_error_token(current_token.code); break;
    }
}



void _if_exist()
{
    if ( current_token.code == IF_TOKEN )
    {
        getToken();
        _test_symbol(EXISTS_TOKEN);

    }
}



void _exist_aux()
{
    if( current_token.code == TABLE_TOKEN || current_token.code == COLUMNFAMILY_TOKEN )
        getToken();

}
void _cql_type()
{
    if(current_token.code==HEX_TOKEN
            || current_token.code==BLOB_TOKEN
            || current_token.code==BOOLEAN_TOKEN
            || current_token.code==FLOAT_TOKEN
            || current_token.code==INT_TOKEN
            || current_token.code==STRING_TOKEN
            || current_token.code==UUID_TOKEN){
        column_type1 = malloc(100*sizeof(char));
        strcpy(column_type1,current_token.value);
        pushToList(&columnTypes, current_token.value, sizeofString(current_token.value)+1);  
        getToken();
    }
    else if(current_token.code==MAP_TOKEN){
        getToken();
        _test_symbol(INF_TOKEN);
        _cql_type();
        _test_symbol(VIR_TOKEN);
        _cql_type();
        _test_symbol(SUP_TOKEN);
    }
    else if(current_token.code == SET_TOKEN || current_token.code==LIST_TOKEN){
        getToken();
        _test_symbol(INF_TOKEN);
        _cql_type();
        _test_symbol(SUP_TOKEN);
    }
    else if(current_token.code==IDENTIFIER_TOKEN) {
        no_error = false;
        print_error_token(current_token.code);
        getToken();
        if(current_token.code==P_TOKEN){
            getToken();
            _test_symbol(IDENTIFIER_TOKEN);
        }
        else;
    }
    else if(current_token.code==STRING_TOKEN){
        getToken();
    }
    else {
        print_error_token(current_token.code);
        getToken();
    }
}

void _column_defenition()
{
    char* col_name;
    last_token.code = current_token.code ;
    _test_symbol(IDENTIFIER_TOKEN);
    col_name = (char*) malloc(strlen(last_token.value)*sizeof(char));
    strcpy(col_name, last_token.value);

    _cql_type(); 
    if(current_token.code == STATIC_TOKEN) getToken();
    if(current_token.code == PRIMARY_TOKEN)
    {
        getToken();
        if(current_token.code == KEY_TOKEN) {
            int c = getColumnIndex1(columnNames,col_name);
            pushToList(&primaryKeys, &c, sizeof(int));
            pkCount++;
            getToken();
        }
        else print_error_token(current_token.code);
    }

    int result = search_column(col_name);
    if( result == COLUMN_NOT_FOUND) {
        pushToList(&columnNames, col_name, sizeofString(col_name)+1);
        columnCount++;
    }else {
        char*result = malloc(250);
        sprintf(result,"columng name(%s) used more than once\n",col_name);
        pushResult(result,sizeofString(result)+1);
        no_error = false;
        ready_for_execution = false;
    }

}

void _primary_key_ox_para()
{
    if(current_token.code == PRIMARY_TOKEN)
    {
        getToken();
        if(current_token.code == KEY_TOKEN)
        {
            getToken();
            _test_symbol(PO_TOKEN);
            _primary_key_def();
            _test_symbol(PF_TOKEN);
        }
        else print_error_token(current_token.code);
    }

}

void _primary_key_def()
{   
    _partition_key();
    _clustring_columns();

}

void _partition_key()
{   
    int c;
    int result;
    switch(current_token.code)
    {
    case IDENTIFIER_TOKEN :
        result = search_column(current_token.value);
        if(result == COLUMN_ALREADY_EXISTS){
            int c = getColumnIndex1(columnNames,current_token.value);
            pushToList(&primaryKeys, &c, sizeof(int));
            pkCount++;
        }else{
            c = -1;
            pushToList(&primaryKeys, &c, sizeof(int));
            pkCount++;
        }
        getToken(); 
        break;
    case PO_TOKEN : 
        getToken();
        _test_symbol(IDENTIFIER_TOKEN);
        result = search_column(last_token.value);
        if(result == COLUMN_ALREADY_EXISTS){
            c = getColumnIndex1(columnNames,last_token.value);
            pushToList(&primaryKeys, &c, sizeof(int));
            pkCount++;
        }else{
            c = -1;
            pushToList(&primaryKeys, &c, sizeof(int));
            pkCount++;
        }
        while(current_token.code == VIR_TOKEN)
        {
            getToken();
            _test_symbol(IDENTIFIER_TOKEN);
            result = search_column(last_token.value);
            if(result == COLUMN_ALREADY_EXISTS){
                c = getColumnIndex1(columnNames,last_token.value);
                pushToList(&primaryKeys, &c, sizeof(int));
                pkCount++;
            }else{
                c = -1;
                pushToList(&primaryKeys, &c, sizeof(int));
                pkCount++;
            }
        }
        _test_symbol(PF_TOKEN);
        break;
    default : print_error_token(current_token.code);
    }
}

void _clustring_columns()
{
    int result,c;
    if(current_token.code == PF_TOKEN);
    else
    {
        _test_symbol(VIR_TOKEN);
        _test_symbol(IDENTIFIER_TOKEN);
        result = search_column(last_token.value);
        if(result == COLUMN_ALREADY_EXISTS){
            c = getColumnIndex1(columnNames,last_token.value);
            pushToList(&primaryKeys, &c, sizeof(int));
            pkCount++;
        }else{
            c = -1;
            pushToList(&primaryKeys, &c, sizeof(int));
            pkCount++;
        }
        while(current_token.code == VIR_TOKEN)
        {
            getToken();
            _test_symbol(IDENTIFIER_TOKEN);
           result = search_column(last_token.value);
            if(result == COLUMN_ALREADY_EXISTS){
                c = getColumnIndex1(columnNames,last_token.value);
                pushToList(&primaryKeys, &c, sizeof(int));
                pkCount++;
            }else{
                c = -1;
                pushToList(&primaryKeys, &c, sizeof(int));
                pkCount++;
            }
        }
    }
}

void _table_options()
{
    switch(current_token.code)
    {
    case COMPACT_TOKEN      : getToken();
        _test_symbol(STORAGE_TOKEN);
        _table_options_ox();
        break;
    case CLUSTERING_TOKEN   : getToken();
        _test_symbol(ORDER_TOKEN);
        _test_symbol(BY_TOKEN);
        _test_symbol(PO_TOKEN);
        _clustering_oredr();
        _test_symbol(PF_TOKEN);
        _table_options_ox();
        break;
    case IDENTIFIER_TOKEN   : _options(); break;
    default : print_error_token(current_token.code);
    }
}

void _table_options_ox()
{
    if(current_token.code == COMPACT_TOKEN
            || current_token.code == CLUSTERING_TOKEN
            || current_token.code == IDENTIFIER_TOKEN) _table_options();
}

void _clustering_oredr()
{
    _test_symbol(IDENTIFIER_TOKEN);
    _asc_desc();
    while(current_token.code == VIR_TOKEN)
    {
        getToken();
        _test_symbol(IDENTIFIER_TOKEN);
        _asc_desc();
    }
}

void _asc_desc()
{
    switch(current_token.code)
    {
    case ASC_TOKEN  : getToken(); break;
    case DESC_TOKEN : getToken(); break;
    default : print_error_token(current_token.code); getToken();
    }
}

void _index_identifier()
{

    if(current_token.code == IDENTIFIER_TOKEN)
        getToken();
    else if ( current_token.code == KEYS_TOKEN 
        || current_token.code == VALUES_TOKEN 
        || current_token.code == ENTRIES_TOKEN 
        || current_token.code == FULL_TOKEN)
    {
        getToken();
        _test_symbol(PO_TOKEN);
        _test_symbol(IDENTIFIER_TOKEN);
        _test_symbol(PF_TOKEN);
    }
    else{
        //print_error_token(current_token.code);
    }
}

void _index_option()
{
    _test_symbol(WITH_TOKEN);
    _test_symbol(OPTIONS_TOKEN);
    _test_symbol(EG_TOKEN);
    _test_symbol(ACOLADO_TOKEN);
    _map_literal();
}



void _options()
{
    _option();
    while(current_token.code == AND_TOKEN)
    {
        getToken();
        _option();
    }
}

void _option()
{
    _test_symbol(IDENTIFIER_TOKEN);
    _test_symbol(EG_TOKEN);
    switch(current_token.code)
    {
    case IDENTIFIER_TOKEN   : getToken(); break;
    case ACOLADO_TOKEN      : getToken(); _map_literal(); break;
    default                 : _constant();
    }
}

void _constant()
{
    if(strcasecmp(lastOp,"IN") != 0){
        pushToList(&valuesWhere, current_token.value,sizeofString(current_token.value)+1);
        pushToList(&TypesWhere, &current_token.code,sizeof(int));
    }
    switch(current_token.code)
    {
    case STRING_TOKEN       : getToken(); break;
    case INT_TOKEN          : getToken(); break;
    case FLOAT_TOKEN        : getToken(); break;
    case BOOLEAN_TOKEN      : getToken(); break;
    case UUID_TOKEN         : getToken(); break;
    case BLOB_TOKEN         : getToken(); break;
    //case NULL_TOKEN         : getToken(); break;
    default                 : print_error_token(current_token.code);
    }
}

void _map_literal()
{
    _term();

    _test_symbol(DEUXP_TOKEN);
    _term();
    while(current_token.code == VIR_TOKEN)
    {
        getToken();
        _term();
        while(current_token.code == DEUXP_TOKEN)
        {
            getToken();
            _term();
        }
    }
    _test_symbol(ACOLADF_TOKEN);
}

void _term()
{
    switch(current_token.code)
    {
    case ACOLADO_TOKEN      : getToken(); _aux_1();  _test_symbol(ACOLADF_TOKEN); break;
    case CROCHO_TOKEN       : getToken(); _aux_2();  _test_symbol(CROCHF_TOKEN);  break;
    case PO_TOKEN           : getToken(); _aux_3();                               break;
    case IDENTIFIER_TOKEN   : getToken();
        _test_symbol(PO_TOKEN);
        _aux_4();
        _test_symbol(PF_TOKEN);
        break;
    case PI_TOKEN           : getToken(); break;
    case DEUXP_TOKEN        : getToken();
        _test_symbol(IDENTIFIER_TOKEN);
        break;
    default                 : _constant();
    }
}

void _opt_data()
{
    switch (current_token.code)
    {
    case ACOLADO_TOKEN  : getToken(); _aux_1(); _test_symbol(ACOLADF_TOKEN);    break;
    case CROCHO_TOKEN   : getToken(); _aux_2(); _test_symbol(CROCHF_TOKEN);     break;
    case PO_TOKEN       : getToken(); _aux_3();                                 break;
    case PI_TOKEN       : getToken();                                           break;
    case DEUXP_TOKEN    : getToken(); _test_symbol(IDENTIFIER_TOKEN);           break;
    default             : _constant();                                         break;
    }
}

void _aux_1()
{

    if(current_token.code == ACOLADF_TOKEN);
    else {
        if(current_token.code == IDENTIFIER_TOKEN)
        {
            getToken();
            _aux_1_2();

        }
        else {
            _opt_data();
            _aux_1_1();
        }
    }
}

void _aux_2()
{
    if(current_token.code == CROCHF_TOKEN);
    else {
        _term();
        while(current_token.code == VIR_TOKEN) {
            getToken();
            _term();
        }
        _test_symbol(PF_TOKEN);
    }
}

void _aux_3()
{
    if(current_token.code == ACOLADO_TOKEN
            || current_token.code == CROCHO_TOKEN
            || current_token.code == PO_TOKEN
            || current_token.code == IDENTIFIER_TOKEN
            || current_token.code == PI_TOKEN
            || current_token.code == DEUXP_TOKEN
            || current_token.code == STRING_TOKEN
            || current_token.code == INT_TOKEN
            || current_token.code == FLOAT_TOKEN
            || current_token.code == BOOLEAN_TOKEN
            || current_token.code == UUID_TOKEN
            || current_token.code == BLOB_TOKEN)
    {
        pushToList(&inTypes,&current_token.code,sizeof(int));
        pushToList(&inValues,current_token.value,sizeofString(current_token.value)+1);
        _term();
        while(current_token.code == VIR_TOKEN) {
            getToken();
            pushToList(&inTypes,&current_token.code,sizeof(int));
            pushToList(&inValues,current_token.value,sizeofString(current_token.value)+1);
            _term();
        }
        _test_symbol(PF_TOKEN);
    }
    else {
        _cql_type();
        _test_symbol(PF_TOKEN);
        _term();
    }
}

void _aux_4()
{
    if(current_token.code == PF_TOKEN);
    else {
        _term();
        while(current_token.code == VIR_TOKEN) {
            getToken();
            _term();
        }
    }
}

void _aux_1_1()
{
    if(current_token.code == DEUXP_TOKEN) {
        getToken();
        _term();
        while(current_token.code == VIR_TOKEN)
        {
            getToken();
            _term();
            _test_symbol(DEUXP_TOKEN);
            _term();
        }
    }
    else if(current_token.code == VIR_TOKEN) {
        while(current_token.code == VIR_TOKEN) {
            getToken();
            _term();
        }
    }
    else {
        //print_error_token(current_token.code);
        getToken();
    }
}

void _aux_1_2()
{
    switch(current_token.code)
    {
    case DEUXP_TOKEN    : getToken();
        _term();
        while(current_token.code == VIR_TOKEN)
        {
            getToken();
            _test_symbol(IDENTIFIER_TOKEN);
            _test_symbol(DEUXP_TOKEN);
            _term();
        }

        break;
    case PO_TOKEN       : getToken();
        _aux_4();
        _test_symbol(PF_TOKEN);
        break;
    default : ;//print_error_token(current_token.code);

    }
}

void _if_not_exists()
{
    if(current_token.code == IF_TOKEN)
    {
        if_exist = true;
        getToken();
        if(current_token.code == NOT_TOKEN)
        {
            getToken();
        }
        _test_symbol(EXISTS_TOKEN);
    }
}

void _tuple_literal(void){
    _test_symbol(PO_TOKEN);
    pushToList(&row->cells, initCell(current_token.value),sizeof(Cell));
    pushToList(&types,&current_token.code,sizeof(int));
    _term();

    while(current_token.code==VIR_TOKEN){
        getToken();
        pushToList(&row->cells, initCell(current_token.value),sizeof(Cell));
        pushToList(&types,&current_token.code,sizeof(int));
        _term();
    }
    _test_symbol(PF_TOKEN);
}

void _insert_statement_aux(void){
    if(current_token.code==PO_TOKEN){
        getToken();
        pushToList(&columnNames,current_token.value,sizeofString(current_token.value)+1);
        _test_symbol(IDENTIFIER_TOKEN);
        while(current_token.code==VIR_TOKEN){
            getToken();
            pushToList(&columnNames,current_token.value,sizeofString(current_token.value)+1);
            _test_symbol(IDENTIFIER_TOKEN);
        }
        _test_symbol(PF_TOKEN);
        _test_symbol(VALUES_TOKEN);
        _tuple_literal();
    }
    else if(current_token.code==VALUES_TOKEN){
        getToken();
        _tuple_literal();
    }
    else ;//print_error_token(current_token.code);
}

void _option_default(void){
    if(current_token.code==DEFAULT_TOKEN){
        getToken();
        _default_aux();
    }
    else ;//getToken();
}

void _default_aux(void){
    if(current_token.code==UNSET_TOKEN){
        getToken();
    }
    else ;//print_error_token(current_token.code); getToken();
}

void _option_selection(void){
    if(current_token.code==IDENTIFIER_TOKEN){
        getToken();
        if(current_token.code==CROCHO_TOKEN){
            getToken();
            _term();
        }
        else if(current_token.code==P_TOKEN){
            getToken();
            _test_symbol(IDENTIFIER_TOKEN);
        }
        else;
    }
    else;
}


void _modification_statement(void){

    if(current_token.code==INSERT_TOKEN) _insert_statement();
    else if(current_token.code==UPDATE_TOKEN) _update_statement();
    else if(current_token.code==DELETE_TOKEN) _delete_statement();
    else {

        getToken();
    }

}

void _option_batch(void){
    if(current_token.code==UNLOGGED_TOKEN) getToken();
    else if(current_token.code==COUNTER_TOKEN) getToken();
    else;
}

void _select_clause(void) {
    _selector();
    if(current_token.code==AS_TOKEN) {
        getToken();
        pushToList(&asNames,current_token.value,sizeofString(current_token.value)+1);
        _test_symbol(IDENTIFIER_TOKEN);
    }else {
        if(last_token.code == IDENTIFIER_TOKEN)
            pushToList(&asNames,last_token.value,sizeofString(last_token.value)+1);
    }
    while(current_token.code == VIR_TOKEN){
        getToken();
        _selector();
        if(current_token.code==AS_TOKEN) {
            getToken();
            pushToList(&asNames,current_token.value,sizeofString(current_token.value)+1);
            _test_symbol(IDENTIFIER_TOKEN);
        }else {
            if(last_token.code == IDENTIFIER_TOKEN)
                pushToList(&asNames,last_token.value,sizeofString(last_token.value)+1);
            }
    }
}


void _selector(void){
    switch(current_token.code){
    case IDENTIFIER_TOKEN: {
        pushToList(&indexes,current_token.value,sizeofString(current_token.value)+1);
        getToken();
        _selector_aux();
        break;
    }
    case TOKEN_TOKEN:{
        getToken();
        _test_symbol(PO_TOKEN);
        _option_selector();
        _test_symbol(PF_TOKEN);
        break;
    }
    case COUNT_TOKEN: {
        getToken();
        count_flag = true;
        _test_symbol(PO_TOKEN);
        _test_symbol(ETOILE_TOKEN);
        _test_symbol(PF_TOKEN);
        break;
    }
    case ACOLADO_TOKEN: {
        getToken();
        _aux_1();
        _test_symbol(ACOLADF_TOKEN);
        break;
    }
    case CROCHO_TOKEN: {
        getToken();
        _aux_2();
        _test_symbol(CROCHF_TOKEN);
        break;
    }
    case PO_TOKEN:{
        getToken();
        _aux_3();
        break;
    }
    case PI_TOKEN:{
        getToken();
        break;
    }
    case DEUXP_TOKEN: {
        getToken();
        _test_symbol(IDENTIFIER_TOKEN);
        break;
    }
    default : _constant();
    }
}

void _selector_aux(void){
    if(current_token.code == PO_TOKEN) {
        getToken();
        _aux();
    }
    else ;
}

void _option_selector(void){

    _selector();
    while(current_token.code==VIR_TOKEN){
        getToken();
        _selector();
    }
}

void _aux(void){
    ;
}

void _where_clause(void){
    _relation();
    while(current_token.code==AND_TOKEN){
        getToken();
        _relation();
    }

    switch(current_token.code){
    case ORDER_TOKEN: getToken(); _test_symbol(BY_TOKEN); _order_by_clause(); break;
    case LIMIT_TOKEN: getToken(); _option_type(); break;
    case ALLOW_TOKEN: getToken(); _test_symbol(FILTERING_TOKEN); break;
    //case PER_TOKEN  : getToken(); _test_symbol(PARTITION_TOKEN); _test_symbol(LIMIT_TOKEN); _option_type(); break;
    //case GROUP_TOKEN: getToken(); _test_symbol(BY_TOKEN); _group_by_clause(); break;
    }
}



void _order_by_clause(void){
    column_name1 = malloc(100*sizeof(char));
    strcpy(column_name1,current_token.value);
    _test_symbol(IDENTIFIER_TOKEN);
    if(current_token.code==ASC_TOKEN || current_token.code==DESC_TOKEN){
        orderBy = current_token.code;
        getToken();
    }else  orderBy = ASC_TOKEN;
    
    /*while(current_token.code==VIR_TOKEN){
        _test_symbol(IDENTIFIER_TOKEN);
        if(current_token.code==ASC_TOKEN || current_token.code==DESC_TOKEN);
        else;
        getToken();
    }*/
    switch(current_token.code){
    case LIMIT_TOKEN: getToken(); _option_type(); break;
    case ALLOW_TOKEN: _test_symbol(FILTERING_TOKEN); break;
    //case PER_TOKEN  : getToken();_test_symbol(PARTITION_TOKEN); _test_symbol(LIMIT_TOKEN); _option_type(); break;
    }
}

void _option_type(void){
    if(current_token.code==INT_TOKEN){
        limit = atoi(current_token.value);
        getToken();
        switch(current_token.code){
        case LIMIT_TOKEN: getToken(); _option_type(); break;
        case ALLOW_TOKEN: getToken(); _test_symbol(FILTERING_TOKEN); break;
        }
    }
    else if(current_token.code==PI_TOKEN || current_token.code==DEUXP_TOKEN){
        getToken();
        _test_symbol(IDENTIFIER_TOKEN);
        switch(current_token.code){
        case LIMIT_TOKEN: getToken(); _option_type(); break;
        case ALLOW_TOKEN: getToken();_test_symbol(FILTERING_TOKEN); break;
        }
    }
    else {
        print_error_token(current_token.code);
        getToken();
    }
}

void _relation(void){
    if(current_token.code==IDENTIFIER_TOKEN){
        pushToList(&indexesWhere, current_token.value,sizeofString(current_token.value)+1);
        getToken();
        _operator();
        _term();
    }
    else if(current_token.code==PO_TOKEN){
        getToken();
        _test_symbol(IDENTIFIER_TOKEN);
        while(current_token.code==VIR_TOKEN){
            getToken();
            _test_symbol(IDENTIFIER_TOKEN);
        }
        _test_symbol(PF_TOKEN);

    }
    else if(current_token.code==TOKEN_TOKEN){
        getToken();
        _test_symbol(PO_TOKEN);
        _aux_4();
        while (current_token.code==VIR_TOKEN) {
            getToken();
            _term();
        }
        _test_symbol(PF_TOKEN);

        _operator();

        _term();
    }
    else print_error_token(current_token.code);

}

void _operator(void){
    if(current_token.code==EG_TOKEN
            || current_token.code==INF_TOKEN
            || current_token.code==INFEG_TOKEN
            || current_token.code==SUP_TOKEN
            || current_token.code==SUPEG_TOKEN
            || current_token.code==DIFF_TOKEN
            || current_token.code==IN_TOKEN)
    {
        if(current_token.code==IN_TOKEN){
            printf("%s %d\n",current_token.value,current_token.code);
            pushToList(&valuesWhere, current_token.value,sizeofString(current_token.value)+1);
            pushToList(&TypesWhere, &current_token.code,sizeof(int)); 
        }
        strcpy(lastOp,current_token.value);
        pushToList(&opsWhere, current_token.value,sizeofString(current_token.value)+1);
        getToken();

    }
    else if(current_token.code==CONTAINS_TOKEN){
        getToken();
        _test_symbol(KEY_TOKEN);
    }
    else print_error_token(current_token.code); 
}

void _option_using(void){
    if(current_token.code==USING_TOKEN){

        getToken();
        _update_parametre();
        while(current_token.code==VIR_TOKEN){
            getToken();
            _update_parametre();
        }
    }
    else;
}

void _update_parametre(void){

    _update_parametre_aux();
    _option_type_aux();
}

void _update_parametre_aux(void){
    if(current_token.code==TIMESTAMP_TOKEN || current_token.code==TTL_TOKEN){
        getToken();
    }
    else print_error_token(current_token.code); 

}

void _option_type_aux(void){

    if(current_token.code==INT_TOKEN){

        getToken();
    }
    else if(current_token.code==PI_TOKEN || current_token.code==DEUXP_TOKEN){
        getToken();
        _test_symbol(IDENTIFIER_TOKEN);
    }
    else print_error_token(current_token.code);

}

void _assignment(void){
    pushToList(&indexes,current_token.value,sizeofString(current_token.value)+1);
    _test_symbol(IDENTIFIER_TOKEN);
    _assignment_aux();
}

void _assignment_aux(void){
    if(current_token.code==CROCHO_TOKEN){
        getToken();
        _term();
        _test_symbol(CROCHF_TOKEN);
        _test_symbol(EG_TOKEN);
        _term();
    }

    else if(current_token.code==P_TOKEN){
        getToken();
        _test_symbol(IDENTIFIER_TOKEN);
        _test_symbol(EG_TOKEN);
        _term();
    }
    else if(current_token.code==EG_TOKEN){
        getToken();
        pushToList(&newValues,current_token.value,sizeofString(current_token.value)+1);
        pushToList(&types,&current_token.code,sizeof(int));
        _term();
    }
    else print_error_token(current_token.code); 


}

void _aff_column_name(void){
    if(current_token.code==IDENTIFIER_TOKEN){
        getToken();
        if(current_token.code==PLUS_TOKEN || current_token.code==MOINS_TOKEN);
        else print_error_token(current_token.code);
        getToken();
        _term();
    }
    else if(current_token.code==CROCHO_TOKEN){
        getToken();
        if(current_token.code!=CROCHF_TOKEN){
            _option_list_literal();
        }
        _test_symbol(CROCHF_TOKEN);
        _test_symbol(PLUS_TOKEN);
        _test_symbol(IDENTIFIER_TOKEN);
    }

    else _term();




}

void _option_list_literal(void){
    _term();
    while(current_token.code==VIR_TOKEN){
        getToken();
        _term();
    }
}

void _option_if(void){
    if(current_token.code==IF_TOKEN){
        getToken();
        _option_if_aux();
    }
    else;
}

void _option_if_aux(void){
    if(current_token.code==EXISTS_TOKEN) {
        getToken();
    }
    else if(current_token.code==IDENTIFIER_TOKEN){
        _condition();
        while(current_token.code==AND_TOKEN){
            getToken();
            _condition();
        }
    }
    else print_error_token(current_token.code);
}

void _condition(void){
    _test_symbol(IDENTIFIER_TOKEN);
    if(current_token.code==CROCHO_TOKEN){
        getToken();
        _term();
        _test_symbol(CROCHF_TOKEN);
        _operator();
        _term();
    }
    else if(current_token.code==P_TOKEN){
        getToken();
        _test_symbol(IDENTIFIER_TOKEN);
        _operator();
        _term();
    }
    else {
        _operator();
        _term();
    }

}
void _create_user_statement()
{
    pushToList(&row->cells, initCell(current_token.value),sizeof(Cell));
  _test_symbol(IDENTIFIER_TOKEN);
  if(current_token.code == WITH_TOKEN)
  {
      getToken();
      _test_symbol(PASSWORD_TOKEN);
        pushToList(&row->cells, initCell(current_token.value),sizeof(Cell));
      _test_symbol(STRING_TOKEN);
  }

}

void _alter_user_statement()
{   
    pushToList(&row->cells, initCell(current_token.value),sizeof(Cell));
    _test_symbol(IDENTIFIER_TOKEN);
    _test_symbol(WITH_TOKEN);
    _test_symbol(PASSWORD_TOKEN);
    pushToList(&row->cells, initCell(current_token.value),sizeof(Cell));
    _test_symbol(STRING_TOKEN);

}

void _drop_user_statement()
{
    strcpy(userName,current_token.value);
    _test_symbol(IDENTIFIER_TOKEN);
}

void _list_statement(){
    _test_symbol(LIST_TOKEN);
    
    if(current_token.code==USERS_TOKEN){
        _list_users_statement();
    }
    
}


void _list_users_statement(){
    _test_symbol(USERS_TOKEN);
}



// UNUSED GRAMMER.
//grant options

/*void _permissions(){
        if(current_token.code==ALL_TOKEN){
            getToken();
            if(current_token.code==PERMISSIONS_TOKEN){
                getToken();
            }
            else
            {getToken();}
        }
        else
            {_permission();
            if(current_token.code==PERMISSION_TOKEN)
                getToken();



        }
}*/

/*void _ressource(){
    switch(current_token.code){
        case ALL_TOKEN       :               getToken(); _all_statement(); break;
        case KEYSPACE_TOKEN  :               getToken(); _test_symbol(IDENTIFIER_TOKEN);break;
        case TABLE_TOKEN     :               getToken();
                                             if(current_token.code==IDENTIFIER_TOKEN || current_token.code==TABLE_FUNCTION_NAME_TOKEN) getToken();
                                             else{
                                                    print_error_token(current_token.code);
                                                    getToken();
                                                 }
                                            break;

        case IDENTIFIER_TOKEN :             getToken();break;
        case TABLE_FUNCTION_NAME_TOKEN :    getToken();break;
        case ROLE_TOKEN     :               getToken();
                                            if(current_token.code==IDENTIFIER_TOKEN|| current_token.code==STRING_TOKEN)getToken(); 
                                            break;
        case FUNCTION_TOKEN :               getToken();
                                            _test_symbol(TABLE_FUNCTION_NAME_TOKEN);
                                            _test_symbol(PO_TOKEN);
                                            _function_statement();
                                            _test_symbol(PF_TOKEN); break;
        case MBEAN_TOKEN    :               _test_symbol(STRING_TOKEN);break;

        case MBEANS_TOKEN   :               _test_symbol(STRING_TOKEN);break;
        default             :               printf("\nressource_ERROR\n");getToken(); break;


}}*/
/*void _permission(){
    switch(current_token.code){
        case CREATE_TOKEN   :getToken();break;
        case ALTER_TOKEN    :getToken();break;
        case DROP_TOKEN     :getToken();break;
        case SELECT_TOKEN   :getToken();break;
        case MODIFY_TOKEN   :getToken();break;
        case AUTHORIZE_TOKEN:getToken();break;
        case DESCRIBE_TOKEN :getToken();break;
        case EXECUTE_TOKEN  :getToken();break;
        default             :printf("\npermission_ERROR\n");getToken(); break;
    }
}*/

/*void _all_statement(){
    switch(current_token.code){
        case KEYSPACES_TOKEN :getToken();break;
        case ROLES_TOKEN     :getToken();break;
        case FUNCTIONS_TOKEN :getToken(); _all_functions_statement();break;
        case MBEANS_TOKEN    :getToken();break;
        default             :printf("\nall statement_ERROR\n");getToken(); break;

    }
}*/

/*void _all_functions_statement(){
    if(current_token.code==IN_TOKEN){
        getToken();
        _test_symbol(KEYSPACE_TOKEN);
        _test_symbol(IDENTIFIER_TOKEN);
    }
    else{
        getToken();
    }
}*/


/*void _function_statement(){

    if(current_token.code!=PF_TOKEN){
        _cql_type();
        while(current_token.code==VIR_TOKEN){
            getToken();
            _cql_type();
        }}
    else getToken();
}*/

//list options

/*void _of_role_statement(){
    if(current_token.code==OF_TOKEN){
        getToken();
        if(current_token.code==IDENTIFIER_TOKEN|| current_token.code==STRING_TOKEN){getToken();}
        else {print_error_token(current_token.code);getToken();}


}

}*/

/*void _no_recursive_statement(){
    if(current_token.code==NORECURSIVE_TOKEN)
        getToken();
}*/

/*void _on_ressource_statement(){
    if(current_token.code==ON_TOKEN){
        getToken();
        _ressource();
}

}*/


/*void _of_role_name_statement(){
    if(current_token.code==OF_TOKEN){
        getToken();
        if(current_token.code==IDENTIFIER_TOKEN || current_token.code==STRING_TOKEN)getToken();
        else {print_error_token(current_token.code);getToken();}
        _no_recursive_statement();
    }
}*/
/*void _create_materialized_statement()
{
  _test_symbol(VIEW_TOKEN);
  _if_not_exists();
  _test_symbol(IDENTIFIER_TOKEN);
  _test_symbol(AS_TOKEN);
  _select_statement();
  _test_symbol(PRIMARY_TOKEN);
  _test_symbol(KEY_TOKEN);
  _test_symbol(PO_TOKEN);
  _primary_key_def();
  _test_symbol(WITH_TOKEN);
  _table_options();
}*/

/*void _create_role_statement()
{
    _if_not_exists();
    _test_symbol(IDENTIFIER_TOKEN);
    if(current_token.code == WITH_TOKEN)
    {
         getToken();
        _role_options();
    }
}*/



/*void _create_trigger_statement()
{

    _if_not_exists();
    _test_symbol(IDENTIFIER_TOKEN);
    _test_symbol(ON_TOKEN);
    isTableName();
    _test_symbol(USING_TOKEN);
    _test_symbol(STRING_TOKEN);
}*/

/*void _create_function_statement()
{
  _if_not_exists();
  isTableName();
  _test_symbol(PO_TOKEN);
  _arg_dec();
  _test_symbol(PF_TOKEN);
  if(current_token.code == CALLED_TOKEN)
      getToken();
  else if ( current_token.code == RETURNS_TOKEN )
  {
      getToken();
      _test_symbol(NULL_TOKEN);
  }

  _test_symbol(ON_TOKEN);
  _test_symbol(NULL_TOKEN);
  _test_symbol(INPUT_TOKEN);
  _test_symbol(RETURNS_TOKEN);
  _cql_type();
  _test_symbol(LANGUAGE_TOKEN);
  _test_symbol(IDENTIFIER_TOKEN);
  _test_symbol(AS_TOKEN);
  _test_symbol(STRING_TOKEN);

}*/

/*void _create_aggregate_statement() {

}*/
/*void _alter_role_statement()
{
    _test_symbol(IDENTIFIER_TOKEN);
    _test_symbol(WITH_TOKEN);
    _role_options();

}


/*void _drop_role_statement()
{
    _if_exist();
    _test_symbol(IDENTIFIER_TOKEN);
}*/

/*void _drop_materialized_statement()
{
    _test_symbol(VIEW_TOKEN);
    _if_exist();
    _test_symbol(IDENTIFIER_TOKEN);
}*/


/*void _drop_function_statement()
{
    _if_exist();
    isTableName();
    _arg_sin();
}*/

/*void _drop_aggregate_statement()
{
    _drop_function_statement();
}*/

/*void _drop_trigger_statement()
{
    _if_exist();
    isTableName();
    _test_symbol(ON_TOKEN);
    isTableName();
}*/
/*void _grant_statement(){
    _test_symbol(GRANT_TOKEN);
    if(current_token.code==IDENTIFIER_TOKEN || current_token.code==STRING_TOKEN)
        _grant_role_statement();
    else _grant_permission_statement();
}*/

/*void _grant_role_statement(){

    if(current_token.code==IDENTIFIER_TOKEN || current_token.code==STRING_TOKEN) getToken();
       else {print_error_token(current_token.code);getToken();}

       _test_symbol(TO_TOKEN);

       if(current_token.code==IDENTIFIER_TOKEN || current_token.code==STRING_TOKEN)getToken();
       else {print_error_token(current_token.code);getToken();}
}*/

/*void _grant_permission_statement(){
    _permissions();
    _test_symbol(ON_TOKEN);
    _ressource();
    _test_symbol(TO_TOKEN);

    if(current_token.code==IDENTIFIER_TOKEN || current_token.code==STRING_TOKEN);
    else print_error_token(current_token.code);
    getToken();

}*/

/*void _revoke_statement(){
    _test_symbol(REVOKE_TOKEN);
    if(current_token.code==IDENTIFIER_TOKEN || current_token.code==STRING_TOKEN)
        _revoke_role_statement();
    else _revoke_permission_statement();

}*/

/*void _revoke_role_statement(){
    if(current_token.code==IDENTIFIER_TOKEN || current_token.code==STRING_TOKEN) getToken();
    else {print_error_token(current_token.code);getToken();}
    _test_symbol(FROM_TOKEN);
    if(current_token.code==IDENTIFIER_TOKEN || current_token.code==STRING_TOKEN)getToken();
    else {print_error_token(current_token.code);getToken();}
}*/

/*void _revoke_permission_statement(){
    _permissions();
    _test_symbol(ON_TOKEN);
    _ressource();
    _test_symbol(FROM_TOKEN);
    if(current_token.code==IDENTIFIER_TOKEN|| current_token.code==STRING_TOKEN)getToken();
    else {print_error_token(current_token.code);getToken();}

}*/



/*void _list_roles_statement(){
    _test_symbol(ROLES_TOKEN);
    _of_role_statement();
    _no_recursive_statement();
}*/


/*void _list_permissions_statement(){
    _permissions();
    _on_ressource_statement();
    _of_role_name_statement();
}*/
/*void _role_options()
{
    _role_option();
    while (current_token.code == AND_TOKEN )
    {
        getToken();
        _role_option();
    }
}*/

/*void _role_option()
{
    switch (current_token.code) {
    case PASSWORD_TOKEN  :     getToken();    _test_symbol(EG_TOKEN);    _test_symbol(STRING_TOKEN);                          break;
    case LOGIN_TOKEN     :     getToken();    _test_symbol(EG_TOKEN);    _test_symbol(BOOLEAN_VALUE_TOKEN);                   break;
    case SUPERUSER_TOKEN :     getToken();    _test_symbol(EG_TOKEN);    _test_symbol(BOOLEAN_VALUE_TOKEN);                   break;
    case OPTIONS_TOKEN   :     getToken();    _test_symbol(EG_TOKEN);    _test_symbol(ACOLADO_TOKEN);   _map_literal();       break;
    default:   print_error_token(current_token.code);

    }
}*/
/*void _arg_sin()
{
    if( current_token.code == END_TOKEN );
    _cql_type();
    //avancer avant dans cql_type
    while(current_token.code == VIR_TOKEN )
    {
        getToken();
        _cql_type();
    }
}*/
/*void _arg_dec()
{
    _test_symbol(IDENTIFIER_TOKEN);
    _cql_type();
    while( current_token.code == VIR_TOKEN )
    {
        getToken();
        _test_symbol(IDENTIFIER_TOKEN);
        _cql_type();
    }
}*/
/*void _group_by_clause(void){
    _test_symbol(IDENTIFIER_TOKEN);
    while(current_token.code==VIR_TOKEN){
        _test_symbol(IDENTIFIER_TOKEN);
    }
    switch(current_token.code){
    case ORDER_TOKEN: getToken();_test_symbol(BY_TOKEN); _order_by_clause(); break;
    case PER_TOKEN  : getToken();_test_symbol(PARTITION_TOKEN); _test_symbol(LIMIT_TOKEN); _option_type(); break;
    case LIMIT_TOKEN: getToken(); _option_type(); break;
    case ALLOW_TOKEN: _test_symbol(FILTERING_TOKEN); break;
    }
}*/