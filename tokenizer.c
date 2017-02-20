

// Token definitions
#include "tokens.h"

int isHex(char c);
void readChar();
void readIDF();
void readNumber(int negative);
void readOp();
int isSeperator();
void readToken();

void addToken(Token * token);


FILE * file;
char currentChar;
Token * tokens;
Token * lastToken;
int charCounter;

int col = -1;
int row = 0;

int commentFlag = 0;
int commentCol;
int commentRow;


void readChar(){
	currentChar = fgetc(file);
	if(currentChar == '\n'){
		row++;
		col = -1;
	} else {
		col++;
	}
}

void readNumber(int negative){

	charCounter = 0;

	Token * token = (Token *) malloc(sizeof(Token));
	memset(token, 0, sizeof(Token));

	token->col = col;
	token->row = row;

	if(negative == 1){
		token->value[charCounter] = '-';
		charCounter++;
	}

	int hasDot = 0, hasE = 0, hasDigitAfterE = 0;
	int isBlob = 0;
	if(currentChar == '0' && negative == 0){
		readChar();
		if(currentChar == 'x' || currentChar == 'X'){
			isBlob = 1;
			readChar();
		} else {
			token->value[charCounter] = '0';
			charCounter++;
		}
	}

	if(isBlob){
		//possibly exceeding MAX_WORD_LENGTH... Solution is needed
		while(isHex(currentChar) == 1 && charCounter < MAX_WORD_LEN){
			token->value[charCounter] = currentChar;
			charCounter++;
			readChar();
		}
		if(token->value[0] != 0) token->code = BLOB_TOKEN;
		else {
			token->value[0] = '0';
			token->value[1] = 'x';
			token->code = ERROR_TOKEN;
		}

	} else {
		// eat digits
		while(isdigit(currentChar) || (currentChar == '.' && hasDot == 0 && hasE == 0) || (currentChar == 'E' && hasE == 0)
				|| ((currentChar == '+' || currentChar == '-') && hasE == 1 && hasDigitAfterE == 0)){
			if(currentChar == '.') hasDot = 1;
			if(hasE == 1 && currentChar != '-' && currentChar != '+') hasDigitAfterE = 1;
			if(currentChar == 'E') hasE = 1;
			token->value[charCounter] = currentChar;
			charCounter++;
			readChar();
		}

		if(isHex(currentChar) == 1 || currentChar == '-'){

			while((isHex(currentChar) == 1 || currentChar == '-' ) && charCounter < 34){
				token->value[charCounter] = currentChar;
				charCounter++;
				readChar();
			}
			//check length
			if(charCounter == 34){
				//check hyphens
				if(token->value[6] == '-' && token->value[11] == '-' && token->value[16] == '-' && token->value[21] == '-'){
					token->code = UUID_TOKEN;
				}
			}
			if(token->code != UUID_TOKEN){
				token->code = ERROR_TOKEN;
			}

		} else {
			// verify end of literal
			if(hasE == 1 && hasDigitAfterE == 0){
				token->code = ERROR_TOKEN;
			} else if(isalpha(currentChar) || currentChar == '_'){
				token->code = ERROR_TOKEN;
				while(isalnum(currentChar)){
					if(charCounter < MAX_WORD_LEN) token->value[charCounter] = currentChar;
					charCounter++;
					readChar();
				}
			} else {
				if(hasDot == 1 || hasE == 1)
					token->code = FLOAT_TOKEN;
				else
					token->code = INT_TOKEN;
			}
		}

	}

	//add token
	addToken(token);

}

int isHex(char c){
	return (c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5' || c == '6' || c == '7' || c == '8' || c == '9'
			|| c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f'
			|| c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F');
}

void readIDF(){

	int quotedMode = (currentChar == '"' ? 1 : 0);

	charCounter = 0;

	Token * token = (Token *) malloc(sizeof(Token));
	memset(token, 0, sizeof(Token));

	token->col = col;
	token->row = row;

	//NOTE: string might easily exceed maxLength... Solution is yet to be discussed.
	if(quotedMode == 1){
		readChar();
		int endOfString = 0;
		while(endOfString != 1 && currentChar != EOF){
			if(currentChar != '"'){
				if(charCounter < MAX_WORD_LEN){
					token->value[charCounter] = currentChar;
					charCounter++;
				}
				readChar();
			} else {
				readChar();
				if(currentChar == '"'){
					if(charCounter < MAX_WORD_LEN){
						token->value[charCounter] = currentChar;
						charCounter++;
					}
					readChar();
				} else {
					endOfString = 1;
				}
			}
		}
		if(endOfString == 1) token->code = IDENTIFIER_TOKEN;
		else token->code = ERROR_TOKEN;
		//NOTE: maybe add a token called QUOTED_IDF_TOKEN, since quoted idf are case sensitive
	} else {
		//not quoted
		token->value[charCounter] = currentChar;
		charCounter++;
		readChar();

		int hasDigit = 0;

		while(isalnum(currentChar)){
			if(charCounter < MAX_WORD_LEN) token->value[charCounter] = currentChar;
			charCounter++;
			readChar();

			if(isdigit(currentChar)) hasDigit = 1;
		}

		if(currentChar == '-'){
			//check for hex
			if(isHex(token->value[0]) == 1 && isHex(token->value[1]) == 1 && isHex(token->value[2]) == 1
					&& isHex(token->value[3]) == 1 && isHex(token->value[4]) == 1 && isHex(token->value[5]) == 1){

				if(isHex(currentChar) == 1 || currentChar == '-'){

					while((isHex(currentChar) == 1 || currentChar == '-' ) && charCounter < 34){
						token->value[charCounter] = currentChar;
						charCounter++;
						readChar();
					}
					//check length
					if(charCounter == 34){
						//check hyphens
						if(token->value[6] == '-' && token->value[11] == '-' && token->value[16] == '-' && token->value[21] == '-'){
							token->code = UUID_TOKEN;
						}
					}
					if(token->code != UUID_TOKEN){
						token->code = ERROR_TOKEN;
					}

				}

			}

		} else {
			//check for keywords
			token->code = IDENTIFIER_TOKEN;
			if(hasDigit == 0){
				for(int i = 0; i < sizeof(KEYWORDS)/sizeof(KEYWORDS[0]); i++){
					if(STRCMP(token->value, KEYWORDS[i]) == 0 && i <= END_MULTILINE_COMMENT_TOKEN){
						token->code = i;
						i = sizeof(KEYWORDS)/sizeof(KEYWORDS[0]);
					}
				}
			}
			if(strcasecmp("NaN", token->value) == 0 || strcasecmp("Infinity", token->value) == 0){
				token->code = FLOAT_TOKEN;
			}
		}


	}


	//add token
	addToken(token);
}

void readString(){
//Should this function eliminate '\n' ?

	charCounter = 0;

	Token * token = (Token *) malloc(sizeof(Token));
	memset(token, 0, sizeof(Token));

	token->col = col;
	token->row = row;

	//NOTE: string might easily exceed maxLength... Solution is yet to be discussed.
	readChar(); // to eliminate the first '
	int endOfString = 0;
	while(endOfString != 1 && currentChar != EOF){
		if(currentChar != '\''){
			if(charCounter < MAX_WORD_LEN){
				token->value[charCounter] = currentChar;
				charCounter++;
			}
			readChar();
		} else {
			readChar();
			if(currentChar == '\''){
				if(charCounter < MAX_WORD_LEN){
					token->value[charCounter] = currentChar;
					charCounter++;
				}
				readChar();
			} else {
				endOfString = 1;
			}
		}
	}
	if(endOfString == 1) token->code = STRING_TOKEN;
	else token->code = ERROR_TOKEN;
	//add token
	addToken(token);
}


void readOp(){

	charCounter = 0;

	Token * token = (Token *) malloc(sizeof(Token));
	memset(token, 0, sizeof(Token));

	token->col = col;
	token->row = row;

	int finished = 0;
	token->code = -1;

	while(!finished && charCounter < MAX_WORD_LEN){

		int found = 0;

		token->value[charCounter] = currentChar;

		for(int i = 0; i < sizeof(KEYWORDS)/sizeof(KEYWORDS[0]); i++){
			if(STRCMP(token->value, KEYWORDS[i]) == 0 && i <= END_MULTILINE_COMMENT_TOKEN){
				token->code = i;
				found = 1;
				i = sizeof(KEYWORDS)/sizeof(KEYWORDS[0]);
			}
		}


		if(!found){

			if(token->code == -1){
				token->code = ERROR_TOKEN;
				readChar();
				charCounter++;
			} else {
				token->value[charCounter] = 0;
			}

			finished = 1;
		} else {
			readChar();
			charCounter++;
		}
	}


	//handling detection of comments
	if(token->code == SINGLELINE_COMMENT_TOKEN || token->code == OTHER_SINGLELINE_COMMENT_TOKEN){
		while(currentChar != '\n' && currentChar != EOF){
			readChar();
		}
		if(currentChar != EOF) readChar();

	} else if(token->code == START_MULTILINE_COMMENT_TOKEN){

		if(!commentFlag){
			commentFlag = 1;
			commentCol = token->col;
			commentRow = token->row;
		}

	} else if(token->code == END_MULTILINE_COMMENT_TOKEN){
		if(commentFlag) commentFlag = 0;
		else {
			token->code = ERROR_TOKEN;
			addToken(token);
		}
	} else if(!commentFlag){
		if(isdigit(currentChar) && token->code == MOINS_TOKEN){
			free(token);
			readNumber(1);
		} else {
			//add token
			addToken(token);
		}

	}
	// free token, because it didn't get added to the list.
	if(commentFlag){
		free(token);
	}


}

int isSeperator(){
	for(int i = 0; i < sizeof(SEPARATORS)/sizeof(SEPARATORS[0]); i++){
		if(currentChar == SEPARATORS[i]) return 1;
	}
	return 0;
}

void readToken(){

    // Comment handling: only read operators to check for multiline comment end
	if(commentFlag){

		while(commentFlag == 1){
			while((isalnum(currentChar) || isSeperator()) && currentChar != EOF){
				// while not operator
				readChar();
			}
			if(currentChar != EOF){
				readOp();
			} else {
				// file ended with open comment
				Token * token = (Token *) malloc(sizeof(Token));
				memset(token, 0, sizeof(Token));
				strcpy(token->value,KEYWORDS[START_MULTILINE_COMMENT_TOKEN]);
				token->col = col;
				token->row = row;
				token->code = ERROR_TOKEN;
				addToken(token);
				commentFlag = 0;
			}

		}

	} else {

		// divide the work to 4 sections : word (keywords + idf), literal, operator (& error), separator
		if(isalpha(currentChar) || currentChar == '_' || currentChar == '"'){
			//identifier or keyword
			readIDF();
		} else if(isdigit(currentChar)){
			//literal
			readNumber(0);
		}  else if (currentChar == '\'') {
			//strings
			readString();
		} else if(isSeperator()) {
			//to be ignored
			readChar();
		} else {
			//Operators
			readOp();
		}

	}

}

Token * tokenize(char * filePath){
	FILE *f = NULL;  
	if(filePath == NULL) return NULL;
	if(filePath[0] == '\n'){
		filePath++;
	}
	if(filePath[0] == '@'){
		filePath++;
		file = fopen(filePath, "r");
	}else{
		f = fopen ("stdin", "w");  
		fprintf (f, "%s", filePath);  
		fclose(f);
		f = fopen ("stdin", "r");  
		file = f;
	}
	if(file != NULL){
		readChar();
		if(currentChar == EOF){
			printf("File is empty.\n");
		} else {
			while(currentChar != EOF){
                readToken();
			}
			if(currentChar == EOF){
				Token * token = (Token *) malloc(sizeof(Token));
				memset(token, 0, sizeof(Token));
				strcpy(token->value,KEYWORDS[END_TOKEN]);
				token->col = col;
				token->row = row;
				token->code = END_TOKEN;
				addToken(token);
			}
		}
		fclose(file);
	}
	//printTokens();
	return tokens;
}


void printTokens(){
	Token * token = tokens;
	while(token != NULL){
		printToken(token);
		token = token->next;
	}
}

void printToken(Token * token){
	printf("Token Value: %s\nCode: \"%s\" (%d)\nCol: %d\tRow: %d\n\n",token->value, KEYWORDS[token->code], token->code, token->col, token->row);
}

//adds token to tokens list
void addToken(Token * token){
	token->next = NULL;
	if(tokens == NULL){
		tokens = token;
		lastToken = token;
	} else {
		lastToken->next = token;
		lastToken = token;
	}
}


// deallocates all the tokens
void freeTokens(){
	Token * temp;
	while(tokens != NULL){
		temp = tokens;
		tokens = tokens->next;
		free(temp);
	}
	tokens = NULL;
	lastToken = NULL;
	col = -1;
	row = 0;
}
