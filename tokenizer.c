

// définitions des tokens
#include "tokens.h"

int isHex(char c);
void readChar();
void readIDF();
void readNumber(int negative);
void readOp();
int isSeperator();
void readToken();

void addToken(Token * token);


//ficher à lire
FILE * file;

// le caractère courant
char currentChar;

// la liste chainée des token
Token * tokens;

// dernier token
Token * lastToken;

int charCounter;

int col = -1;
int row = 0;


//les drapeaux des commentaires pour sauter les caractères jusqu'à la fin du commentaire
int commentFlag = 0;
int commentCol;
int commentRow;


// avancer la tête de lecture de caractère
void readChar(){
	currentChar = fgetc(file);

	// avancer le compteur des colonnes et des lignes
	if(currentChar == '\n'){
		row++;
		col = -1;
	} else {
		col++;
	}
}


// continuer la lecture d'un nombre
void readNumber(int negative){

	charCounter = 0;

	Token * token = (Token *) malloc(sizeof(Token));
	memset(token, 0, sizeof(Token));

	token->col = col;
	token->row = row;

	// si la constante débute par le signe moins
	if(negative == 1){
		token->value[charCounter] = '-';
		charCounter++;
	}


	// traiter les differents câs: ecriture avec virgule, ou exponentielle ou hexadecimale
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

	// si la constante est un objet binaire
	if(isBlob){

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
		// consommer un digit (traitant tous les caractères prévues)
		while(isdigit(currentChar) || (currentChar == '.' && hasDot == 0 && hasE == 0) || (currentChar == 'E' && hasE == 0)
				|| ((currentChar == '+' || currentChar == '-') && hasE == 1 && hasDigitAfterE == 0)){
			if(currentChar == '.') hasDot = 1;
			if(hasE == 1 && currentChar != '-' && currentChar != '+') hasDigitAfterE = 1;
			if(currentChar == 'E') hasE = 1;
			token->value[charCounter] = currentChar;
			charCounter++;
			readChar();
		}

		// si la constante est blob elle va contenir des '-' et des hexadecimaux
		if(isHex(currentChar) == 1 || currentChar == '-'){

			// lire le Blob
			while((isHex(currentChar) == 1 || currentChar == '-' ) && charCounter < 34){
				token->value[charCounter] = currentChar;
				charCounter++;
				readChar();
			}
			// verifie la longueur du blob
			if(charCounter == 34){
				// vérifier les traits d'union
				if(token->value[6] == '-' && token->value[11] == '-' && token->value[16] == '-' && token->value[21] == '-'){
					token->code = UUID_TOKEN;
				}
			}
			if(token->code != UUID_TOKEN){
				token->code = ERROR_TOKEN;
			}

		} else {
			// vérifier la fin de la constante
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
				// associer le type du nombre
				if(hasDot == 1 || hasE == 1)
					token->code = FLOAT_TOKEN;
				else
					token->code = INT_TOKEN;
			}
		}

	}

	//ajouter token
	addToken(token);

}


// vérifier si le caractère represente un chiffre hexadecimal
int isHex(char c){
	return (c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5' || c == '6' || c == '7' || c == '8' || c == '9'
			|| c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f'
			|| c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F');
}


// lire un identifiant
void readIDF(){

	// drapeau pour distinguer "idf" et idf
	int quotedMode = (currentChar == '"' ? 1 : 0);

	charCounter = 0;

	Token * token = (Token *) malloc(sizeof(Token));
	memset(token, 0, sizeof(Token));

	token->col = col;
	token->row = row;

	// si on a des guillemets
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
		
	} else {
		// sans guillemets
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
			// verifier s'il continet hex
			if(isHex(token->value[0]) == 1 && isHex(token->value[1]) == 1 && isHex(token->value[2]) == 1
					&& isHex(token->value[3]) == 1 && isHex(token->value[4]) == 1 && isHex(token->value[5]) == 1){

				if(isHex(currentChar) == 1 || currentChar == '-'){

					while((isHex(currentChar) == 1 || currentChar == '-' ) && charCounter < 34){
						token->value[charCounter] = currentChar;
						charCounter++;
						readChar();
					}
					//verifier la longueur
					if(charCounter == 34){
						//verifier les traits d'union
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
			//vérifier les mots clé
			token->code = IDENTIFIER_TOKEN;
			if(hasDigit == 0){
				for(int i = 0; i < sizeof(KEYWORDS)/sizeof(KEYWORDS[0]); i++){
					if(STRCMP(token->value, KEYWORDS[i]) == 0 && i <= END_MULTILINE_COMMENT_TOKEN){
						token->code = i;
						i = sizeof(KEYWORDS)/sizeof(KEYWORDS[0]);
					}
				}
			}
			// NaN et Infinity se sont des constantes
			if(strcasecmp("NaN", token->value) == 0 || strcasecmp("Infinity", token->value) == 0){
				token->code = FLOAT_TOKEN;
			}
		}


	}


	//ajouter token
	addToken(token);
}


// lire une chaine de caractère
void readString(){

	charCounter = 0;

	Token * token = (Token *) malloc(sizeof(Token));
	memset(token, 0, sizeof(Token));

	token->col = col;
	token->row = row;

	readChar(); //eliminer le premier "
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
	//ajouter token
	addToken(token);
}

// lire un symbole d'operateur (+,-,* ...)
void readOp(){

	charCounter = 0;

	Token * token = (Token *) malloc(sizeof(Token));
	memset(token, 0, sizeof(Token));

	token->col = col;
	token->row = row;

	int finished = 0;
	token->code = -1;

	// lire les caractères jusqu'à arriver à une chaine qui ne figure pas dans la liste des opérateurs
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


	//traiter les commentaires
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
			//ajouter token
			addToken(token);
		}

	}
	// déallouer le token car il n'était pas ajouté à la liste des token
	if(commentFlag){
		free(token);
	}


}


// lire un separateur (whitespace)
int isSeperator(){
	for(int i = 0; i < sizeof(SEPARATORS)/sizeof(SEPARATORS[0]); i++){
		if(currentChar == SEPARATORS[i]) return 1;
	}
	return 0;
}


// lire un token
void readToken(){

    // gestion des commentaires (une ligne et multiple lignes)
	if(commentFlag){

		while(commentFlag == 1){
			while((isalnum(currentChar) || isSeperator()) && currentChar != EOF){
				// tant que ce n'est pas un séparateur
				readChar();
			}
			if(currentChar != EOF){
				readOp();
			} else {
				// fin de fichier avec un commentaire non términé
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

		// diviser le travail sur 4 sections
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
			//whitespace
			readChar();
		} else {
			//Operateurs
			readOp();
		}

	}

}

// extracter la liste des token depuis un fichier
Token * tokenize(char * filePath){
	FILE *f = NULL;  
	if(filePath == NULL) return NULL;
	f = fopen ("queryFile", "r");  
	file = f;
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

// afficher les tokens
void printTokens(){
	Token * token = tokens;
	while(token != NULL){
		printToken(token);
		token = token->next;
	}
}

// afficher un seul token
void printToken(Token * token){
	printf("Token Value: %s\nCode: \"%s\" (%d)\nCol: %d\tRow: %d\n\n",token->value, KEYWORDS[token->code], token->code, token->col, token->row);
}

//ajouter un token à la liste
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


// déallouer un token
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