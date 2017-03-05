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

void _test_symbol(code_token_t);
void isTableName(void);
void _index_option(void);
void _index_identifier(void);
void _options(void);
void _option(void);
void _constant(void);
void _map_literal(void);
void _term(void);
void _aux_1(void);
void _aux_2(void);
void _aux_3(void);
void _aux_4(void);
void _aux_1_1(void);
void _aux_1_2(void);
void _opt_data(void);
void _if_not_exists(void);
void _cql_type(void);
void _column_defenition(void);
void _primary_key_ox_para(void);
void _primary_key_def(void);
void _partition_key(void);
void _clustring_columns(void);
void _table_options(void);
void _clustering_oredr(void);
void _table_options_ox(void);
void _asc_desc(void);
void _alter_table_instruction(void);
void _alter_type_modification(void);
void _role_options(void);
void _role_option(void);
void _if_exist(void);
void _arg_sin(void);
void _exist_aux(void);
void _arg_dec(void);
void _select_clause(void);
void _selector(void);
void _selector_aux(void);
void _where_clause(void);
void _group_clause(void);
void _order_by_clause(void);
void _option_type(void);
void _relation(void);
void _operator(void);

void _use_statement(void);
void _select_statement(void);
void _update_statement(void);
void _insert_statement(void);
void _delete_statement(void);
void _batch_statement(void);

void _option_using(void);
void _update_parametre(void);
void _assignment(void);
void _assignment_aux(void);
void _option_type_aux(void);
void _update_parametre_aux(void);
void _aff_column_name(void);
void _option_list_literal(void);
void _option_if(void);
void _option_if_aux(void);
void _condition(void);
void _tuple_literal(void);
void _insert_statement_aux(void);
void _option_default(void);
void _default_aux(void);
void _option_selection(void);
void _option_batch(void);
void _modification_statement(void);


void _permissions(void);
void _ressource(void);
void _permission(void);
void _all_statement(void);
void _all_functions_statement(void);
void _function_statement(void);
void _of_role_statement(void);
void _no_recursive_statement(void);
void _on_ressource_statement(void);
void _of_role_name_statement(void);

void _grant_statement(void);
void _grant_role_statement(void);
void _grant_permission_statement(void);

//revoke_statement
void _revoke_statement(void);
void _revoke_role_statement(void);
void _revoke_permission_statement(void);

//list_statement
void _list_statement(void);
void _list_roles_statement(void);
void _list_permissions_statement(void);
void _list_users_statement(void);

void _create_keyspace_statement(void);
void _create_table_statement(void);
void _create_index_statement(void);
void _create_materialized_statement(void);
void _create_role_statement(void);
void _create_user_statement(void);
void _create_type_statement(void);
void _create_trigger_statement(void);
void _create_function_statement(void);
void _create_aggregate_statement(void);

void _using_statement(void);

void _alter_keyspace_statement(void);
void _alter_table_statement(void);
void _alter_role_statement(void);
void _alter_user_statement(void);
void _alter_type_statement(void);

void _drop_keyspace_statement(void);
void _drop_table_statement(void);
void _drop_index_statement(void);
void _drop_role_statement(void);
void _drop_user_statement(void);
void _drop_function_statement(void);

void _drop_aggregate_statement(void);
void _drop_type_statement(void);
void _drop_trigger_statement(void);
void _drop_materialized_statement(void);

void _truncate_statement(void);