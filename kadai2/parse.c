/**
 * @file parse.c
 * @brief 簡略化EBNFに基づく再帰下降構文解析器 (1ルール1関数対応版)
 */
#include "parse.h"

/* scan.c で定義されている文字列バッファ */
// extern char string_attr[];

/* グローバル変数 */
int token;

/* プリティプリント制御用 */
static int indent_level  = 0;
static int is_newline    = 1;
static int space_pending = 0;

/* プロトタイプ宣言 (EBNFの ::= の数だけ定義: 全16関数) */
int        parse_program(void);
static int parse_block(void);
static int parse_variable_declaration(void);
static int parse_variable_names(void); /* New */
static int parse_type(void);
static int parse_subprogram_declaration(void);
static int parse_formal_parameters(void); /* New */
static int parse_compound_statement(void);
static int parse_statement(void);   /* 統合 */
static int parse_expressions(void); /* New */
static int parse_variable(void);
static int parse_expression(void);
static int parse_simple_expression(void);
static int parse_term(void);
static int parse_factor(void);
static int parse_output_format(void); /* New */

/* ヘルパー関数 */
static int  output_token(void);
static int  error_msg(char * mes);
static void print_newline(void);
static void print_indent(void);

/* トークン文字列取得 */
static char const * get_token_string(int code) {
	switch (code) {
		case TPROGRAM: return "program";
		case TVAR: return "var";
		case TARRAY: return "array";
		case TOF: return "of";
		case TBEGIN: return "begin";
		case TEND: return "end";
		case TIF: return "if";
		case TTHEN: return "then";
		case TELSE: return "else";
		case TPROCEDURE: return "procedure";
		case TRETURN: return "return";
		case TCALL: return "call";
		case TWHILE: return "while";
		case TDO: return "do";
		case TNOT: return "not";
		case TOR: return "or";
		case TDIV: return "div";
		case TAND: return "and";
		case TCHAR: return "char";
		case TINTEGER: return "integer";
		case TBOOLEAN: return "boolean";
		case TREAD: return "read";
		case TWRITE: return "write";
		case TREADLN: return "readln";
		case TWRITELN: return "writeln";
		case TTRUE: return "true";
		case TFALSE: return "false";
		case TBREAK: return "break";
		case TPLUS: return "+";
		case TMINUS: return "-";
		case TSTAR: return "*";
		case TEQUAL: return "=";
		case TNOTEQ: return "<>";
		case TLE: return "<";
		case TLEEQ: return "<=";
		case TGR: return ">";
		case TGREQ: return ">=";
		case TLPAREN: return "(";
		case TRPAREN: return ")";
		case TLSQPAREN: return "[";
		case TRSQPAREN: return "]";
		case TASSIGN: return ":=";
		case TDOT: return ".";
		case TCOMMA: return ",";
		case TCOLON: return ":";
		case TSEMI: return ";";
		case TNAME: return string_attr;
		case TNUMBER: return string_attr;
		case TSTRING: return string_attr;
		default: return "???";
	}
}

/* --- ヘルパー実装 --- */

static int error_msg(char * mes) {
	fprintf(stderr, "\nSyntax Error at line %d: %s\n", get_linenum(), mes);
	debug_print("Error occurred.\n");
	return ERROR;
}

static void print_newline(void) {
	printf("\n");
	is_newline    = 1;
	space_pending = 0;
}

static void print_indent(void) {
	if (is_newline) {
		for (int i = 0; i < indent_level; i++) {
			printf("    ");
		}
		is_newline = 0;
	}
}

static int output_token(void) {
	char const * str = get_token_string(token);

	char msg[128];
	snprintf(msg, sizeof(msg), "output_token(): %s\n", str);
	debug_print(msg);

	if (token == TSTRING) {
		print_indent();
		if (space_pending) printf(" ");
		printf("'%s'", str);
	} else {
		if (token == TSEMI || token == TDOT || token == TCOMMA) space_pending = 0;
		print_indent();
		if (space_pending) printf(" ");
		printf("%s", str);
	}
	space_pending = 1;
	return NORMAL;
}

/* --- 構文解析の実装 (EBNF順) --- */

/* 1. program ::= "program" Name ";" block "." */
int parse_program(void) {
	debug_print("parse_program() called.\n");
	indent_level = 0;
	if (token != TPROGRAM) return error_msg("Keyword 'program' expected.");
	output_token();
	token = scan();
	if (token != TNAME) return error_msg("Program name expected.");
	output_token();
	token = scan();
	if (token != TSEMI) return error_msg("';' expected.");
	output_token();
	print_newline();
	token = scan();

	if (parse_block() == ERROR) return ERROR;

	if (token != TDOT) return error_msg("'.' expected.");
	output_token();
	print_newline();
	token = scan();
	return NORMAL;
}

/* 2. block ::= { variable_declaration | subprogram_declaration } compound_statement */
static int parse_block(void) {
	debug_print("parse_block() called.\n");
	while (token == TVAR || token == TPROCEDURE) {
		if (token == TVAR) {
			if (parse_variable_declaration() == ERROR) return ERROR;
		} else {
			if (parse_subprogram_declaration() == ERROR) return ERROR;
		}
	}
	if (parse_compound_statement() == ERROR) return ERROR;
	return NORMAL;
}

/* 3. variable_declaration ::= "var" variable_names ":" type ";" { variable_names ":" type ";" } */
static int parse_variable_declaration(void) {
	debug_print("parse_variable_declaration() called.\n");
	indent_level = 1;
	output_token(); /* var */
	print_newline();
	token = scan();

	indent_level++;
	while (token == TNAME) {
		if (parse_variable_names() == ERROR) return ERROR;

		if (token != TCOLON) return error_msg("':' expected.");
		output_token();
		token = scan();

		if (parse_type() == ERROR) return ERROR;

		if (token != TSEMI) return error_msg("';' expected.");
		output_token();
		print_newline();
		token = scan();
	}
	indent_level--;
	return NORMAL;
}

/* 4. variable_names ::= Name { "," Name } */
static int parse_variable_names(void) {
	debug_print("parse_variable_names() called.\n");
	if (token != TNAME) return error_msg("Name expected.");
	output_token();
	token = scan();

	while (token == TCOMMA) {
		output_token();
		token = scan();
		if (token != TNAME) return error_msg("Name expected after ','.");
		output_token();
		token = scan();
	}
	return NORMAL;
}

/* 5. type ::= standard_type | "array" ... */
static int parse_type(void) {
	debug_print("parse_type() called.\n");
	if (token == TINTEGER || token == TBOOLEAN || token == TCHAR) {
		output_token();
		token = scan();
	} else if (token == TARRAY) {
		output_token();
		token = scan();
		if (token != TLSQPAREN) return error_msg("'[' expected.");
		output_token();
		token = scan();
		if (token != TNUMBER) return error_msg("Array size expected.");
		output_token();
		token = scan();
		if (token != TRSQPAREN) return error_msg("']' expected.");
		output_token();
		token = scan();
		if (token != TOF) return error_msg("'of' expected.");
		output_token();
		token = scan();
		if (parse_type() == ERROR) return ERROR;
	} else {
		return error_msg("Type expected.");
	}
	return NORMAL;
}

/* 6. subprogram_declaration ::= "procedure" Name [ formal_parameters ] ";" [ variable_declaration ]
 * compound_statement ";" */
static int parse_subprogram_declaration(void) {
	debug_print("parse_subprogram_declaration() called.\n");
	output_token(); /* procedure */
	token = scan();
	if (token != TNAME) return error_msg("Procedure name expected.");
	output_token();
	token = scan();

	if (token == TLPAREN) {
		if (parse_formal_parameters() == ERROR) return ERROR;
	}

	if (token != TSEMI) return error_msg("';' expected.");
	output_token();
	print_newline();
	token = scan();

	indent_level++;
	if (token == TVAR) {
		if (parse_variable_declaration() == ERROR) return ERROR;
	}
	if (parse_compound_statement() == ERROR) return ERROR;

	if (token != TSEMI) return error_msg("';' expected after procedure body.");
	output_token();
	print_newline();
	token = scan();
	indent_level--;
	return NORMAL;
}

/* 7. formal_parameters ::= "(" variable_names ":" type { ";" variable_names ":" type } ")" */
static int parse_formal_parameters(void) {
	debug_print("parse_formal_parameters() called.\n");
	output_token(); /* ( */
	token = scan();

	while (token == TNAME) {
		if (parse_variable_names() == ERROR) return ERROR;

		if (token != TCOLON) return error_msg("':' expected.");
		output_token();
		token = scan();

		if (parse_type() == ERROR) return ERROR;

		if (token == TSEMI) {
			output_token();
			token = scan();
		} else {
			break;
		}
	}

	if (token != TRPAREN) return error_msg("')' expected.");
	output_token();
	token = scan();
	return NORMAL;
}

/* 8. compound_statement ::= "begin" statement { ";" statement } "end" */
static int parse_compound_statement(void) {
	debug_print("parse_compound_statement() called.\n");
	if (token != TBEGIN) return error_msg("'begin' expected.");
	output_token();
	print_newline();
	token = scan();

	indent_level++;
	while (token != TEND) {
		if (parse_statement() == ERROR) return ERROR;

		if (token == TSEMI) {
			output_token();
			print_newline();
			token = scan();
		} else if (token == TEND) {
			break;
		} else {
			return error_msg("';' or 'end' expected.");
		}
	}
	indent_level--;

	if (!is_newline) print_newline();
	output_token(); /* end */
	token = scan();
	return NORMAL;
}

/* 9. statement ::= assignment | if | while | break | call | return | read | write | compound |
 * empty */
static int parse_statement(void) {
	debug_print("parse_statement() called.\n");
	switch (token) {
		case TNAME: /* Assignment: variable := expression */
			if (parse_variable() == ERROR) return ERROR;
			if (token != TASSIGN) return error_msg("':=' expected.");
			output_token();
			token = scan();
			if (parse_expression() == ERROR) return ERROR;
			return NORMAL;

		case TIF: /* If: if expr then stmt [else stmt] */
			output_token();
			token = scan();
			if (parse_expression() == ERROR) return ERROR;
			if (token != TTHEN) return error_msg("'then' expected.");
			output_token();
			token = scan();

			/* stmt 1 */
			if (token == TBEGIN) {
				// indent_level == 0;
				print_newline();
				if (parse_compound_statement() == ERROR) return ERROR;
			} else {
				print_newline();
				indent_level++;
				if (parse_statement() == ERROR) return ERROR;
				indent_level--;
			}

			if (token == TELSE) {
				if (!is_newline) print_newline();
				output_token();
				token = scan();
				/* stmt 2 */
				if (token == TBEGIN) {
					print_newline();
					if (parse_compound_statement() == ERROR) return ERROR;
				} else {
					print_newline();
					indent_level++;
					if (parse_statement() == ERROR) return ERROR;
					indent_level--;
				}
			}
			return NORMAL;

		case TWHILE: /* While: while expr do stmt */
			output_token();
			token = scan();
			if (parse_expression() == ERROR) return ERROR;
			if (token != TDO) return error_msg("'do' expected.");
			output_token();
			token = scan();
			if (token == TBEGIN) {
				print_newline();
				if (parse_compound_statement() == ERROR) return ERROR;
			} else {
				print_newline();
				indent_level++;
				if (parse_statement() == ERROR) return ERROR;
				indent_level--;
			}
			return NORMAL;

		case TBREAK: /* Break */
			output_token();
			token = scan();
			return NORMAL;

		case TCALL: /* Call: call Name [ ( expressions ) ] */
			output_token();
			token = scan();
			if (token != TNAME) return error_msg("Procedure name expected.");
			output_token();
			token = scan();
			if (token == TLPAREN) {
				output_token();
				token = scan();
				if (parse_expressions() == ERROR) return ERROR;
				if (token != TRPAREN) return error_msg("')' expected.");
				output_token();
				token = scan();
			}
			return NORMAL;

		case TRETURN: /* Return */
			output_token();
			token = scan();
			return NORMAL;

		case TREAD:
		case TREADLN: /* Input: read( variable { , variable } ) */
			output_token();
			token = scan();
			if (token == TLPAREN) {
				output_token();
				token = scan();
				if (parse_variable() == ERROR) return ERROR;
				while (token == TCOMMA) {
					output_token();
					token = scan();
					if (parse_variable() == ERROR) return ERROR;
				}
				if (token != TRPAREN) return error_msg("')' expected.");
				output_token();
				token = scan();
			}
			return NORMAL;

		case TWRITE:
		case TWRITELN: /* Output: write( output_format { , output_format } ) */
			output_token();
			token = scan();
			if (token == TLPAREN) {
				output_token();
				token = scan();
				if (parse_output_format() == ERROR) return ERROR;
				while (token == TCOMMA) {
					output_token();
					token = scan();
					if (parse_output_format() == ERROR) return ERROR;
				}
				if (token != TRPAREN) return error_msg("')' expected.");
				output_token();
				token = scan();
			}
			return NORMAL;

		case TBEGIN: /* Compound */ return parse_compound_statement();

		default: /* Empty */ return NORMAL;
	}
}

/* 10. expressions ::= expression { "," expression } */
static int parse_expressions(void) {
	debug_print("parse_expressions() called.\n");
	if (parse_expression() == ERROR) return ERROR;
	while (token == TCOMMA) {
		output_token();
		token = scan();
		if (parse_expression() == ERROR) return ERROR;
	}
	return NORMAL;
}

/* 11. variable ::= Name [ "[" expression "]" ] */
static int parse_variable(void) {
	debug_print("parse_variable() called.\n");
	if (token != TNAME) return error_msg("Variable name expected.");
	output_token();
	token = scan();
	if (token == TLSQPAREN) {
		output_token();
		token = scan();
		if (parse_expression() == ERROR) return ERROR;
		if (token != TRSQPAREN) return error_msg("']' expected.");
		output_token();
		token = scan();
	}
	return NORMAL;
}

/* 12. expression ::= simple_expression { RelOp simple_expression } */
static int parse_expression(void) {
	debug_print("parse_expression() called.\n");
	if (parse_simple_expression() == ERROR) return ERROR;
	while (token == TEQUAL || token == TNOTEQ || token == TLE || token == TLEEQ || token == TGR
	       || token == TGREQ) {
		output_token();
		token = scan();
		if (parse_simple_expression() == ERROR) return ERROR;
	}
	return NORMAL;
}

/* 13. simple_expression ::= [Sign] term { AddOp term } */
static int parse_simple_expression(void) {
	debug_print("parse_simple_expression() called.\n");
	if (token == TPLUS || token == TMINUS) {
		output_token();
		token = scan();
	}
	if (parse_term() == ERROR) return ERROR;
	while (token == TPLUS || token == TMINUS || token == TOR) {
		output_token();
		token = scan();
		if (parse_term() == ERROR) return ERROR;
	}
	return NORMAL;
}

/* 14. term ::= factor { MultOp factor } */
static int parse_term(void) {
	debug_print("parse_term() called.\n");
	if (parse_factor() == ERROR) return ERROR;
	while (token == TSTAR || token == TDIV || token == TAND) {
		output_token();
		token = scan();
		if (parse_factor() == ERROR) return ERROR;
	}
	return NORMAL;
}

/* 15. factor ::= variable | constant | ( expr ) | not factor | type ( expr ) */
static int parse_factor(void) {
	debug_print("parse_factor() called.\n");
	switch (token) {
		case TNAME: return parse_variable();
		case TNUMBER:
		case TSTRING:
		case TTRUE:
		case TFALSE:
			output_token();
			token = scan();
			return NORMAL;
		case TLPAREN:
			output_token();
			token = scan();
			if (parse_expression() == ERROR) return ERROR;
			if (token != TRPAREN) {
				debug_print("parse_factor(): TRPAREN, Missing ')'.\n");
				return error_msg("')' expected.");
			}
			output_token();
			token = scan();
			return NORMAL;
		case TNOT:
			output_token();
			token = scan();
			return parse_factor();
		case TINTEGER:
		case TBOOLEAN:
		case TCHAR:
			output_token();
			token = scan();
			if (token != TLPAREN) return error_msg("'(' expected.");
			output_token();
			token = scan();
			if (parse_expression() == ERROR) return ERROR;
			if (token != TRPAREN) { 
				debug_print("parse_factor(): TRPAREN, Missing ')'.\n");
				return error_msg("')' expected.");}
			output_token();
			token = scan();
			return NORMAL;
		default: return error_msg("Factor expected.");
	}
}

/* 16. output_format ::= expression [ ":" number ] | "long_string" */
static int parse_output_format(void) {
	debug_print("parse_output_format() called.\n");
	/* EBNF: 文字列の長さが1以外(2以上)なら文字列として扱う */
	if (token == TSTRING && (int)strlen(string_attr) != 1) {
		debug_print("parse_output_format(): Long string detected.\n");

		char msg[128];
		snprintf(msg, sizeof(msg), "parse_output_format(): str, len = %s, %d\n", string_attr,(int)strlen(string_attr));
		debug_print(msg);

		output_token();
		debug_print("parse_output_format(): Long string output done.\n");
		token = scan();
	} else {
		if (parse_expression() == ERROR) return ERROR;
		if (token == TCOLON) {
			output_token();
			token = scan();
			if (token != TNUMBER) return error_msg("Width expected.");
			output_token();
			token = scan();
		}
	}
	return NORMAL;
}