#ifndef TOKEN_H
#define TOKEN_H
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern FILE* yyin, * yyout;

typedef enum TOKEN_TYPE
{
	TOKEN_INT,
	TOKEN_IF,
	TOKEN_WHILE,
	TOKEN_OP_ADD,
	TOKEN_OP_MUL,
	TOKEN_OP_EQUAL,
	TOKEN_OP_ASSIGNMENT,
	TOKEN_OP_GREAT,
	TOKEN_OP_GREAT_EQ,
	TOKEN_OP_LESS,
	TOKEN_OP_LESS_EQ,
	TOKEN_OP_INEQUAL,
	TOKEN_ID,
	TOKEN_COMP,
	TOKEN_FLOAT,
	TOKEN_VOID,
	TOKEN_RETURN,
	TOKEN_COMMA,
	TOKEN_COLON,
	TOKEN_SEMICOLON,
	TOKEN_OPEN_PARENTHESES,
	TOKEN_CLOSE_PARENTHESES,
	TOKEN_OPEN_BRACKETS,
	TOKEN_CLOSE_BRACKETS,
	TOKEN_OPEN_CURLY,
	TOKEN_CLOSE_CURLY,
	TOKEN_EOF,
	TOKEN_ERROR
}TOKEN_TYPE;

typedef struct Token
{
	TOKEN_TYPE kind;
	char* lexeme;
	int lineNumber;
}Token;

typedef struct Node
{
	Token* tokensArray;
	struct Node* prev;
	struct Node* next;
} Node;

const char* tokenEnumToString(TOKEN_TYPE tokenType);
void create_and_store_token(TOKEN_TYPE kind, char* lexeme, int numOfLine);
Token* next_token();
Token* back_token();
void restartTokenizer();



#endif