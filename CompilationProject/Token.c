
#include "Token.h"


/* This package describes the storage of tokens identified in the input text.
* The storage is a bi-directional list of nodes.
* Each node is an array of tokens; the size of this array is defined as TOKEN_ARRAY_SIZE.
* Such data structure supports an efficient way to manipulate tokens.

There are three functions providing an external access to the storage:
- function create_and_store_tokens ; it is called by the lexical analyzer when it identifies a legal token in the input text.
- functions next_token and back_token; they are called by parser during the syntax analysis (the second stage of compilation)
*/

int writeIndex = 0;
int readIndex = 0;
Node* currentNode = NULL;


#define TOKEN_ARRAY_SIZE 1000

/*
* This function creates a token and stores it in the storage.
*/
void create_and_store_token(eTOKENS kind, char* lexeme, int numOfLine)
{
	int length = strlen(lexeme) + 1;

	// case 1: there is still no tokens in the storage.

	if (currentNode == NULL)
	{
		currentNode = (Node*)malloc(sizeof(Node));

		if (currentNode == NULL)
		{
			fprintf(yyout, "\nUnable to allocate memory! \n");
			exit(0);
		}
		currentNode->tokensArray = (Token*)calloc(sizeof(Token), TOKEN_ARRAY_SIZE);
		if (currentNode->tokensArray == NULL)
		{
			fprintf(yyout, "\nUnable to allocate memory! \n");
			exit(0);
		}
		currentNode->prev = NULL;
		currentNode->next = NULL;
	}

	// case 2: at least one token exsits in the storage.
	else
	{
		// the array (the current node) is full, need to allocate a new node
		if (writeIndex == TOKEN_ARRAY_SIZE - 1)
		{
			writeIndex = 0;
			currentNode->next = (Node*)malloc(sizeof(Node));

			if (currentNode == NULL)
			{
				fprintf(yyout, "\nUnable to allocate memory! \n");
				exit(0);
			}
			currentNode->next->prev = currentNode;
			currentNode = currentNode->next;
			currentNode->tokensArray =(Token*)calloc(sizeof(Token), TOKEN_ARRAY_SIZE);

			if (currentNode->tokensArray == NULL)
			{
				fprintf(yyout, "\nUnable to allocate memory! \n");
				exit(0);
			}
			currentNode->next = NULL;
		}
		else writeIndex++;
	}

	currentNode->tokensArray[writeIndex].kind = kind;
	currentNode->tokensArray[writeIndex].lineNumber = numOfLine;
	currentNode->tokensArray[writeIndex].lexeme = (char*)malloc(sizeof(char) * length);
	

#ifdef _WIN32
	strcpy_s(currentNode->tokensArray[writeIndex].lexeme, length, lexeme);
#else
	strcpy(currentNode->tokensArray[currentIndex].lexeme, lexeme);
#endif		
}

/*
* This function returns the token in the storage that is stored immediately before the current token (if exsits).
*/
Token* back_token() {
	
	if (readIndex == 0) {
		// If we hit a new node than return the last token of the previous node
		if (currentNode->prev != NULL) return &currentNode->prev->tokensArray[TOKEN_ARRAY_SIZE - 1];
		// Else we are on the first node on index 0(no previous token available)
		else return&currentNode->tokensArray[readIndex];

	}	
	return &currentNode->tokensArray[--readIndex];
}


/*
* If the next token already exists in the storage (this happens when back_token was called before this call to next_token):
*  this function returns the next stored token.
* Else: continues to read the input file in order to identify, create and store a new token (using yylex function);
*  returns the token that was created.
*/
Token* next_token() {

	yylex();
	if (readIndex == TOKEN_ARRAY_SIZE - 1) {

		readIndex = 0;
		return &currentNode->tokensArray[TOKEN_ARRAY_SIZE - 1];
	}
	return &currentNode->tokensArray[readIndex++];
}


char* tokenEnumToString(eTOKENS tokenType) {

	switch (tokenType) {

	case TOKEN_INT:
		return "TOKEN_INT";

	case TOKEN_IF:
		return "TOKEN_IF";

	case TOKEN_WHILE:
		return "TOKEN_WHILE";

	case TOKEN_OP_ADD:
		return "TOKEN_OP_ADD";

	case TOKEN_OP_MUL:
		return "TOKEN_OP_MUL";

	case TOKEN_OP_EQUAL:
		return "TOKEN_OP_EQUAL";

	case TOKEN_OP_ASSIGNMENT:
		return "TOKEN_OP_EQUAL";

	case TOKEN_OP_GREAT:
		return "TOKEN_OP_GREAT";

	case TOKEN_OP_GREAT_EQ:
		return "TOKEN_OP_GREAT_EQ";

	case TOKEN_OP_LESS:
		return "TOKEN_OP_LESS";

	case TOKEN_OP_LESS_EQ:
		return "TOKEN_OP_LESS_EQ";

	case TOKEN_OP_INEQUAL:
		return "TOKEN_OP_IN_EQUAL";

	case TOKEN_ID:
		return "TOKEN_ID";

	case TOKEN_FLOAT_NUM:
		return "TOKEN_FLOAT_NUM";

	case TOKEN_COMP:
		return "TOKEN_COMP";

	case TOKEN_INT_NUM:
		return "TOKEN_INT_NUM";

	case TOKEN_FLOAT:
		return "TOKEN_FLOAT";

	case TOKEN_VOID:
		return "TOKEN_VOID";

	case TOKEN_RETURN:
		return "TOKEN_RETURN";

	case TOKEN_COMMA:
		return "TOKEN_COMMA";

	case TOKEN_COLON:
		return "TOKEN_COLON";

	case TOKEN_SEMICOLON:
		return "TOKEN_SEMICOLON";

	case TOKEN_OPEN_PARENTHESES:
		return "TOKEN_OPEN_PARENTHESES";

	case TOKEN_CLOSE_PARENTHESES:
		return "TOKEN_CLOSE_PARENTHESES";

	case TOKEN_OPEN_BRACKETS:
		return "TOKEN_OPEN_BRACKETS";

	case TOKEN_CLOSE_BRACKETS:
		return "TOKEN_CLOSE_BRACKETS";

	case TOKEN_OPEN_CURLY:
		return "TOKEN_OPEN_CURLY";

	case TOKEN_CLOSE_CURLY:
		return "TOKEN_CLOSE_CURLY";

	case TOKEN_EOF:
		return "TOKEN_EOF";
	}
}
