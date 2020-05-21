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
		fprintf(log_file, "Expected token of type '{%s}', at line : {%d}. ", tokenEnumToString(token_type), currToken->lineNumber);
		fprintf(log_file, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
	}
}

void errorRecovery(eTOKENS follow[], int size) {

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

void parse_program() {
	
	restartTokenizer();
	int counter = 0;
	fprintf(log_file, "Rule(PROG->GLOBAL_VARS FUNC_PREDEFS FUNC_FULL_DEFS)\n");
	parse_global_vars();
	fprintf(log_file, "Rule(FUNC_PREDEFS->FUNC_PROTOTYPE; FUNC_PREDEFS')\n");
	do {
		// Just for a nice printing
		if (counter > 0) fprintf(log_file, "Rule(FUNC_PREDEFS'->FUNC_PROTOTYPE; FUNC_PREDEFS')\n");
		parse_func_prototype();
		// Check if ends with semicolon
		currToken = next_token();
		// Just for nice printing. 
		if (currToken->kind == TOKEN_OPEN_CURLY) {
			if (counter > 0); // if counter is greater than zero and we meet open curly than it start func full predefs correctly. do nothing...

			else {
				// If counter is zero and we meet token open curly than missing functions predefs
				//fprintf(log_file, "Missing functions predefinitions....\n");
				currToken = back_token();
				match(TOKEN_SEMICOLON);
			}		
		}
		else {
			currToken = back_token();
			match(TOKEN_SEMICOLON);
		}
		counter++;
	} while (currToken->kind == TOKEN_SEMICOLON);		

	fprintf(log_file, "Rule(FUNC_PREDEFS'->epsilon)\n");
	fprintf(log_file, "Rule(FUNC_FULL_DEFS->FUNC_WITH_BODY FUNC_FULL_DEFS')\n");
	if (currToken->kind == TOKEN_OPEN_CURLY) {
		fprintf(log_file, "Rule(FUNC_WITH_BODY->FUNC_PROTOTYPE COMP_STMT)\n");
		currToken = back_token();        
		parse_comp_stmt();       
		currToken = next_token();

		if (currToken->kind != TOKEN_EOF) {  
			currToken = back_token();     
			parse_func_full_defs();
		}
		else {
			currToken = back_token();
			fprintf(log_file, "Rule(FUN_FUL_DEFS'-> epsilon)\n");
		}
	}
	else {
		fprintf(log_file, "Expected token of type '{TOKEN_OPEN_CURLY}', at line : {%d}. ", currToken->lineNumber);
		fprintf(log_file, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind, currToken->lexeme));
		while (currToken->kind != TOKEN_EOF) currToken = next_token();
		
		currToken = back_token();
	}
	match(TOKEN_EOF);
}

void parse_comp_stmt() {

	
	currToken = next_token();
	switch (currToken->kind) {

	case TOKEN_OPEN_CURLY:
		fprintf(log_file, "Rule(COMP_STMT->{ VAR_DEC_LIST STMT_LIST })\n");
		parse_var_dec_list();
		parse_stmt_list();
		match(TOKEN_CLOSE_CURLY);
		break;

	default:
		fprintf(log_file, "Expected token of type '{TOKEN_OPEN_CURLY}', at line : {%d}. ", currToken->lineNumber);
		fprintf(log_file, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
		currToken = back_token();
		errorRecovery(paramTagFollow, 6);
		break;
	}
}

void parse_stmt_list() {

	fprintf(log_file, "Rule(STMT_LIST->STMT STMT_LIST')\n");
	parse_stmt();
	parse_stmt_list_tag();
}

void parse_stmt_list_tag() {

	currToken = next_token();

	switch (currToken->kind) {

	case TOKEN_SEMICOLON:

		currToken = next_token();
		fprintf(log_file, "Rule(STMT_LIST'->;STMT STMT_LIST')\n");
		if (currToken->kind != TOKEN_CLOSE_CURLY  && currToken->kind != TOKEN_EOF) {
			currToken = back_token();
			parse_stmt();
			parse_stmt_list_tag();
		}
		else {
			// If currToken is close curly than we have finished with this func full defs. may finish the process.
			fprintf(log_file, "Rule(STMT_LIST'->epsilon)\n");
			currToken = back_token();
		}
		break;

	default:
		fprintf(log_file, "Rule(STMT_LIST'->epsilon)\n");
		currToken = back_token();
		break;
	}

}

void parse_stmt() {

	currToken = next_token();

	switch (currToken->kind) {

	case TOKEN_ID:
		fprintf(log_file, "Rule(STMT->id STMT')\n");
		parse_stmt_tag();
		break;

	case TOKEN_IF:
		fprintf(log_file, "Rule(STMT->if(CONDITION)STMT)\n");
		match(TOKEN_OPEN_PARENTHESES);
		parse_condition();
		match(TOKEN_CLOSE_PARENTHESES);
		parse_stmt();
		break;

	case TOKEN_RETURN:
		fprintf(log_file, "Rule(STMT->RETURN_STMT\n");
		parse_return_stmt();
		break;

	case TOKEN_OPEN_CURLY:
		fprintf(log_file, "Rule(STMT->COMP_STMT\n");
		currToken = back_token();
		parse_comp_stmt();
		break;

	default:
		fprintf(log_file, "Expected token of type '{TOKEN_OPEN_CURLY or TOKEN_ID or TOKEN_IF or TOKEN_RETURN}', at line : {%d}. ", currToken->lineNumber);
		fprintf(log_file, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
		currToken = back_token();
		errorRecovery(stmtFollow, 2);
		break;
	}
}

void parse_stmt_tag() {

	currToken = next_token();
	switch (currToken->kind) {

	case TOKEN_OPEN_BRACKETS:
		fprintf(log_file, "Rule(STMT'->[EXPR_LIST]=EXPR)\n");
		parse_expr_list();
		match(TOKEN_CLOSE_BRACKETS);
		match(TOKEN_OP_ASSIGNMENT);
		parse_expr();
		break;

	case TOKEN_OPEN_PARENTHESES:
		fprintf(log_file, "Rule(STMT'->(ARGS))\n");
		parse_args();
		match(TOKEN_CLOSE_PARENTHESES);
		break;
		
	case TOKEN_OP_ASSIGNMENT:
		fprintf(log_file, "Rule(STMT'-> = EXPR)\n");
		parse_expr();
		break;

	default:
		fprintf(log_file, "Expected token of type '{TOKEN_OPEN_PARENTHESES or TOKEN_OPEN_BRACKETS or TOKEN_OP_ASSIGNMENT}', at line : {%d}. ", currToken->lineNumber);
		fprintf(log_file, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
		currToken = back_token();
		errorRecovery(stmtTagFollow, 2);
		break;
	}

}

void parse_args() {

	currToken = next_token();
	switch (currToken->kind) {

	case TOKEN_ID:
		fprintf(log_file, "Rule(ARGS->ARG_LIST)\n");
		currToken = back_token();
		parse_arg_list();
		break;

	case TOKEN_INT_NUM:
		fprintf(log_file, "Rule(ARGS->ARG_LIST)\n");
		currToken = back_token();
		parse_arg_list();
		break;

	case TOKEN_FLOAT_NUM:
		fprintf(log_file, "Rule(ARGS->ARG_LIST)\n");
		currToken = back_token();
		parse_arg_list();
		break;

	case TOKEN_OPEN_PARENTHESES:
		fprintf(log_file, "Rule(ARGS->ARG_LIST)\n");
		currToken = back_token();
		parse_arg_list();
		break;

	default:
		fprintf(log_file, "Rule(ARGS->epsilon)\n");
		currToken = back_token();
		break;
	}
}

void parse_arg_list() {

	currToken = next_token();
	switch (currToken->kind) {

	case TOKEN_ID:
		fprintf(log_file, "Rule(ARG_LIST->EXPR)\n");
		currToken = back_token();
		parse_expr();
		parse_arg_list_tag();
		break;

	case TOKEN_INT_NUM:
		fprintf(log_file, "Rule(ARG_LIST->EXPR)\n");
		currToken = back_token();
		parse_expr();
		parse_arg_list_tag();
		break;

	case TOKEN_FLOAT_NUM:
		fprintf(log_file, "Rule(ARG_LIST->EXPR)\n");
		currToken = back_token();
		parse_expr();
		parse_arg_list_tag();
		break;

	case TOKEN_OPEN_PARENTHESES:
		fprintf(log_file, "Rule(ARG_LIST->EXPR)\n");
		currToken = back_token();
		parse_expr();
		parse_arg_list_tag();
		break;

	default:
		fprintf(log_file, "Expected token of type '{TOKEN_ID or TOKEN_INT_NUM or TOKEN_FLOAT_NUM or TOKEN_OPEN_PARENTHESES}', at line : {%d}. ", currToken->lineNumber);
		fprintf(log_file, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
		currToken = back_token();
		errorRecovery(argListFollow, 1);
		break;
	}
}

void parse_arg_list_tag() {

	currToken = next_token();

	switch (currToken->kind) {

	case TOKEN_COMMA:
		fprintf(log_file, "Rule(ARG_LIST'->,EXPR ARG_LIST')\n");
		parse_expr();
		parse_arg_list_tag();
		break;

	default:
		fprintf(log_file, "Rule(ARG_LIST'->epsilon)\n");
		currToken = back_token();
		break;
	}
}

void parse_expr() {

	fprintf(log_file, "Rule(EXPR->TERM EXPR')\n");
	parse_term();
	parse_expr_tag();
}

void parse_expr_tag() {

	currToken = next_token();
	switch (currToken->kind) {

	case TOKEN_OP_ADD:
		fprintf(log_file, "Rule(EXPR'->+TERM EXPR')\n");
		parse_term();
		parse_expr_tag();
		break;

	default:
		fprintf(log_file, "Rule(EXPR'->epsilon)\n" );
		currToken = back_token();
		break;
	}
}

void parse_term() {

	fprintf(log_file, "Rule(TERM->FACTOR TERM')\n");
	parse_factor();
	parse_term_tag();
}

void parse_term_tag() {

	currToken = next_token();
	switch (currToken->kind) {

	case TOKEN_OP_MUL:
		fprintf(log_file, "Rule(TERM'->*FACTOR TERM')\n");
		parse_factor();
		parse_term_tag();
		break;

	default:
		
		currToken = back_token();
		fprintf(log_file, "Rule(TERM'->epsilon)\n");
		break;
	}
}

void parse_factor() {

	currToken = next_token();
	switch (currToken->kind) {

	case TOKEN_ID:
		fprintf(log_file, "Rule(FACTOR-> id FACTOR')\n");
		parse_factor_tag();
		break;

	case TOKEN_INT_NUM:
		fprintf(log_file, "Rule(FACTOR-> int_num)\n");
		break;

	case TOKEN_FLOAT_NUM:
		fprintf(log_file, "Rule(FACTOR-> float_num)\n");
		break;

	case TOKEN_OPEN_PARENTHESES:
		fprintf(log_file, "Rule(FACTOR-> (EXPR))\n");
		parse_expr();
		match(TOKEN_CLOSE_PARENTHESES);
		break;
	default:
		fprintf(log_file, "Expected tokens of type '{TOKEN_ID or TOKEN_INT_NUM or TOKEN_FLOAT_NUM or TOKEN_OPEN_PARENTHESES}', at line : {%d}. ", currToken->lineNumber);
		fprintf(log_file, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
		currToken = back_token();
		errorRecovery(factorFollow, 13);
		break;
	}
}

void parse_factor_tag() {

	
	currToken = next_token();
	switch (currToken->kind) {

	case TOKEN_OPEN_PARENTHESES:
		fprintf(log_file, "Rule(FACTOR'-> (ARGS))\n");
		parse_args();
		match(TOKEN_CLOSE_PARENTHESES);
		break;

	case TOKEN_OPEN_BRACKETS:
		fprintf(log_file, "Rule(FACTOR'-> [EXPR_LIST])\n");
		parse_expr_list();
		match(TOKEN_CLOSE_BRACKETS);
		break;

	default:		
		currToken = back_token();
		fprintf(log_file, "Rule(FACTOR'-> epsilon)\n");
		break;
	}
}

void parse_expr_list() {

	fprintf(log_file, "Rule(EXPR_LIST->EXPR EXPR_LIST')\n");
	parse_expr();
	parse_expr_list_tag();
}


void parse_expr_list_tag() {

	currToken = next_token();
	switch (currToken->kind) {
		
	case TOKEN_COMMA:
		fprintf(log_file, "Rule(EXPR_LIST'->, EXPR EXPR_LIST')\n");
		parse_expr();
		parse_expr_tag();
		break;

	default:
		fprintf(log_file, "Rule(EXPR_LIST'->epsilon)\n");
		currToken = back_token();
		break;
	}
}

void parse_return_stmt() {

	fprintf(log_file, "Rule(RETURN_STMT->return RETURN_STMT')\n");
	parse_return_stmt_tag();
}

void parse_return_stmt_tag() {

	currToken = next_token();
	switch (currToken->kind) {

	case TOKEN_ID:
		fprintf(log_file, "Rule(RETURN_STMT'->EXPR)\n");
		currToken = back_token();
		parse_expr();
		break;

	case TOKEN_INT_NUM:
		fprintf(log_file, "Rule(RETURN_STMT'->EXPR)\n");
		currToken = back_token();
		parse_expr();
		break;

	case TOKEN_FLOAT:
		fprintf(log_file, "Rule(RETURN_STMT'->EXPR)\n");
		currToken = back_token();
		parse_expr();
		break;

	case TOKEN_OPEN_PARENTHESES:
		fprintf(log_file, "Rule(RETURN_STMT'->EXPR)\n");
		currToken = back_token();
		parse_expr();
		break;

	default:
		fprintf(log_file, "Rule(RETURN_STMT'->epsilon)\n");
		currToken = back_token();
		break;
	}
}

void parse_condition() {

	fprintf(log_file, "Rule(CONDITION->EXPR rel_op EXPR)\n");
	parse_expr();
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
		fprintf(log_file, "Expected token of type 'rel_op', at line : {%d}. ", currToken->lineNumber);
		fprintf(log_file, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind, currToken->lexeme));
		break;
	}

	parse_expr();
}

void parse_var_dec_list() {

	fprintf(log_file, "Rule(VAR_DEC_LIST->VAR_DEC_LIST'))\n");
	parse_var_dec_list_tag();
}

void parse_var_dec_list_tag() {

	
	currToken = next_token();
	switch (currToken->kind) {

	case TOKEN_INT:
		fprintf(log_file, "Rule(VAR_DEC_LIST'->VAR_DEC VAR_DEC_LIST')\n");
		currToken = back_token();
		parse_var_dec(0);
		parse_var_dec_list_tag();
		break;

	case TOKEN_FLOAT:
		fprintf(log_file, "VAR_DEC_LIST'->VAR_DEC VAR_DEC_LIST')\n");
		currToken = back_token();
		parse_var_dec(0);
		parse_var_dec_list_tag();
		break;

	default:
		fprintf(log_file, "VAR_DEC_LIST'->VAR_DEC_LIST'->epsilon)\n");
		currToken = back_token();
		break;
	}
}

void parse_func_prototype() {

	fprintf(log_file, "Rule(FUNC_PROTOTYPE->RETURNED_TYPE id(PARAMS))\n");
	parse_returned_type();
	match(TOKEN_ID);
	match(TOKEN_OPEN_PARENTHESES);
	
	currToken = next_token();

	// if currentToken is close parentheses than the params is empty
	if (currToken->kind != TOKEN_CLOSE_PARENTHESES) {
		// then params is not empy
		currToken = back_token();
		parse_params();
		match(TOKEN_CLOSE_PARENTHESES);
	}
	else fprintf(log_file, "PARAMS->epsilon\n"); // else params is empty
}

void parse_params() {

	fprintf(log_file, "PARAMS->PARAM_LIST\n");
	parse_param_list();
}

void parse_param_list() {

	fprintf(log_file, "PARAM_LIST->PARAM PARAM_LIST'\n");
	parse_param();
	parse_param_list_tag();
}

void parse_param_list_tag() {

	currToken = next_token();
	switch (currToken->kind) {

	case TOKEN_COMMA:
		fprintf(log_file, "PARAM_LIST'->, PARAM PARAM_LIST'\n");
		parse_param();
		parse_param_list_tag();
		break;

	case TOKEN_CLOSE_PARENTHESES:
		fprintf(log_file, "PARAM_LIST'->epsilon\n");
		currToken = back_token();
		break;

	default:
		fprintf(log_file, "Expected token of type '{TOKEN_COMMA or TOKEN_CLOSE_PARENTHESES}', at line : {%d}. ", currToken->lineNumber);
		fprintf(log_file, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
		currToken = back_token();
		errorRecovery(paramsListTagFollow, 1);
		break;
	}
}

void parse_param() {

	fprintf(log_file, "PARAM->TYPE id PARAM'\n");
	parse_type();
	match(TOKEN_ID);
	parse_param_tag();
}

void parse_param_tag() {

	
	currToken = next_token();
	switch (currToken->kind) {

	case TOKEN_OPEN_BRACKETS:
		fprintf(log_file, "PARAM'->[DIM_SIZES]\n");
		parse_dim_sizes();
		match(TOKEN_CLOSE_BRACKETS);
		break;

	default:
		fprintf(log_file, "PARAM'->epsilon\n");
		currToken = back_token();
		break;
	}
}

void parse_returned_type() {

	fprintf(log_file, "Rule(RETURNED_TYPE->TYPE|void)\n");
	currToken = next_token();
	switch (currToken->kind) {

	case TOKEN_INT:
		fprintf(log_file, "Rule(RETURNED_TYPE->int)\n");
		break;

	case TOKEN_FLOAT:
		fprintf(log_file, "Rule(RETURNED_TYPE->float)\n");
		break;

	case TOKEN_VOID:
		fprintf(log_file, "Rule(RETURNED_TYPE->void)\n");
		break;

	default:
		fprintf(log_file, "Expected token of type '{TOKEN_INT or TOKEN_FLOAT or TOKEN_VOID}', at line : {%d}. ", currToken->lineNumber);
		fprintf(log_file, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
		currToken = back_token();
		errorRecovery(returnedTypeFollow, 1);
		break;
	}
}

void parse_global_vars() {

	fprintf(log_file, "Rule(GLOBAL_VARS->VAR_DEC GLOBAL_VARS')\n");
	parse_var_dec(1);
	parse_global_vars_tag();
	// if lookahead is ( than it means global vars has finished, needs to start func_predefs(hence back_token 3 times)
	if (currToken->kind == TOKEN_OPEN_PARENTHESES) {
		currToken = back_token();
		currToken = back_token();
		currToken = back_token();
	}
}

void parse_global_vars_tag() {

	
	currToken = next_token();
	switch (currToken->kind) {

	case TOKEN_INT:
		fprintf(log_file, "Rule(GLOBAL_VARS'->VAR_DEC GLOBAL_VARS')\n");
		currToken = back_token();
		parse_var_dec(0);
		parse_global_vars_tag();
		break;

	case TOKEN_FLOAT:
		fprintf(log_file, "Rule(GLOBAL_VARS'->VAR_DEC GLOBAL_VARS')\n");
		currToken = back_token();
		parse_var_dec(0);
		parse_global_vars_tag();
		break;

	case TOKEN_OPEN_PARENTHESES: // if lookahead is ( than it means global vars has finished.
		fprintf(log_file, "Rule(GLOBAL_VARS'->epsilon)\n");
		break;

	default:
		fprintf(log_file, "Rule(GLOBAL_VARS'->epsilon)\n");
		currToken = back_token();
		break;
	}
}

void parse_var_dec(int isFirstGlobalVar) {

	currToken = next_token();
	switch (currToken->kind) {
		
	case TOKEN_INT:
		fprintf(log_file, "Rule(VAR_DEC->TYPE id VAR_DEC')\n");
		currToken = back_token();
		parse_type();
		match(TOKEN_ID);
		parse_var_dec_tag(isFirstGlobalVar);
		break;
		
	case TOKEN_FLOAT:
		fprintf(log_file, "Rule(VAR_DEC->TYPE id VAR_DEC')\n");
		currToken = back_token();
		parse_type();
		match(TOKEN_ID);
		parse_var_dec_tag(isFirstGlobalVar);
		break;

	default:
		fprintf(log_file, "Expected token of type '{TOKEN_INT or TOKEN_FLOAT}', at line : {%d}. ", currToken->lineNumber);
		fprintf(log_file, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
		currToken = back_token();
		
		errorRecovery(varDecFollow, 7);
		break;
	}
}

void parse_var_dec_tag(int isFirstGlobalVar) {


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

	case TOKEN_OPEN_PARENTHESES:
		if (isFirstGlobalVar) {
			// If we meet open parenthese before the first global var was found than its an error(global var is not nullable!). so perform error recory
			fprintf(log_file, "Expected token of type '{TOKEN_SEMICOLON or TOKEN_OPEN_BRACKETS}', at line : {%d}. ", currToken->lineNumber);
			fprintf(log_file, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind, currToken->lexeme));
			currToken = back_token();
			errorRecovery(varDecTagFollow, 7);
		}
		else currToken = back_token(); //Else we are done with Global vars. no need to perform recovery
		
		break;

	default:
		fprintf(log_file, "Expected token of type '{TOKEN_SEMICOLON or TOKEN_OPEN_BRACKETS}', at line : {%d}. ", currToken->lineNumber);
		fprintf(log_file, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind, currToken->lexeme));
		currToken = back_token();
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
		fprintf(log_file, "Expected tokens of type '{TOKEN_INT_NUM}', at line : {%d}. ", currToken->lineNumber);
		fprintf(log_file, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
		currToken = back_token();
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
		fprintf(log_file, "DIM_SIZES'-> epsilon\n");
		currToken = back_token();
		break;

	default:
		fprintf(log_file, "Expected tokens of type '{TOKEN_COMMA} or {TOKEN_CLOSE_BRACKETS}', at line : {%d}. ", currToken->lineNumber);
		fprintf(log_file, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
		currToken = back_token();
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
		fprintf(log_file, "Expected token of type '{TOKEN_INT or TOKEN_FLOAT}', at line : {%d}. ", currToken->lineNumber);
		fprintf(log_file, "Actual token of type '{%s}', lexeme : {%s}\n", tokenEnumToString(currToken->kind), currToken->lexeme);
		currToken = back_token();
		errorRecovery(typeFollow, 1);
		break;
	}
}

void parse_func_full_defs() {

	fprintf(log_file, "Rule(FUNC_FULL_DEFS->FUNC_WITH_BODY FUNC_FULL_DEFS')\n");
	parse_func_with_body();
	parse_func_full_defs_tag();
}

void parse_func_full_defs_tag() {

	currToken = next_token();
	if (currToken->kind != TOKEN_EOF) {
		fprintf(log_file, "Rule(FUN_FUL_DEFS'->FUNC_FULL_DEFS) \n");
		currToken = back_token();
		parse_func_full_defs();
	}
	else fprintf(log_file, "Rule(FUN_FUL_DEFS'->epsilon)\n");
}

void parse_func_with_body() {

	fprintf(log_file, "Rule(FUNC_WITH_BODY->FUNC_PROTOTYPE COMP_STMT)\n");
	parse_func_prototype();
	parse_comp_stmt();
}