#ifndef MY_PARSER_H
#define MY_PARSER_H

#include "Token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static FILE* log_file = NULL;

static Token* currToken = NULL;

void startParsing(int);

void match(eTOKENS token);

void errorRecovery(eTOKENS follow[], int size);

void parse_program();

void parse_global_vars();

void parse_func_predefs();

void parse_func_full_defs();

void parse_global_vars_tag();

void parse_var_dec();

void parse_var_dec_tag();

void parse_type();

void parse_dim_sizes();

void parse_dim_sizes_tag();

void parse_func_predefs();

void parse_func_predefs_tag();

void parse_func_prototype();

void parse_func_with_body();

void parse_returned_type();

void parse_params();

void parse_param_list();

void parse_param_list_tag();

void parse_param();

void parse_param_tag();

void parse_comp_stmt();

void parse_var_dec_list();

void parse_var_dec_list_tag();

void parse_stmt_list();

void parse_stmt_list_tag();

void parse_stmt();

void parse_stmt_tag();

void parse_args();

void parse_arg_list();

void parse_arg_list_tag();

void parse_return_stmt();

void parse_return_stmt_tag();

void parse_expr_list();

void parse_expr_list_tag();

void parse_condition();

void parse_expr();

void parse_expr_tag();

void parse_term();

void parse_term_tag();

void parse_factor();

void parse_factor_tag();

#endif
