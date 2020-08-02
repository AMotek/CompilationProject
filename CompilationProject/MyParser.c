#include "MyParser.h"
#include "Follow.h"
#include "ErrorMessageCenter.h"


/*
	The main idea of implementation of part 3 is combination of both approches provided in tips for function implementation file.
	We implement the second approach which means we create a new table entry for the paremeters of the full definitions functions once enterting it's scope.
	Once a full definition function is detected we check if the same pre def has been inserted before, if so we change the same entry to be a full def function Role
and change the singature(including the return type) to the full def one.
	Obviously if the signatures do not match a proper error message will be printed but yet the signature will be changed.
	That way a further functions call will be checked againt the full function definition signature if the same full defintion has been inserted before hand, other wise it will be check against
	the predef function definition.
*/

void startParsing(int fileToOpen) {

	if (fileToOpen == 1) {
		log_file_syntactic = fopen("C:\\temp\\test1_204371454_205838618__syntactic.txt", "w");
		log_file_semantic = fopen("C:\\temp\\test1_204371454_205838618__semantic.txt", "w");
	}

	else {
		log_file_syntactic = fopen("C:\\temp\\test2_204371454_205838618__syntactic.txt", "w");
		log_file_semantic = fopen("C:\\temp\\test2_204371454_205838618__semantic.txt", "w");
	}

	if (log_file_syntactic == NULL) printf("Unable to open syntactic file for write, closing program...\n");

	else if (log_file_semantic == NULL) printf("Unable to open semantic file for write, closing program...\n");

	else parseProgram();
}

void match(TOKEN_TYPE token_type) {


	currToken = next_token();
	if (currToken->kind != token_type) {
		fprintf(log_file_syntactic, "Expected token of type '{%s}', at line : {%d}. ", tokenEnumToString(token_type), currToken->lineNumber);
		fprintf(log_file_syntactic, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
	}
}

void errorRecovery(TOKEN_TYPE follow[], int size) {

	int found = 0;

	while (!found) {

		currToken = next_token();
		for (int i = 0; i < size; i++) {

			if (currToken->kind == TOKEN_EOF || currToken->kind == follow[i]) {
				found = 1;
				currToken = back_token();
				break;
			}
		}
	}
}

void parseProgram() {

	mainTable = makeTable(NULL); // Creating main table
	currTable = mainTable;
	restartTokenizer(); // Restart the tokenzier each time a new parse is starting.

	fprintf(log_file_syntactic, "Rule(PROG->GLOBAL_VARS FUNC_PREDEFS FUNC_FULL_DEFS)\n");

	parseGlobalVars();

	fprintf(log_file_syntactic, "Rule(FUNC_PREDEFS->FUNC_PROTOTYPE; FUNC_PREDEFS')\n");
	do {

		// The idea is to hold a temporary entry with all the infomration needed from the parsers. Once the parsing is done i will validate the same entry name has not been enterd, if so enter a new 
		// entry to the current table and copy all the information from the temporary entry pointer.

		TableEntry* tempEntry = parseFuncPrototype();

		currToken = next_token();

		if (currToken->kind == TOKEN_OPEN_CURLY) {

			setRole(tempEntry, FULL_DEF_FUNC);
			currFullFuncName = getEntryName(tempEntry);
		}

		else {

			setRole(tempEntry, PRE_DEF_FUNC);
			currToken = back_token();
			match(TOKEN_SEMICOLON);
		}
		addFuncEntry(tempEntry, mainTable);


	} while (currToken->kind == TOKEN_SEMICOLON && getLookAhead(1) != TOKEN_EOF);


	fprintf(log_file_syntactic, "Rule(FUNC_PREDEFS'->epsilon)\n");
	fprintf(log_file_syntactic, "Rule(FUNC_FULL_DEFS->FUNC_WITH_BODY FUNC_FULL_DEFS')\n");

	if (currToken->kind == TOKEN_OPEN_CURLY) {

		fprintf(log_file_syntactic, "Rule(FUNC_WITH_BODY->FUNC_PROTOTYPE COMP_STMT)\n");
		currToken = back_token();
		parseCompStmt(FULL_DEF_FUNC);

		currToken = next_token();

		if (currToken->kind != TOKEN_EOF) {
			currToken = back_token();

			parseFuncFullDefs();

		}
		else {
			currToken = back_token();
			fprintf(log_file_syntactic, "Rule(FUN_FUL_DEFS'-> epsilon)\n");
		}
	}
	else {
		fprintf(log_file_syntactic, "Expected token of type '{TOKEN_OPEN_CURLY}', at line : {%d}. ", currToken->lineNumber);
		fprintf(log_file_syntactic, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
		while (currToken->kind != TOKEN_EOF) currToken = next_token();

		currToken = back_token();
	}

	onCloseMainTable();
	match(TOKEN_EOF);
}

void parseCompStmt(Role role) {

	currToken = next_token();
	// Creating a new table
	Table* tempTable = makeTable(currTable);
	currTable = tempTable;

	switch (currToken->kind) {

	case TOKEN_OPEN_CURLY:
		fprintf(log_file_syntactic, "Rule(COMP_STMT->{ VAR_DEC_LIST STMT_LIST })\n");
		parseVarDecList();
		parseStmtList();
		match(TOKEN_CLOSE_CURLY);
		onCloseCurly();
		// On function call needs to pop two tables. andd check return 
		if (role == FULL_DEF_FUNC) {
			onCloseCurly();
			checkReturnStatement();
		}
		break;

	default:
		fprintf(log_file_syntactic, "Expected token of type '{TOKEN_OPEN_CURLY}', at line : {%d}. ", currToken->lineNumber);
		fprintf(log_file_syntactic, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
		currToken = back_token();
		errorRecovery(paramTagFollow, 6);
		break;
	}
}

void parseStmtList() {

	fprintf(log_file_syntactic, "Rule(STMT_LIST->STMT STMT_LIST')\n");
	parseStmt();
	parseStmtListTag();
}

void parseStmtListTag() {

	currToken = next_token();

	switch (currToken->kind) {

	case TOKEN_SEMICOLON:

		currToken = next_token();
		fprintf(log_file_syntactic, "Rule(STMT_LIST'->;STMT STMT_LIST')\n");
		if (currToken->kind != TOKEN_CLOSE_CURLY && currToken->kind != TOKEN_EOF) {
			currToken = back_token();
			parseStmt();
			parseStmtListTag();
		}
		else {
			// If currToken is close curly than we have finished with this func full defs. may finish the process.
			fprintf(log_file_syntactic, "Rule(STMT_LIST'->epsilon)\n");
			currToken = back_token();
		}
		break;

	default:
		fprintf(log_file_syntactic, "Rule(STMT_LIST'->epsilon)\n");
		currToken = back_token();
		break;
	}

}

void parseStmt() {

	currToken = next_token();

	switch (currToken->kind) {

	case TOKEN_ID:
		fprintf(log_file_syntactic, "Rule(STMT->id STMT')\n");

		parseStmtTag();
		break;

	case TOKEN_IF:
		fprintf(log_file_syntactic, "Rule(STMT->if(CONDITION)STMT)\n");
		match(TOKEN_OPEN_PARENTHESES);
		parseCondition();
		match(TOKEN_CLOSE_PARENTHESES);
		parseStmt();
		break;

	case TOKEN_RETURN:
		fprintf(log_file_syntactic, "Rule(STMT->RETURN_STMT\n");
		parseReturnStmt();
		break;

	case TOKEN_OPEN_CURLY:
		fprintf(log_file_syntactic, "Rule(STMT->COMP_STMT\n");
		currToken = back_token();
		parseCompStmt(NONE);
		break;

	default:
		fprintf(log_file_syntactic, "Expected token of type '{TOKEN_OPEN_CURLY or TOKEN_ID or TOKEN_IF or TOKEN_RETURN}', at line : {%d}. ", currToken->lineNumber);
		fprintf(log_file_syntactic, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
		currToken = back_token();
		errorRecovery(stmtFollow, 2);
		break;
	}
}

void parseStmtTag() {

	TableEntry* currEntry = find(currTable, currToken->lexeme);

	ValueType* valueTypeList = NULL;
	ValueType* valueType;
	int lineNumber = currToken->lineNumber;
	checkValidId(currEntry, lineNumber, currToken->lexeme);
	currToken = next_token();
	switch (currToken->kind) {

	case TOKEN_OPEN_BRACKETS:
		fprintf(log_file_syntactic, "Rule(STMT'->[EXPR_LIST]=EXPR)\n");
		valueTypeList = parseExprList();
		match(TOKEN_CLOSE_BRACKETS);
		match(TOKEN_OP_ASSIGNMENT);

		valueType = parseExpr();
		if (currEntry != NULL) {
			checkArrayAccess(valueTypeList, currEntry, lineNumber);
			checkArrayElemAssignment(valueType->type, currEntry, lineNumber);
			setUsedEntry(TRUE, currEntry);
		}
		break;

	case TOKEN_OPEN_PARENTHESES:
		fprintf(log_file_syntactic, "Rule(STMT'->(ARGS))\n");

		valueTypeList = parseArgs();

		if (currEntry != NULL)
			checkFunctionArgumentCall(valueTypeList, currEntry, lineNumber);

		match(TOKEN_CLOSE_PARENTHESES);
		break;

	case TOKEN_OP_ASSIGNMENT:
		fprintf(log_file_syntactic, "Rule(STMT'-> = EXPR)\n");

		valueType = parseExpr();

		if (currEntry != NULL) {
			checkIdAssigment(valueType->type, currEntry, lineNumber);
			setUsedEntry(TRUE, currEntry);
		}
		break;

	default:
		fprintf(log_file_syntactic, "Expected token of type '{TOKEN_OPEN_PARENTHESES or TOKEN_OPEN_BRACKETS or TOKEN_OP_ASSIGNMENT}', at line : {%d}. ", currToken->lineNumber);
		fprintf(log_file_syntactic, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
		currToken = back_token();
		errorRecovery(stmtTagFollow, 2);
		break;
	}

}

ValueType* parseArgs() {

	ValueType* argsList = NULL;
	currToken = next_token();
	switch (currToken->kind) {

	case TOKEN_ID:
		fprintf(log_file_syntactic, "Rule(ARGS->ARG_LIST)\n");
		currToken = back_token();
		argsList = parseArgsList();
		break;

	case TOKEN_INT:
		fprintf(log_file_syntactic, "Rule(ARGS->ARG_LIST)\n");
		currToken = back_token();
		argsList = parseArgsList();
		break;

	case TOKEN_FLOAT:
		fprintf(log_file_syntactic, "Rule(ARGS->ARG_LIST)\n");
		currToken = back_token();
		argsList = parseArgsList();
		break;

	case TOKEN_OPEN_PARENTHESES:
		fprintf(log_file_syntactic, "Rule(ARGS->ARG_LIST)\n");
		currToken = back_token();
		argsList = parseArgsList();
		break;

	default:
		fprintf(log_file_syntactic, "Rule(ARGS->epsilon)\n");
		currToken = back_token();
		break;
	}

	return argsList;
}

ValueType* parseArgsList() {

	ValueType* list = NULL;
	ValueType* item = NULL;
	currToken = next_token();
	switch (currToken->kind) {

	case TOKEN_ID:
		fprintf(log_file_syntactic, "Rule(ARG_LIST->EXPR)\n");
		currToken = back_token();
		item = parseExpr();
		list = addItemToValueTypeList(NULL, item);
		parseArgsListTag(list);
		break;

	case TOKEN_INT:
		fprintf(log_file_syntactic, "Rule(ARG_LIST->EXPR)\n");
		currToken = back_token();
		item = parseExpr();
		list = addItemToValueTypeList(NULL, item);
		parseArgsListTag(list);
		break;

	case TOKEN_FLOAT:
		fprintf(log_file_syntactic, "Rule(ARG_LIST->EXPR)\n");
		currToken = back_token();
		item = parseExpr();
		list = addItemToValueTypeList(NULL, item);
		parseArgsListTag(list);
		break;

	case TOKEN_OPEN_PARENTHESES:
		fprintf(log_file_syntactic, "Rule(ARG_LIST->EXPR)\n");
		currToken = back_token();
		item = parseExpr();
		list = addItemToValueTypeList(NULL, item);
		parseArgsListTag(list);
		break;

	default:
		fprintf(log_file_syntactic, "Expected token of type '{TOKEN_ID or TOKEN_INT_NUM or TOKEN_FLOAT_NUM or TOKEN_OPEN_PARENTHESES}', at line : {%d}. ", currToken->lineNumber);
		fprintf(log_file_syntactic, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
		currToken = back_token();
		errorRecovery(argListFollow, 1);
		break;
	}


	return list;
}

void parseArgsListTag(ValueType* list) {

	currToken = next_token();
	ValueType* item = NULL;

	switch (currToken->kind) {

	case TOKEN_COMMA:
		fprintf(log_file_syntactic, "Rule(ARG_LIST'->,EXPR ARG_LIST')\n");
		item = parseExpr();
		list = addItemToValueTypeList(list, item);
		parseArgsListTag(list);
		break;

	default:
		fprintf(log_file_syntactic, "Rule(ARG_LIST'->epsilon)\n");
		currToken = back_token();
		break;
	}
}

ValueType* parseExpr() {

	fprintf(log_file_syntactic, "Rule(EXPR->TERM EXPR')\n");
	ValueType* exprValueType = getDummyValueTypeItem();
	int exprVlaue = UNKNOWN_VALUE;
	ValueType* termValueType = parseTerm();
	ValueType* exprTagValueType = parseExprTag();

	TOKEN_TYPE exprType = getExpressionType(termValueType->type, exprTagValueType->type);

	if (exprType != TOKEN_ERROR) {
		if (termValueType->value != UNKNOWN_VALUE && exprTagValueType->value != UNKNOWN_VALUE)
			exprVlaue = termValueType->value + exprTagValueType->value;
	}
	setTypeValue(exprValueType, exprType, exprVlaue);
	// Once finished procees of addition operation, these two can free their memory.
	freeValueTypeList(termValueType);
	freeValueTypeList(exprTagValueType);

	return exprValueType;
}

ValueType* parseExprTag() {

	currToken = next_token();
	// Just a defualt values.
	ValueType* item = getDummyValueTypeItem();
	int value = UNKNOWN_VALUE;
	TOKEN_TYPE typeAddOperation = TOKEN_ERROR;

	switch (currToken->kind) {

	case TOKEN_OP_ADD:
		fprintf(log_file_syntactic, "Rule(EXPR'->+TERM EXPR')\n");
		ValueType* termValueType = parseTerm();
		ValueType* exprTagValueType = parseExprTag();

		typeAddOperation = getExpressionType(termValueType->type, exprTagValueType->type);

		if (typeAddOperation != TOKEN_ERROR) {
			if (termValueType->value != UNKNOWN_VALUE && exprTagValueType->value != UNKNOWN_VALUE)
				value = termValueType->value + exprTagValueType->value;
		}
		// Once finished procees of addition operation, these two can free their memory.
		freeValueTypeList(termValueType);
		freeValueTypeList(exprTagValueType);
		break;

	default:
		fprintf(log_file_syntactic, "Rule(EXPR'->epsilon)\n");
		currToken = back_token();
		// TOKEN_INT_NUN don't effect to final result, hence i use it a defualt return state for the recursive calls of parseExprTag
		//This is the stop condition of the recursive call, and the arithmetic operation for this recursive call is addition, hence addion zero to the final reusult won't effect the result.
		value = 0;
		typeAddOperation = TOKEN_INT;
		break;
	}

	setTypeValue(item, typeAddOperation, value);
	return item;
}

ValueType* parseTerm() {

	fprintf(log_file_syntactic, "Rule(TERM->FACTOR TERM')\n");
	int value = UNKNOWN_VALUE;
	ValueType* item = getDummyValueTypeItem();

	ValueType* factorValueType = parseFactor();

	ValueType* termTagValueType = parseTermTag();
	TOKEN_TYPE resultType = getExpressionType(factorValueType->type, termTagValueType->type);
	if (factorValueType->value != UNKNOWN_VALUE && termTagValueType->value != UNKNOWN_VALUE)
		value = factorValueType->value * termTagValueType->value;

	setTypeValue(item, resultType, value);

	// Once finished procees of addition operation, these two can free their memory.
	freeValueTypeList(factorValueType);
	freeValueTypeList(termTagValueType);

	return item;
}

ValueType* parseTermTag() {

	int value = UNKNOWN_VALUE;
	ValueType* item = getDummyValueTypeItem();
	TOKEN_TYPE typeMulOperation = TOKEN_ERROR;

	currToken = next_token();
	switch (currToken->kind) {

	case TOKEN_OP_MUL:
		fprintf(log_file_syntactic, "Rule(TERM'->*FACTOR TERM')\n");
		ValueType* factorType = parseFactor();
		ValueType* termTagType = parseTermTag();
		typeMulOperation = getExpressionType(factorType->type, termTagType->type);

		if (typeMulOperation != TOKEN_ERROR) {
			if (factorType->value != UNKNOWN_VALUE && termTagType->value != UNKNOWN_VALUE) {
				value = factorType->value * termTagType->value;
			}
		}
		// Once finished procees of addition operation, these two can free their memory.
		freeValueTypeList(factorType);
		freeValueTypeList(termTagType);
		break;

	default:
		currToken = back_token();
		fprintf(log_file_syntactic, "Rule(TERM'->epsilon)\n");
		// TOKEN_INT_NUN don't effect to final result, hence i use it as defualt return state for the recursive calls of parseExprTag
		// This is the stop condition for the recursive calls. The arithmetic operations is multiplication, hence i assign the value 1 so i dont effect the overall result
		typeMulOperation = TOKEN_INT;
		value = 1;
		break;
	}

	setTypeValue(item, typeMulOperation, value);
	return item;
}

ValueType* parseFactor() {
	// Default values.
	ValueType* currTokenValueType = getDummyValueTypeItem();
	TOKEN_TYPE type = TOKEN_ERROR;
	int value = UNKNOWN_VALUE;

	currToken = next_token();

	switch (currToken->kind) {

	case TOKEN_ID:
		fprintf(log_file_syntactic, "Rule(FACTOR-> id FACTOR')\n");
		type = parseFactorTag();
		break;

	case TOKEN_INT:

		fprintf(log_file_syntactic, "Rule(FACTOR-> int_num)\n");
		type = TOKEN_INT;
		char* end;
		value = strtol(currToken->lexeme, &end, 10);
		break;

	case TOKEN_FLOAT:

		fprintf(log_file_syntactic, "Rule(FACTOR-> float_num)\n");

		type = TOKEN_FLOAT;
		break;

	case TOKEN_OPEN_PARENTHESES:
		fprintf(log_file_syntactic, "Rule(FACTOR-> (EXPR))\n");
		ValueType* temp = parseExpr();
		type = temp->type;
		value = temp->value;
		freeValueTypeList(temp);
		match(TOKEN_CLOSE_PARENTHESES);
		break;
	default:
		fprintf(log_file_syntactic, "Expected tokens of type '{TOKEN_ID or TOKEN_INT_NUM or TOKEN_FLOAT_NUM or TOKEN_OPEN_PARENTHESES}', at line : {%d}. ", currToken->lineNumber);
		fprintf(log_file_syntactic, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
		currToken = back_token();
		errorRecovery(factorFollow, 13);
		break;
	}
	setTypeValue(currTokenValueType, type, value);
	return currTokenValueType;
}

TOKEN_TYPE parseFactorTag() {

	TableEntry* currEntry = find(currTable, currToken->lexeme);
	TOKEN_TYPE tokenType = TOKEN_ERROR;
	ValueType* list = NULL;
	checkValidId(currEntry, currToken->lineNumber, currToken->lexeme);

	// If current entry is a function than i need to return the return type else its an array, than needs to return it's type 
	if (currEntry != NULL) {
		tokenType = getType(currEntry);
		setUsedEntry(TRUE, currEntry);
	}
	currToken = next_token();

	switch (currToken->kind) {

	case TOKEN_OPEN_PARENTHESES:
		fprintf(log_file_syntactic, "Rule(FACTOR'-> (ARGS))\n");

		list = parseArgs();
		if (currEntry != NULL)
			checkFunctionArgumentCall(list, currEntry, currToken->lineNumber);
		match(TOKEN_CLOSE_PARENTHESES);
		break;

	case TOKEN_OPEN_BRACKETS:
		fprintf(log_file_syntactic, "Rule(FACTOR'-> [EXPR_LIST])\n");
		list = parseExprList();
		if (currEntry != NULL)
			checkArrayAccess(list, currEntry, currToken->lineNumber);
		match(TOKEN_CLOSE_BRACKETS);
		break;

	default:
		currToken = back_token();
		fprintf(log_file_syntactic, "Rule(FACTOR'-> epsilon)\n");
		break;
	}
	freeValueTypeList(list);
	return tokenType;
}

ValueType* parseExprList() {

	fprintf(log_file_syntactic, "Rule(EXPR_LIST->EXPR EXPR_LIST')\n");
	ValueType* item = parseExpr();
	ValueType* list = addItemToValueTypeList(NULL, item);
	parseExprListTag(list);
	return list;
}


void parseExprListTag(ValueType* list) {

	currToken = next_token();
	ValueType* item = NULL;
	switch (currToken->kind) {

	case TOKEN_COMMA:
		fprintf(log_file_syntactic, "Rule(EXPR_LIST'->, EXPR EXPR_LIST')\n");
		item = parseExpr();
		addItemToValueTypeList(list, item);
		parseExprListTag(list);
		break;

	default:
		fprintf(log_file_syntactic, "Rule(EXPR_LIST'->epsilon)\n");
		currToken = back_token();
		break;
	}
}

void parseReturnStmt() {

	fprintf(log_file_syntactic, "Rule(RETURN_STMT->return RETURN_STMT')\n");
	parseReturnStmtTag();
}

void parseReturnStmtTag() {

	currToken = next_token();
	ValueType* item = NULL;
	int defulatFlag = 0;
	switch (currToken->kind) {

	case TOKEN_ID:
		fprintf(log_file_syntactic, "Rule(RETURN_STMT'->EXPR)\n");
		currToken = back_token();
		item = parseExpr();
		break;

	case TOKEN_INT:
		fprintf(log_file_syntactic, "Rule(RETURN_STMT'->EXPR)\n");
		currToken = back_token();
		item = parseExpr();
		break;

	case TOKEN_FLOAT:
		fprintf(log_file_syntactic, "Rule(RETURN_STMT'->EXPR)\n");
		currToken = back_token();
		item = parseExpr();
		break;

	case TOKEN_OPEN_PARENTHESES:
		fprintf(log_file_syntactic, "Rule(RETURN_STMT'->EXPR)\n");
		currToken = back_token();
		item = parseExpr();
		break;

	default:
		fprintf(log_file_syntactic, "Rule(RETURN_STMT'->epsilon)\n");
		currToken = back_token();
		defulatFlag = 1;
		break;
	}
	// Getting entry for the same full function being parsed at the moment
	TableEntry* sameFunctionEntry = find(mainTable, currFullFuncName);
	setHasReturn(sameFunctionEntry, TRUE);
	setActualReturnType(sameFunctionEntry, TOKEN_VOID);
	// Setting the actual return type. when leaving the scope there will be 
	if (!defulatFlag) {
		setActualReturnType(sameFunctionEntry, item->type);

	}
	checkReturnValue(sameFunctionEntry);
	freeValueTypeList(item);
}

void parseCondition() {

	fprintf(log_file_syntactic, "Rule(CONDITION->EXPR rel_op EXPR)\n");
	ValueType* leftSide = parseExpr();
	currToken = next_token();

	switch (currToken->kind) {

	case TOKEN_OP_ASSIGNMENT:
		// found a match token, do nothing;
		break;

	case TOKEN_OP_EQUAL:
		// found a match token, do nothing;
		break;

	case TOKEN_OP_GREAT:
		// found a match token, do nothing;
		break;

	case TOKEN_OP_GREAT_EQ:
		// found a match token, do nothing;
		break;

	case TOKEN_OP_INEQUAL:
		// found a match token, do nothing;
		break;

	case TOKEN_OP_LESS:
		// found a match token, do nothing;
		break;

	case TOKEN_OP_LESS_EQ:
		// found a match token, do nothing;
		break;

	default:
		fprintf(log_file_syntactic, "Expected token of type 'rel_op', at line : {%d}. ", currToken->lineNumber);
		fprintf(log_file_syntactic, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
		break;
	}

	ValueType* rightSide = parseExpr();
	// TOTO add check for the if condition

}

void parseVarDecList() {

	fprintf(log_file_syntactic, "Rule(VAR_DEC_LIST->VAR_DEC_LIST'))\n");
	parseVarDecListTag();
}

void parseVarDecListTag() {


	currToken = next_token();
	switch (currToken->kind) {

	case TOKEN_INT:
		fprintf(log_file_syntactic, "Rule(VAR_DEC_LIST'->VAR_DEC VAR_DEC_LIST')\n");
		parseVarDec();
		parseVarDecListTag();
		break;

	case TOKEN_FLOAT:
		fprintf(log_file_syntactic, "VAR_DEC_LIST'->VAR_DEC VAR_DEC_LIST')\n");
		parseVarDec();
		parseVarDecListTag();
		break;

	default:
		fprintf(log_file_syntactic, "VAR_DEC_LIST'->VAR_DEC_LIST'->epsilon)\n");
		currToken = back_token();
		break;
	}
}

TableEntry* parseFuncPrototype() {
	// tempEntry will hold all the necessary information regard this func prototpe, the one to validate if needs no duplication is made will be parase_program
	fprintf(log_file_syntactic, "Rule(FUNC_PROTOTYPE->RETURNED_TYPE id(PARAMS))\n");
	TableEntry* tempEntry = getNewDummyEntry();
	TOKEN_TYPE returnType = pasreReturnedType();

	setType(tempEntry, returnType);
	match(TOKEN_ID);
	setEntryName(tempEntry, currToken->lexeme);

	match(TOKEN_OPEN_PARENTHESES);


	// if lookAhead of 1 is close parentheses than the params is empty
	if (getLookAhead(1) != TOKEN_CLOSE_PARENTHESES) {

		TableEntry* paramsList = parseParams();

		setFuncSignature(tempEntry, paramsList);
	}
	else fprintf(log_file_syntactic, "PARAMS->epsilon\n"); // else params is empty

	match(TOKEN_CLOSE_PARENTHESES);
	return tempEntry;
}

TableEntry* parseParams() {

	fprintf(log_file_syntactic, "PARAMS->PARAM_LIST\n");
	TableEntry* list = parseParamsList();
	return list;
}

TableEntry* parseParamsList() {

	fprintf(log_file_syntactic, "PARAM_LIST->PARAM PARAM_LIST'\n");
	TableEntry* entry = parseParam();

	TableEntry* list = addItemToList(NULL, entry);
	parseParamListTag(list);
	return list;
}

void parseParamListTag(TableEntry* list) {

	currToken = next_token();
	switch (currToken->kind) {

	case TOKEN_COMMA:
		fprintf(log_file_syntactic, "PARAM_LIST'->, PARAM PARAM_LIST'\n");
		addItemToList(list, parseParam());
		parseParamListTag(list);
		break;

	case TOKEN_CLOSE_PARENTHESES:
		fprintf(log_file_syntactic, "PARAM_LIST'->epsilon\n");
		currToken = back_token();
		break;

	default:
		fprintf(log_file_syntactic, "Expected token of type '{TOKEN_COMMA or TOKEN_CLOSE_PARENTHESES}', at line : {%d}. ", currToken->lineNumber);
		fprintf(log_file_syntactic, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
		currToken = back_token();
		errorRecovery(paramsListTagFollow, 1);
		break;
	}
}

TableEntry* parseParam() {

	fprintf(log_file_syntactic, "PARAM->TYPE id PARAM'\n");
	TableEntry* entry = getNewDummyEntry();
	currToken = next_token();
	setType(entry, parseType());
	match(TOKEN_ID);
	setEntryName(entry, currToken->lexeme);
	// Role VARIABLE is just a defualt value if it should be ARRAY than parseParamTag will update the same value
	setRole(entry, VARIABLE);
	parseParamTag(entry);
	return entry;
}

void parseParamTag(TableEntry* entry) {


	currToken = next_token();
	switch (currToken->kind) {

	case TOKEN_OPEN_BRACKETS:
		fprintf(log_file_syntactic, "PARAM'->[DIM_SIZES]\n");
		setRole(entry, ARRAY);
		parseDimSizes(entry);
		match(TOKEN_CLOSE_BRACKETS);
		break;

	default:
		fprintf(log_file_syntactic, "PARAM'->epsilon\n");
		currToken = back_token();
		break;
	}
}

TOKEN_TYPE pasreReturnedType() {

	fprintf(log_file_syntactic, "Rule(RETURNED_TYPE->TYPE|void)\n");

	currToken = next_token();

	switch (currToken->kind) {

	case TOKEN_INT:
		fprintf(log_file_syntactic, "Rule(RETURNED_TYPE->int)\n");
		return TOKEN_INT;

	case TOKEN_FLOAT:
		fprintf(log_file_syntactic, "Rule(RETURNED_TYPE->float)\n");
		return TOKEN_FLOAT;

	case TOKEN_VOID:
		fprintf(log_file_syntactic, "Rule(RETURNED_TYPE->void)\n");
		return TOKEN_VOID;

	default:
		fprintf(log_file_syntactic, "Expected token of type '{TOKEN_INT or TOKEN_FLOAT or TOKEN_VOID}', at line : {%d}. ", currToken->lineNumber);
		fprintf(log_file_syntactic, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
		currToken = back_token();
		errorRecovery(returnedTypeFollow, 1);
		break;
	}

	return TOKEN_ERROR;
}

void parseGlobalVars() {

	fprintf(log_file_syntactic, "Rule(GLOBAL_VARS->VAR_DEC GLOBAL_VARS')\n");
	// If lookahead is TOKEN_OPEN_PARENTHESES it means it's a function predefinition, do not parse predefinition from here.
	if (getLookAhead(3) == TOKEN_SEMICOLON) {
		currToken = next_token();
		parseVarDec();
		parseGlobalVarsTag();
	}

}

void parseGlobalVarsTag() {


	// If look ahead is semi colon, it means we are on global var and need to start parsing the same, else it's a function prototype, do not parse.

	if (getLookAhead(3) == TOKEN_SEMICOLON) {

		currToken = next_token();

		switch (currToken->kind) {

		case TOKEN_INT:
			fprintf(log_file_syntactic, "Rule(GLOBAL_VARS'->VAR_DEC GLOBAL_VARS')\n");
			parseVarDec();
			parseGlobalVarsTag();
			break;

		case TOKEN_FLOAT:
			fprintf(log_file_syntactic, "Rule(GLOBAL_VARS'->VAR_DEC GLOBAL_VARS')\n");
			parseVarDec();
			parseGlobalVarsTag();
			break;

		case TOKEN_OPEN_PARENTHESES: // if lookahead is ( than it means global vars has finished.
			fprintf(log_file_syntactic, "Rule(GLOBAL_VARS'->epsilon)\n");
			break;

		default:
			fprintf(log_file_syntactic, "Rule(GLOBAL_VARS'->epsilon)\n");
			currToken = back_token();
			break;
		}
	}
}

void parseVarDec() {

	// My strategy is to create a temp TableEntry holding all the information the next parsing holds. once finished with parsing i'd check if the same lexeme is already in the table
	// If not i shall procceed entering that same entry to the currTable, else needs to print a proper message this lexeme has already been declared and can't be declared twice.
	// Initialize the Role to be variable here, if needs to update to a different role it should be updated later on
	TableEntry* tempEntry = getNewDummyEntry();
	TOKEN_TYPE tokenType = TOKEN_ERROR;
	int tempLine = 0;

	switch (currToken->kind) {

	case TOKEN_INT:
		fprintf(log_file_syntactic, "Rule(VAR_DEC->TYPE id VAR_DEC')\n");
		tokenType = parseType();
		match(TOKEN_ID);
		setEntryName(tempEntry, currToken->lexeme);
		setRole(tempEntry, VARIABLE);
		setType(tempEntry, TOKEN_INT);
		parseVarDecTag(tempEntry);
		break;

	case TOKEN_FLOAT:
		fprintf(log_file_syntactic, "Rule(VAR_DEC->TYPE id VAR_DEC')\n");
		tokenType = parseType();
		match(TOKEN_ID);
		setEntryName(tempEntry, currToken->lexeme);
		setRole(tempEntry, VARIABLE);
		setType(tempEntry, TOKEN_FLOAT);
		parseVarDecTag(tempEntry);
		break;

	default:
		fprintf(log_file_syntactic, "Expected token of type '{TOKEN_INT or TOKEN_FLOAT}', at line : {%d}. ", currToken->lineNumber);
		fprintf(log_file_syntactic, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
		currToken = back_token();
		errorRecovery(varDecFollow, 7);
		break;
	}

	// Checking if the same lexeme has been declared before, if so print proper message
	TableEntry* entryPtr = insert(currTable, getEntryName(tempEntry));
	validateIDWasNotDeclared(entryPtr, tempEntry, getEntryName(tempEntry), currToken->lineNumber);
}

void parseVarDecTag(TableEntry* tempEntry) {



	currToken = next_token();

	switch (currToken->kind) {

	case TOKEN_SEMICOLON:

		fprintf(log_file_syntactic, "Rule(VAR_DEC'->;)\n");
		break;

	case TOKEN_OPEN_BRACKETS:
		// In parse_var_dec i insert name to the table to be Role of VARIABLE. If i reach here, it means the same Role needs to be updated to a type of array of dimension size.
		fprintf(log_file_syntactic, "Rule(VAR_DEC'->[DIM_SIZES];)\n");
		// If we hit open brackets, it means the same entry Role is Array and needs to update the same entry
		setRole(tempEntry, ARRAY);
		parseDimSizes(tempEntry);

		match(TOKEN_CLOSE_BRACKETS);
		match(TOKEN_SEMICOLON);

		break;

	case TOKEN_OPEN_PARENTHESES:
		currToken = back_token(); //Else we are done with Global vars. no need to perform recovery		
		break;

	default:
		fprintf(log_file_syntactic, "Expected token of type '{TOKEN_SEMICOLON or TOKEN_OPEN_BRACKETS}', at line : {%d}. ", currToken->lineNumber);
		fprintf(log_file_syntactic, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
		currToken = back_token();
		errorRecovery(varDecTagFollow, 7);
		break;
	}
}

void parseDimSizes(TableEntry* currEntry) {

	currToken = next_token();
	// Initalize the tempDimension size to size of zero

	switch (currToken->kind) {

	case TOKEN_INT:
		fprintf(log_file_syntactic, "Rule(DIM_SIZES->int_num DIM_SIZES')\n");

		addDimension(currEntry, atoi(currToken->lexeme));
		parseDimSizesTag(currEntry);
		printf("\n");
		break;

	default:

		fprintf(log_file_syntactic, "Expected tokens of type '{TOKEN_INT_NUM}', at line : {%d}. ", currToken->lineNumber);
		fprintf(log_file_syntactic, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
		currToken = back_token();
		errorRecovery(dimSizesFollow, 1);
		break;
	}
}

// this parser hold reference to the TableEntry, just so it can let parse_dim_sizes there's one more dimension to add
void parseDimSizesTag(TableEntry* tempEntry) {

	currToken = next_token();

	switch (currToken->kind) {

	case TOKEN_COMMA:

		fprintf(log_file_syntactic, "DIM_SIZES'-> ,DIM_SIZES\n");
		parseDimSizes(tempEntry);
		break;

	case TOKEN_CLOSE_BRACKETS:
		fprintf(log_file_syntactic, "DIM_SIZES'-> epsilon\n");
		currToken = back_token();
		break;

	default:
		fprintf(log_file_syntactic, "Expected tokens of type '{TOKEN_COMMA} or {TOKEN_CLOSE_BRACKETS}', at line : {%d}. ", currToken->lineNumber);
		fprintf(log_file_syntactic, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
		currToken = back_token();
		errorRecovery(dimSizesTagFollow, 1);
		break;

	}
}

TOKEN_TYPE parseType() {

	switch (currToken->kind) {

	case TOKEN_INT:
		fprintf(log_file_syntactic, "Rule(TYPE->int)\n");
		return TOKEN_INT;

	case TOKEN_FLOAT:
		fprintf(log_file_syntactic, "Rule(TYPE->float)\n");
		return TOKEN_FLOAT;

	default:
		fprintf(log_file_syntactic, "Expected token of type '{TOKEN_INT or TOKEN_FLOAT}', at line : {%d}. ", currToken->lineNumber);
		fprintf(log_file_syntactic, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
		currToken = back_token();
		errorRecovery(typeFollow, 1);
		// Since the 3rd stage is bug free from the syntax perspective, i do not expect to reach here, hence returning NULL so the program compile
		break;
	}
	// Since the 3rd stage is bug free from the syntax perspective, i do not expect to reach here, hence returning ERROR so the program compile
	return TOKEN_ERROR;
}

void parseFuncFullDefs() {

	fprintf(log_file_syntactic, "Rule(FUNC_FULL_DEFS->FUNC_WITH_BODY FUNC_FULL_DEFS')\n");
	parseFuncWithBody();
	parseFuncFullDefsTag();
}

void parseFuncFullDefsTag() {

	currToken = next_token();
	if (currToken->kind != TOKEN_EOF) {
		fprintf(log_file_syntactic, "Rule(FUN_FUL_DEFS'->FUNC_FULL_DEFS) \n");
		currToken = back_token();
		parseFuncFullDefs();
	}
	else fprintf(log_file_syntactic, "Rule(FUN_FUL_DEFS'->epsilon)\n");
}

void parseFuncWithBody() {

	fprintf(log_file_syntactic, "Rule(FUNC_WITH_BODY->FUNC_PROTOTYPE COMP_STMT)\n");
	TableEntry* tempEntry = parseFuncPrototype();
	setRole(tempEntry, FULL_DEF_FUNC);
	currFullFuncName = getEntryName(tempEntry);
	addFuncEntry(tempEntry, mainTable);
	parseCompStmt(FULL_DEF_FUNC);
}

void printDuplicateName(char* name, int lineNumber) {

	fprintf(log_file_semantic, duplicateErrorMsg, lineNumber, name);
}

void addFuncEntry(TableEntry* tempEntry, Table* table) {

	TableEntry* entryPtr = find(table, getEntryName(tempEntry));

	// If entryPtr is NULL it means the entry is not in the table
	if (entryPtr == NULL) {

		entryPtr = insert(table, getEntryName(tempEntry));
		copyEntryValuesToRealEntry(tempEntry, entryPtr);
		if (getRole(entryPtr) == FULL_DEF_FUNC)
			createParametrsTable(getFuncSignature(entryPtr));
	}
	// Else needs to check if the previous inserted entry is of type PRE_DEF while the new entry is of type FUL_DEF, if so it's okay to insert the new ptr to the table
	else if (getRole(tempEntry) == FULL_DEF_FUNC && getRole(entryPtr) == PRE_DEF_FUNC) {

		checkMatchingReturnType(getType(entryPtr), getType(tempEntry));
		// 1)check if signature match, print an error if not
		checkMatchingSignature(getFuncSignature(entryPtr), getFuncSignature(tempEntry));
		// 2) Set previous entry role to FUN_FULL_DEF
		setRole(entryPtr, FULL_DEF_FUNC);
		// 3) Set entry signature to be the full_def_one
		setFuncSignature(entryPtr, getFuncSignature(tempEntry));
		// 4)based on the second approach, create a new Table contains all the full function parameters
		createParametrsTable(getFuncSignature(tempEntry));
	}
	else {
		printDuplicateName(getEntryName(tempEntry), currToken->lineNumber);
	}
	TableEntry* entry = find(table, getEntryName(tempEntry));
}

void createParametrsTable(TableEntry* funcSignature) {
	// Simply creating a new table with all the parameters of the full function definition
	Table* tempTable = makeTable(currTable);
	TableEntry* temp = funcSignature;
	currTable = tempTable;
	while (temp != NULL) {
		TableEntry* entryPtr = insert(currTable, getEntryName(temp));

		validateIDWasNotDeclared(entryPtr, temp, temp->entryName, currToken->lineNumber);
		if (entryPtr != NULL)
			// If entryPtr is not NULL it means the item has naver been declared before and inserted properly to the table.
			copyEntryValuesToRealEntry(temp, entryPtr);
		temp = temp->next;
	}

}

void checkMatchingSignature(TableEntry* preDefSignature, TableEntry* fullDefSignature) {
	// Prints an error if the functions signature do not match

	int predefSize = getListSize(preDefSignature);
	int fulldefSize = getListSize(fullDefSignature);
	if (predefSize != fulldefSize) {
		fprintf(log_file_semantic, missMatchSignatureSize, currToken->lineNumber, predefSize, fulldefSize);
	}

	int minSize = predefSize <= fulldefSize ? predefSize : fulldefSize;

	TableEntry* predDefTemp = preDefSignature;
	TableEntry* fullDefTemp = fullDefSignature;

	for (int i = 0; i < minSize; i++) {
		TOKEN_TYPE preDefType = predDefTemp->type;
		TOKEN_TYPE fullDefType = fullDefTemp->type;

		if (preDefType != fullDefType) {

			fprintf(log_file_semantic, missMatchSignatureType,
				currToken->lineNumber, i, tokenEnumToString(getType(predDefTemp)), tokenEnumToString(getType(fullDefTemp)));

		}
		predDefTemp = predDefTemp->next;
		fullDefTemp = fullDefTemp->next;
	}
}


void checkMatchingReturnType(TOKEN_TYPE expectedToken, TOKEN_TYPE actualToken) {


	if (expectedToken != actualToken)
		fprintf(log_file_semantic, missMatchReturnType, currToken->lineNumber, tokenEnumToString(expectedToken), tokenEnumToString(actualToken));
}

void checkAllVariablesWereUsed() {

	TableEntry** entryArray = currTable->symbolsArray;

	for (int i = 0; i < SIZE; i++) {

		TableEntry* entry = entryArray[i];
		// First entry is a dummy one, hence iterating to the next one immidieatly
		entry = entry->next;
		while (entry != NULL) {
			if (getRole(entry) == VARIABLE || getRole(entry) == ARRAY) {
				if (getUsedEntry(entry) == FALSE)
					fprintf(log_file_semantic, notAllWasUsed, getEntryName(entry));
			}
			entry = entry->next;
		}
	}
}

TOKEN_TYPE getExpressionType(TOKEN_TYPE first, TOKEN_TYPE second) {


	if (first == TOKEN_ERROR || second == TOKEN_ERROR)
		return TOKEN_ERROR;

	else if (first == TOKEN_FLOAT || second == TOKEN_FLOAT)
		return TOKEN_FLOAT;
	else
		return TOKEN_INT;
}

void checkFunctionArgumentCall(ValueType* functionCaller, TableEntry* entry, int lineNumber) {


	Role role = getRole(entry);

	// First check there was a call to a function.
	if (role != FULL_DEF_FUNC && role != PRE_DEF_FUNC)
		fprintf(log_file_semantic, "Expected a call to a function in line %d, got %s instead.\n", lineNumber, roleEnumToString(role));
	// If it's not a function call, there is not function singature to check against.
	else {

		// Then check the size of each calls match
		TableEntry* functionSignature = getFuncSignature(entry);

		int functionCallerSize = getValueTypeListSize(functionCaller);

		int functionSignatureSize = getListSize(functionSignature);

		if (functionCallerSize != functionSignatureSize)
			fprintf(log_file_semantic, missMatchFunctionCall, lineNumber, functionSignatureSize, functionCallerSize);
		// Finally check for each of the argument type againt the type of the signature
		int min = functionCallerSize <= functionSignatureSize ? functionCallerSize : functionSignatureSize;

		for (int i = 0; i < min; i++) {

			if (functionCaller->type == TOKEN_ERROR)
				fprintf(log_file_semantic, "An error occured in line %d, called to function with argument of type TOKEN_ERROR\n", lineNumber);
			else if (functionCaller->type != functionSignature->type) {

				if (functionCaller->type == TOKEN_FLOAT)
					fprintf(log_file_semantic, "An error occured in line %d, can't cast from float to int\n", lineNumber);
			}
			functionCaller = functionCaller->next;
			functionSignature->next;
		}
	}
}

void checkArrayAccess(ValueType* valueTypeList, TableEntry* entry, int lineNumber) {

	Role role = getRole(entry);
	if (role != ARRAY)
		fprintf(log_file_semantic, "Expected a call to TOKEN_ARRAY type in line %d, but got %s instead\n", lineNumber, roleEnumToString(role));

	else {
		// If the id called is indeed array, we'd like to check the dimension calls.
		dimensionElem* dimension = getDimensions(entry);
		ValueType* temp = valueTypeList;

		int valueTypeListSize = valueTypeList->size;

		int dimensionSize = dimension->size;


		if (dimension->size != valueTypeListSize)
			fprintf(log_file_semantic, "Expected dimension size of %d but got %d instead at line %d\n", dimensionSize, valueTypeListSize, lineNumber);
		int min = dimensionSize <= valueTypeListSize ? dimensionSize : valueTypeListSize;
		//printf("%d %d", dimensionSize, valueTypeListSize);
		for (int i = 0; i < min; i++) {

			if (temp->type != TOKEN_INT) {

				fprintf(log_file_semantic, "Expected all values to array call to be of type TOKEN_INT_NUM at line %d, got %s instead\n", lineNumber, tokenEnumToString(temp->type));

				break;
			}

			if (temp->value >= dimension->dimension)
				fprintf(log_file_semantic, "The call to array at line %d, exceeds the %dth array dimension. Expected to be less than %d, but got %d instead\n", lineNumber, i, dimension->dimension, temp->value);
			temp = temp->next;
			dimension = dimension->next;
		}
	}
}

void onCloseCurly() {
	// All the operations needed on close curly will be called from here
	// Check if all variable in same scope has been used
	checkAllVariablesWereUsed();
	// Remove table once finished parsing same table
	currTable = popTable(currTable);
}

void checkReturnStatement() {
	// Check if return statement was used properly

	TableEntry* currEntry = find(currTable, currFullFuncName);
	TOKEN_TYPE returnType = currEntry->type;
	int returnState = getHasReturn(currEntry);
	if (returnType != TOKEN_VOID && returnState == FALSE)
		fprintf(log_file_semantic, "Error occured at function %s, missing return statement", getEntryName(currEntry));
}

void checkValidId(TableEntry* entry, int lineNumber, char* name) {

	if (entry == NULL)
		fprintf(log_file_semantic, undeclaredId, lineNumber, name);
}

void checkIdAssigment(TOKEN_TYPE type, TableEntry* entry, int lineNumber) {

	Role entryRole = getRole(entry);
	TOKEN_TYPE entryType = getType(entry);
	// Assigment to non variable is forbidden
	if (entryRole != VARIABLE) {
		fprintf(log_file_semantic, unsupportedAssigmentRole, lineNumber, roleEnumToString(entryRole));
	}
	else if (type == TOKEN_ERROR)
		fprintf(log_file_semantic, unknownRightValue, lineNumber);
	else if (type != getType(entry)) {

		// Assigning int to float is allowed, the opposite isn't
		if (entryType == TOKEN_INT && type == TOKEN_FLOAT)
			fprintf(log_file_semantic, intLeftFloatRight, lineNumber);
	}
}

void checkArrayElemAssignment(TOKEN_TYPE type, TableEntry* entry, int lineNumber) {

	TOKEN_TYPE entryType = getType(entry);
	if (entry == TOKEN_INT && type == TOKEN_FLOAT)
		fprintf(log_file_semantic, intLeftFloatRight, lineNumber);
	else if (type == TOKEN_ERROR)
		fprintf(log_file_semantic, unknownRightValue, lineNumber);
}

void validateIDWasNotDeclared(TableEntry* currEntry, TableEntry* tempEntry, char* name, int lineNumber) {

	if (currEntry == NULL) {
		// If entryPtrs is NULL it means the same name has been declared in the same scope, we have a duplicate
		fprintf(log_file_semantic, duplicateErrorMsg, lineNumber, name);
	}
	else {
		// Else needs to copy the values of tempEntry values to the entry that is inside the current table

		copyEntryValuesToRealEntry(tempEntry, currEntry);
	}
}

TOKEN_TYPE getLookAhead(int lookAhead) {

	TOKEN_TYPE tokenType;
	for (int i = 0; i < lookAhead; i++)
		currToken = next_token();

	tokenType = currToken->kind;
	performLexicalGoBack(lookAhead);
	return tokenType;
}

void performLexicalGoBack(int goBack) {

	for (int i = 0; i < goBack; i++) {

		currToken = back_token();
	}
}

void onCloseMainTable() {

	checkAllVariablesWereUsed();
	checkRemaningPredef();
}

void checkRemaningPredef() {

	TableEntry** entryArray = mainTable->symbolsArray;
	for (int i = 0; i < SIZE; i++) {

		TableEntry* entry = entryArray[i];
		// First entry is a dummy one, hence iterating to the next one immidieatly
		entry = entry->next;
		while (entry != NULL) {
			if (getRole(entry) == PRE_DEF_FUNC) {
				fprintf(log_file_semantic, "An error occured, there is a predefinition function with no full definition implementation. name:%s \n", getEntryName(entry));
			}
			entry = entry->next;
		}
	}
}
void checkReturnValue(TableEntry* currEntry) {

	TOKEN_TYPE expectedReturn = currEntry->type;
	TOKEN_TYPE actualReturn = currEntry->actualReturnType;
	if (expectedReturn != actualReturn) {
		fprintf(log_file_semantic, "Error occured at line %d, unexpected return value. Expected %s, got %s\n", currToken->lineNumber, tokenEnumToString(expectedReturn), tokenEnumToString(actualReturn));
	}
}