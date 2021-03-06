#ifndef FOLLOW_H
#define FOLLOW_H

#include "Token.h"


TOKEN_TYPE programFollow[] = { TOKEN_EOF };

TOKEN_TYPE globalVarsFollow[] = { TOKEN_INT, TOKEN_FLOAT, TOKEN_VOID };

TOKEN_TYPE globalVarsTagFollow[] = { TOKEN_INT, TOKEN_FLOAT, TOKEN_VOID };

TOKEN_TYPE varDecFollow[] = { TOKEN_INT, TOKEN_FLOAT, TOKEN_VOID, TOKEN_ID, TOKEN_IF, TOKEN_RETURN, TOKEN_OPEN_CURLY };

TOKEN_TYPE varDecTagFollow[] = { TOKEN_INT, TOKEN_FLOAT, TOKEN_VOID, TOKEN_ID, TOKEN_IF, TOKEN_RETURN, TOKEN_OPEN_CURLY };

TOKEN_TYPE typeFollow[] = { TOKEN_ID };

TOKEN_TYPE dimSizesFollow[] = { TOKEN_CLOSE_BRACKETS };

TOKEN_TYPE dimSizesTagFollow[] = { TOKEN_CLOSE_BRACKETS };

TOKEN_TYPE funcPredefsFollow[] = { TOKEN_INT, TOKEN_FLOAT, TOKEN_VOID };

TOKEN_TYPE funcPredefsTagFollow[] = { TOKEN_SEMICOLON };

TOKEN_TYPE funcPrototypeFollow[] = { TOKEN_SEMICOLON, TOKEN_OPEN_CURLY };

TOKEN_TYPE funcFullDefs[] = { TOKEN_EOF };

TOKEN_TYPE funcFullDefsTag[] = { TOKEN_EOF };

TOKEN_TYPE funcWithBodyFollow[] = { TOKEN_VOID, TOKEN_INT, TOKEN_FLOAT, TOKEN_EOF };

TOKEN_TYPE returnedTypeFollow[] = { TOKEN_ID };

TOKEN_TYPE paramsFollow[] = { TOKEN_CLOSE_PARENTHESES };

TOKEN_TYPE paramsListFollow[] = { TOKEN_CLOSE_PARENTHESES };

TOKEN_TYPE paramsListTagFollow[] = { TOKEN_CLOSE_PARENTHESES };

TOKEN_TYPE paramFollow[] = { TOKEN_CLOSE_PARENTHESES, TOKEN_COMMA };

TOKEN_TYPE paramTagFollow[] = { TOKEN_CLOSE_PARENTHESES, TOKEN_COMMA };

TOKEN_TYPE compStmtFollow[] = { TOKEN_CLOSE_CURLY, TOKEN_VOID, TOKEN_INT, TOKEN_FLOAT, TOKEN_SEMICOLON, TOKEN_EOF };

TOKEN_TYPE varDecListFollow[] = { TOKEN_OPEN_CURLY, TOKEN_ID, TOKEN_IF, TOKEN_RETURN };

TOKEN_TYPE varDecListTagFollow[] = { TOKEN_OPEN_CURLY, TOKEN_ID, TOKEN_IF, TOKEN_RETURN };

TOKEN_TYPE stmtListFollow[] = { TOKEN_CLOSE_CURLY };

TOKEN_TYPE stmtListTagFollow[] = { TOKEN_CLOSE_CURLY };

TOKEN_TYPE stmtFollow[] = { TOKEN_CLOSE_CURLY, TOKEN_SEMICOLON };

TOKEN_TYPE stmtTagFollow[] = { TOKEN_CLOSE_CURLY, TOKEN_SEMICOLON };

TOKEN_TYPE argsFollow[] = { TOKEN_CLOSE_PARENTHESES };

TOKEN_TYPE argListFollow[] = { TOKEN_CLOSE_PARENTHESES };

TOKEN_TYPE argListTagFollow[] = { TOKEN_CLOSE_PARENTHESES };

TOKEN_TYPE returnStmtFollow[] = { TOKEN_SEMICOLON, TOKEN_CLOSE_CURLY };

TOKEN_TYPE returnStmtTagFollow[] = { TOKEN_SEMICOLON, TOKEN_CLOSE_CURLY };

TOKEN_TYPE exprListFollow[] = { TOKEN_CLOSE_BRACKETS };

TOKEN_TYPE exprListTagFollow[] = { TOKEN_CLOSE_BRACKETS };

TOKEN_TYPE conditionFollow[] = { TOKEN_CLOSE_PARENTHESES };

TOKEN_TYPE exprFollow[] = { TOKEN_CLOSE_BRACKETS, TOKEN_SEMICOLON, TOKEN_COMMA, TOKEN_CLOSE_PARENTHESES, TOKEN_CLOSE_CURLY,
							TOKEN_OP_EQUAL, TOKEN_OP_GREAT, TOKEN_OP_GREAT_EQ, TOKEN_OP_LESS, TOKEN_OP_LESS_EQ, TOKEN_OP_INEQUAL };

TOKEN_TYPE exprTagFollow[] = { TOKEN_CLOSE_BRACKETS, TOKEN_SEMICOLON, TOKEN_COMMA, TOKEN_CLOSE_PARENTHESES, TOKEN_CLOSE_CURLY,
							TOKEN_OP_EQUAL, TOKEN_OP_GREAT, TOKEN_OP_GREAT_EQ, TOKEN_OP_LESS, TOKEN_OP_LESS_EQ, TOKEN_OP_INEQUAL };

TOKEN_TYPE termFollow[] = { TOKEN_OP_ADD, TOKEN_CLOSE_BRACKETS, TOKEN_SEMICOLON, TOKEN_COMMA, TOKEN_CLOSE_PARENTHESES, TOKEN_CLOSE_CURLY,
							TOKEN_OP_EQUAL,TOKEN_OP_GREAT,TOKEN_OP_GREAT_EQ,TOKEN_OP_LESS,TOKEN_OP_LESS_EQ, TOKEN_OP_INEQUAL };

TOKEN_TYPE termTagFollow[] = { TOKEN_OP_ADD, TOKEN_CLOSE_BRACKETS, TOKEN_SEMICOLON, TOKEN_COMMA, TOKEN_CLOSE_PARENTHESES, TOKEN_CLOSE_CURLY,
							TOKEN_OP_EQUAL,TOKEN_OP_GREAT,TOKEN_OP_GREAT_EQ,TOKEN_OP_LESS,TOKEN_OP_LESS_EQ, TOKEN_OP_INEQUAL };

TOKEN_TYPE factorFollow[] = { TOKEN_OP_MUL, TOKEN_OP_ADD, TOKEN_CLOSE_BRACKETS, TOKEN_SEMICOLON, TOKEN_COMMA, TOKEN_CLOSE_PARENTHESES, TOKEN_CLOSE_CURLY,
							TOKEN_OP_EQUAL,TOKEN_OP_GREAT,TOKEN_OP_GREAT_EQ,TOKEN_OP_LESS,TOKEN_OP_LESS_EQ, TOKEN_OP_INEQUAL };

TOKEN_TYPE factorTagFollow[] = { TOKEN_OP_MUL, TOKEN_OP_ADD, TOKEN_CLOSE_BRACKETS, TOKEN_SEMICOLON, TOKEN_COMMA, TOKEN_CLOSE_PARENTHESES, TOKEN_CLOSE_CURLY,
							TOKEN_OP_EQUAL,TOKEN_OP_GREAT,TOKEN_OP_GREAT_EQ,TOKEN_OP_LESS,TOKEN_OP_LESS_EQ, TOKEN_OP_INEQUAL };

#endif