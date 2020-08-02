#ifndef MY_PARSER_H
#define MY_PARSER_H


#include "Token.h"
#include <stdio.h>
#include <stdlib.h>
#include "SymbolTable.h"

static FILE* log_file_syntactic = NULL;

static FILE* log_file_semantic = NULL;

static Token* currToken = NULL;

static Table* currTable;

static Table* mainTable;

static char* currFullFuncName;

void startParsing(int);

void match(TOKEN_TYPE token);

void errorRecovery(TOKEN_TYPE follow[], int size);

void parseProgram();

void parseGlobalVars();

void parseFuncFullDefs();

void parseFuncFullDefsTag();

void parseGlobalVarsTag();

void parseVarDec();

void parseVarDecTag(TableEntry*);

TOKEN_TYPE parseType();

void parseDimSizes(TableEntry*);

void parseDimSizesTag(TableEntry*);

TableEntry* parseFuncPrototype();

void parseFuncWithBody();

TOKEN_TYPE pasreReturnedType();

TableEntry* parseParams();

TableEntry* parseParamsList();

void parseParamListTag(TableEntry*);

TableEntry* parseParam();

void parseParamTag(TableEntry*);

void parseCompStmt(Role role);

void parseVarDecList();

void parseVarDecListTag();

void parseStmtList();

void parseStmtListTag();

void parseStmt();

void parseStmtTag();

ValueType* parseArgs();

ValueType* parseArgsList();

void parseArgsListTag(ValueType*);

void parseReturnStmt();

void parseReturnStmtTag();

ValueType* parseExprList();

void parseExprListTag(ValueType*);

void parseCondition();

ValueType* parseExpr();

ValueType* parseExprTag();

ValueType* parseTerm();

ValueType* parseTermTag();

ValueType* parseFactor();

TOKEN_TYPE parseFactorTag();

void print_already_declared_name(Token);

void createParametrsTable(TableEntry*);

void checkMatchingSignature(TableEntry*, TableEntry*);

void checkMatchingReturnType(TOKEN_TYPE, TOKEN_TYPE);

void checkAllVariablesWereUsed();

TOKEN_TYPE getExpressionType(TOKEN_TYPE, TOKEN_TYPE);

void checkFunctionArgumentCall(ValueType*, TableEntry*, int);

void addFuncEntry(TableEntry*, Table*);

void checkArrayAccess(ValueType*, TableEntry*, int);

void onCloseCurly();

void checkReturnStatement();

void checkValidId(TableEntry*, int, char*);

void checkIdAssigment(TOKEN_TYPE, TableEntry*, int);

void checkArrayElemAssignment(TOKEN_TYPE, TableEntry*, int);

void validateIDWasNotDeclared(TableEntry*, TableEntry*, char*, int);

TOKEN_TYPE getLookAhead(int);

void performLexicalGoBack(int);

void onCloseMainTable();

void checkRemaningPredef();

void checkReturnValue(TableEntry*);


#endif
