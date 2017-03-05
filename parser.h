#include "tokenizer.h"	
#include "execution.h"
extern Token * tokenList;
extern Token current_token;
extern Token last_token;
extern bool no_error;
extern int CURRENT_PARENT_INST;
extern int CURRENT_CHILD_INST;
extern int CURRENT_DESCENDANT_INST;
extern char *current_keyspace;
extern char *table_name;
extern char *column_name1;
extern char *column_name2;
extern char *column_type1;
extern bool if_exist;
extern Row *row;
extern node *indexes;
extern node *inValues;
extern node *inTypes;
extern node *newValues;
extern node *indexesWhere;
extern node *valuesWhere;
extern node *TypesWhere;
extern node *opsWhere;
extern int limit;
extern int orderBy;
extern int columnCount;
extern int pkCount;
extern node* columnNames;
extern node* columnTypes;
extern node* primaryKeys;
extern node* types;
extern node* asNames;
extern bool count_flag;
extern bool in_flag;
extern node* andOrIn;
extern char *userName;
extern char* describe_keyspace;

void getToken();

int search_column(char* colName);

void print_error(int code);

void parse();

void _program();

void _describe_statement();

void _describe_keyspace_statement();

void _describe_table_statement();

void _create_statement();

void _alter_statement();

void _drop_statement();

void _create_keyspace_statement();

void _create_table_statement();

void _create_index_statement();

void _create_type_statement();

void _using_statement();

void _alter_keyspace_statement();

void _alter_table_statement();

void _alter_type_statement();

void _drop_keyspace_statement();

void _drop_table_statement();

void _drop_index_statement();

void _drop_type_statement();

void _truncate_statement();

void _use_statement(); 

void _select_statement(); 

void _insert_statement(void);

void _delete_statement(void); 

void _batch_statement(void);

void _update_statement(void);

void _test_symbol(int code);

void isTableName();

void _alter_table_instruction();

void _alter_type_modification();

void _if_exist();

void _exist_aux();

void _cql_type();

void _column_defenition();

void _primary_key_ox_para();

void _primary_key_def();

void _partition_key();

void _clustring_columns();

void _table_options();

void _table_options_ox();

void _clustering_oredr();

void _asc_desc();

void _index_identifier();

void _index_option();

void _options();

void _option();

void _constant();

void _map_literal();

void _term();

void _opt_data();

void _aux_1();

void _aux_2();

void _aux_3();

void _aux_4();

void _aux_1_1();

void _aux_1_2();

void _if_not_exists();

void _tuple_literal(void);

void _insert_statement_aux(void);

void _option_default(void);

void _default_aux(void);

void _option_selection(void);

void _modification_statement(void);

void _option_batch(void);

void _select_clause(void); 

void _selector(void);

void _selector_aux(void);

void _option_selector(void);

void _aux(void);

void _where_clause(void);

void _order_by_clause(void);

void _option_type(void);

void _relation(void);

void _operator(void);

void _option_using(void);

void _update_parametre(void);

void _update_parametre_aux(void);

void _option_type_aux(void);

void _assignment(void);

void _assignment_aux(void);

void _aff_column_name(void);

void _option_list_literal(void);

void _option_if(void);

void _option_if_aux(void);

void _condition(void);

void _create_user_statement();

void _alter_user_statement();

void _drop_user_statement();

void _list_statement();

void _list_users_statement();