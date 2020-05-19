#include "MyParser.h"
#include "Follow.h"


void startParsing(int fileToOpen) {

	if (fileToOpen == 1) log_file = fopen("C:\\temp\\test1_204371454_205838618__syntactic.txt", "w");

	else log_file = fopen("C:\\temp\\test2_204371454_205838618__syntactic.txt", "w");


	if (log_file == NULL) printf("Unable to open syntactic file for write, closing program...\n");

	else parse_program();
}

void match(eTOKENS token_type) {

	currToken = next_token();

	if (currToken->kind != token_type) {
		//TODO change to proper print of expetecToken.type and token.type
		fprintf(log_file, "------------------------------------------------------------------------\n");
		fprintf(log_file, "Expected token of type '{%s}', at line : {%d}\n", tokenEnumToString(token_type), currToken->lineNumber);
		fprintf(log_file, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind, currToken->lexeme));
		fprintf(log_file, "------------------------------------------------------------------------\n");
	}
}

void errorRecovery(eTOKENS follow[], int size) {

	int found = 0;
	
	while (!found) {
		currToken = next_token();
		for (int i = 0; i < size; i++) {

			if (currToken->kind == TOKEN_EOF || currToken == follow[i]) {
				found = 1;
				currToken = back_token();
				break;
			}
		}
	}
}


//void parse_program(int fileToOpen) {
//
//	// First time opening output file
//	if (log_file == NULL) {
//		log_file = fopen("C:\\temp\\test1_204371454_205838618__syntactic.txt", "w");
//	}
//	// If file is still null than opening the file has failed
//	if (log_file == NULL) {
//		printf("Failed to open syntatic log file");
//	}
//	fprintf(log_file, "Rule(PROG->GLOBAL_VARS FUNC_PREDEFS FUNC_FULL_DEFS)\n");
//	parse_global_vars();
//	//parse_func_predefs();
//	//parse_func_full_defs();
//	match(TOKEN_EOF);
//}

void parse_program() {
	
	fprintf(log_file, "Rule(PROG->GLOBAL_VARS FUNC_PREDEFS FUNC_FULL_DEFS)\n");
	parse_global_vars();
	do {
		fprintf(log_file, "Rule(FUNC_PREDEFS->FUNC_PROTOTYPE; FUNC_PREDEFS')\n");
		parse_func_prototype();
		match(TOKEN_SEMICOLON);

	} while (currToken->kind == TOKEN_SEMICOLON);		

	if (currToken->kind == TOKEN_OPEN_CURLY) {
		currToken = back_token();        
		parse_comp_stmt();       
		currToken = next_token();

		if (currToken->kind != TOKEN_EOF) {  
			currToken = back_token();     
			parse_func_full_defs();
		}
		else currToken = back_token();			
	}
	else {
		fprintf(log_file, "Expected token of type '{TOKEN_OPEN_CURLY}', at line : {%d}\n", currToken->lineNumber);
		fprintf(log_file, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind, currToken->lexeme));
		while (currToken->kind != TOKEN_EOF) currToken = next_token();
		
		currToken = back_token();
	}
	match(TOKEN_EOF);
}

void parse_func_prototype() {

	fprintf(log_file, "Rule(FUNC_PROTOTYPE->RETURNED_TYPE id(PARAMS))\n");
	parse_returned_type();
	match(TOKEN_ID);
	match(TOKEN_OPEN_PARENTHESES);
	parse_params();
	match(TOKEN_CLOSE_PARENTHESES);
}

void parse_params() {

	fprintf(log_file, "PARAMS->PARAM_LIST|epsilon\n");
	parse_param_list();
}

void parse_param_list() {

	fprintf(log_file, "PARAM_LIST->PARAM PARAM_LIST'\n");
	parse_param();
	parse_param_list_tag();
}

void parse_param_list_tag() {

	while (currToken->kind == TOKEN_COMMA) { //If parse_param() ended with comma then another parse params is needed

	}
}

void parse_param() {

	fprintf(log_file, "PARAM->TYPE id PARAM'\n");
	parse_type();
	match(TOKEN_ID);
	parse_param_tag();
}

void parse_param_tag() {

	fprintf(log_file, "PARAM'->epsilon|[DIM_SIZES]\n");
	match(TOKEN_OPEN_BRACKETS);
	parse_dim_sizes();
	match(TOKEN_CLOSE_BRACKETS);
}

void parse_returned_type() {

	fprintf(log_file, "Rule(RETURNED_TYPE->TYPE|void)\n");
	currToken = next_token();
	switch (currToken->kind) {

	case TOKEN_INT:
		break;

	case TOKEN_FLOAT:
		break;

	case TOKEN_VOID:
		break;

	default:
		errorRecovery(returnedTypeFollow, 1);
		break;
	}
}

void parse_global_vars() {

	fprintf(log_file, "Rule(GLOBAL_VARS->VAR_DEC GLOBAL_VARS')\n");
	parse_var_dec();
	if (currToken->kind == TOKEN_SEMICOLON) parse_global_vars_tag(); // if ended with semi colon than needs to keep parsing global vars

	else if (currToken->kind == TOKEN_OPEN_PARENTHESES) {
		// If lookahead is open parentheses, than no global var has been decleared. I shall back token twice to continue with FUNC_PREDEFS.
		fprintf(log_file, "Missing global vars decleration.");
		currToken = back_token();
		currToken = back_token();
	}
}

void parse_global_vars_tag() {

	
	while (currToken->kind == TOKEN_SEMICOLON) { // If parse_var_dec finished with semi colon than a new global var is ahead of us.
		fprintf(log_file, "Rule(GLOBAL_VARS'->VAR_DEC GLOBAL_VARS')\n");
		parse_var_dec();
	}

	if (currToken->kind == TOKEN_OPEN_PARENTHESES) {
		//Same reason as parse_global_vars
		currToken = back_token();
		currToken = back_token();
	}
}

void parse_var_dec() {

	parse_type();
	match(TOKEN_ID);
	parse_var_dec_tag();
}

void parse_var_dec_tag() {


	currToken = next_token();

	switch (currToken->kind) {

	case TOKEN_SEMICOLON:
		fprintf(log_file, "Rule(VAR_DEC'->;)\n");
		break;

	case TOKEN_OPEN_BRACKETS:
		fprintf(log_file, "Rule(VAR_DEC'->[DIM_SIZES];)\n");
		parse_dim_sizes();
		match(TOKEN_CLOSE_BRACKETS);
		match(TOKEN_SEMICOLON);
		break;

	case TOKEN_OPEN_PARENTHESES: // if lookahead is ( than it means global vars has finished, needs to start func_predefs(hence back_token 2 times)
		break;

	default:
		fprintf(log_file, "Expected token of type '{TOKEN_SEMICOLON or TOKEN_OPEN_CRACKETS}', at line : {%d}\n", currToken->lineNumber);
		fprintf(log_file, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind, currToken->lexeme));
		errorRecovery(varDecTagFollow, 7);
		break;
	}
}

void parse_dim_sizes() {

	currToken = next_token();

	switch (currToken->kind) {

	case TOKEN_INT_NUM:
		fprintf(log_file, "Rule(DIM_SIZES->int_num DIM_SIZES')\n");
		parse_dim_sizes_tag();
		break;

	default:
		// TOOD add working recovery
		fprintf(log_file, "Expected tokens of type '{TOKEN_INT_NUM or TOKEN_CLOSE_BRACKETS}', at line : {%d}\n", currToken->lineNumber);
		fprintf(log_file, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
		errorRecovery(dimSizesFollow, 1);
		break;
	}
}

void parse_dim_sizes_tag() {

	currToken = next_token();

	switch (currToken->kind) {

	case TOKEN_COMMA:
		fprintf(log_file, "DIM_SIZES'-> ,DIM_SIZES\n");
		parse_dim_sizes();
		break;

	case TOKEN_CLOSE_BRACKETS:
		fprintf(log_file, "DIM_SIZES'->epsilon\n");
		currToken = back_token();
		break;

	case TOKEN_EOF:
		currToken = back_token();
		break;

	default:
		fprintf(log_file, "Expected token of type '{TOKEN_COMMA}', at line : {%d}\n", currToken->lineNumber);
		fprintf(log_file, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
		errorRecovery(dimSizesTagFollow, 1);
		break;

	}
}

void parse_type() {

	currToken = next_token();
	switch (currToken->kind) {

	case TOKEN_INT:
		
		fprintf(log_file, "Rule(TYPE->int)\n");
		break;

	case TOKEN_FLOAT:
		fprintf(log_file, "Rule(TYPE->float)\n");
		break;

	default:
		fprintf(log_file, "Expected token of type '{TOKEN_INTEGER or TOKEN_FLOAT}', at line : {%d}\n", currToken->lineNumber);
		fprintf(log_file, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
		errorRecovery(typeFollow, 1);
		break;
	}
}

void parse_func_predefs() {

}

void parse_func_full_defs() {

}