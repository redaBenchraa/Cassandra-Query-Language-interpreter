// TOKENIZER: token definitions

/*//////////////////////////////////////////////////////////////////////////////////////////////////////////*/
/* IMPORTANT: make sure elements in enum TOKEN_CODE & char[][] KEYWORDS have the same order! 		  		*/
/*  You can add, modify, delete 'reserved words' and 'operators', as long as there is no repetition.  		*/
/* 	You can modify Comments, without adding or deleting them. 												*/
/* 	Comments can be made only out of operators (i.e. special characters that are not letters nor digits.)	*/
/*//////////////////////////////////////////////////////////////////////////////////////////////////////////*/


// NOTE: if many valid keywords are concatenated, the first longer 'word' will be taken as a whole.
// E.g. if "=+" and "+=" are both valid, the word "=+=" will be interpreted as "=+","="

// Note: single line comments cannot cause errors. they are ended with '\n' or with EOF.


#include "tokenizer.h"

#define STRCMP strcasecmp
// define STRCMP as strcasecmp or as strcmp to make the language case sensitive or not.

// Elements that are ignored as whitespace
char SEPARATORS[] = {'\0', '\n', '\t', '\r', ' '};
// strings for enum elements
char KEYWORDS[][MAX_WORD_LEN] = {
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

const int KEYWORDSSIZE = sizeof(KEYWORDS)/sizeof(KEYWORDS[0]);
