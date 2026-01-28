/**
 * @file parse.c
 * @brief LL(1) 再帰下降構文解析器 (プリティプリント機能付き)
 */
#include "parse.h"
#include <string.h>

/* グローバル変数 */
int token;

/* プリティプリント用状態変数 */
static int indent_level = 0;   /* インデントの深さ */
static int is_newline = 1;     /* 行頭フラグ */
static int space_pending = 0;  /* 空白出力待ちフラグ */

/* --- プロトタイプ宣言 (すべて parse_ プレフィックス付与) --- */
static int parse_block(void);
static int parse_var_decl(void);
static int parse_subprogram_decl(void);
static int parse_type(void);
static int parse_compound_statement(void);
static int parse_statement(void);
static int parse_condition(void);
static int parse_expression(void);
static int parse_simple_expression(void);
static int parse_term(void);
static int parse_factor(void);
static int parse_variable(void);

/* ヘルパー関数 */
static int output_token(void);
static int error_msg(char *mes);
static void print_newline(void);
static void print_indent(void);

/* トークンコードから文字列への変換 */
static const char *get_token_string(int code) {
    switch(code) {
        case TPROGRAM: return "program";
        case TVAR:     return "var";
        case TARRAY:   return "array";
        case TOF:      return "of";
        case TBEGIN:   return "begin";
        case TEND:     return "end";
        case TIF:      return "if";
        case TTHEN:    return "then";
        case TELSE:    return "else";
        case TPROCEDURE: return "procedure";
        case TRETURN:  return "return";
        case TCALL:    return "call";
        case TWHILE:   return "while";
        case TDO:      return "do";
        case TNOT:     return "not";
        case TOR:      return "or";
        case TDIV:     return "div";
        case TAND:     return "and";
        case TCHAR:    return "char";
        case TINTEGER: return "integer";
        case TBOOLEAN: return "boolean";
        case TREAD:    return "read";
        case TWRITE:   return "write";
        case TREADLN:  return "readln";
        case TWRITELN: return "writeln";
        case TTRUE:    return "true";
        case TFALSE:   return "false";
        case TBREAK:   return "break";
        case TPLUS:    return "+";
        case TMINUS:   return "-";
        case TSTAR:    return "*";
        case TEQUAL:   return "=";
        case TNOTEQ:   return "<>";
        case TLE:      return "<";
        case TLEEQ:    return "<=";
        case TGR:      return ">";
        case TGREQ:    return ">=";
        case TLPAREN:  return "(";
        case TRPAREN:  return ")";
        case TLSQPAREN: return "[";
        case TRSQPAREN: return "]";
        case TASSIGN:  return ":=";
        case TDOT:     return ".";
        case TCOMMA:   return ",";
        case TCOLON:   return ":";
        case TSEMI:    return ";";
        case TNAME:    return string_attr; /* scan.c由来 */
        case TNUMBER:  return string_attr;
        case TSTRING:  return string_attr;
        default:       return "???";
    }
}

/* --- ヘルパー関数実装 --- */

static int error_msg(char *mes) {
    fprintf(stderr, "\nSyntax Error at line %d: %s\n", get_linenum(), mes);
    return ERROR;
}

static void print_newline(void) {
    printf("\n");
    is_newline = 1;
    space_pending = 0;
}

static void print_indent(void) {
    if (is_newline) {
        for (int i = 0; i < indent_level; i++) {
            printf("    "); /* 空白4つ */
        }
        is_newline = 0;
    }
}

static int output_token(void) {
    const char *str = get_token_string(token);

    if (token == TSTRING) {
        print_indent();
        if (space_pending) printf(" ");
        printf("'%s'", str);
    } else {
        /* ";", ".", "," の直前には空白を入れない */
        if (token == TSEMI || token == TDOT || token == TCOMMA) {
            space_pending = 0;
        }
        print_indent();
        if (space_pending) printf(" ");
        printf("%s", str);
    }
    space_pending = 1;
    return NORMAL;
}

/* --- 構文解析関数群 (parse_*) --- */

/* program Name ; block . */
int parse_program(void) {
    indent_level = 0;

    if (token != TPROGRAM) return error_msg("Keyword 'program' is missing.");
    output_token();
    token = scan();

    if (token != TNAME) return error_msg("Program name is missing.");
    output_token();
    token = scan();

    if (token != TSEMI) return error_msg("Semicolon missing.");
    output_token();
    print_newline(); /* ;の次は改行 */
    token = scan();

    if (parse_block() == ERROR) return ERROR;

    if (token != TDOT) return error_msg("Period '.' missing.");
    output_token();
    print_newline();
    token = scan();

    return NORMAL;
}

/* { var_decl } { subprogram_decl } compound_statement */
static int parse_block(void) {
    while (token == TVAR) {
        if (parse_var_decl() == ERROR) return ERROR;
    }
    while (token == TPROCEDURE) {
        if (parse_subprogram_decl() == ERROR) return ERROR;
    }
    if (parse_compound_statement() == ERROR) return ERROR;
    return NORMAL;
}

/* var Name { , Name } : Type ; ... */
static int parse_var_decl(void) {
    indent_level++;
    output_token(); /* var */
    print_newline();
    token = scan();

    indent_level++; /* 変数リストのインデント */

    while (token == TNAME) {
        output_token();
        token = scan();

        while (token == TCOMMA) {
            output_token();
            token = scan();
            if (token != TNAME) return error_msg("Name expected after comma.");
            output_token();
            token = scan();
        }

        if (token != TCOLON) return error_msg("Colon expected.");
        output_token();
        token = scan();

        if (parse_type() == ERROR) return ERROR;

        if (token != TSEMI) return error_msg("Semicolon expected.");
        output_token();
        print_newline();
        token = scan();

        if (token != TNAME) break;
    }

    indent_level -= 2; /* varとリストの分を戻す */
    return NORMAL;
}

/* 簡易的な型解析: integer, boolean, char, array... */
static int parse_type(void) {
    if (token == TINTEGER || token == TBOOLEAN || token == TCHAR || token == TARRAY) {
        output_token();
        if (token == TARRAY) {
            token = scan();
            if (token != TLSQPAREN) return error_msg("'[' expected.");
            output_token();
            token = scan();
            if (token != TNUMBER) return error_msg("Number expected.");
            output_token();
            token = scan();
            if (token != TRSQPAREN) return error_msg("']' expected.");
            output_token();
            token = scan();
            if (token != TOF) return error_msg("'of' expected.");
            output_token();
            token = scan();
            /* 要素の型 (再帰的に parse_type を呼んでも良いが、MPPLでは基本型のみが多い) */
            if (token != TINTEGER && token != TBOOLEAN && token != TCHAR) return error_msg("Standard type expected.");
            output_token();
        }
        token = scan();
        return NORMAL;
    }
    return error_msg("Type expected.");
}

/* procedure Name ( Params ) ; block ; */
static int parse_subprogram_decl(void) {
    indent_level++;
    output_token(); /* procedure */
    token = scan();

    if (token != TNAME) return error_msg("Procedure name expected.");
    output_token();
    token = scan();

    if (token == TLPAREN) { /* 仮引数 */
        output_token();
        token = scan();
        while (token == TNAME) {
            output_token();
            token = scan();
            while (token == TCOMMA) {
                output_token();
                token = scan();
                if (token != TNAME) return error_msg("Name expected.");
                output_token();
                token = scan();
            }
            if (token != TCOLON) return error_msg("Colon expected.");
            output_token();
            token = scan();
            if (parse_type() == ERROR) return ERROR;
            
            if (token == TSEMI) {
                output_token(); /* 引数中の;は改行しない */
                token = scan();
            } else {
                break;
            }
        }
        if (token != TRPAREN) return error_msg("')' expected.");
        output_token();
        token = scan();
    }

    if (token != TSEMI) return error_msg("Semicolon expected.");
    output_token();
    print_newline();
    token = scan();

    if (parse_block() == ERROR) return ERROR;

    if (token != TSEMI) return error_msg("Semicolon expected after block.");
    output_token();
    print_newline();
    token = scan();

    indent_level--;
    return NORMAL;
}

/* begin statement { ; statement } end */
static int parse_compound_statement(void) {
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

/* 各種文の解析 */
static int parse_statement(void) {
    /* 代入文 */
    if (token == TNAME) {
        if (parse_variable() == ERROR) return ERROR;
        if (token != TASSIGN) return error_msg("':=' expected.");
        output_token();
        token = scan();
        if (parse_expression() == ERROR) return ERROR;
        return NORMAL;
    }

    /* if文 */
    if (token == TIF) {
        output_token();
        token = scan();
        if (parse_condition() == ERROR) return ERROR;
        if (token != TTHEN) return error_msg("'then' expected.");
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

        if (token == TELSE) {
            if (!is_newline) print_newline();
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
        }
        return NORMAL;
    }

    /* while文 */
    if (token == TWHILE) {
        output_token();
        token = scan();
        if (parse_condition() == ERROR) return ERROR;
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
    }

    /* call文 */
    if (token == TCALL) {
        output_token();
        token = scan();
        if (token != TNAME) return error_msg("Procedure name expected.");
        output_token();
        token = scan();
        if (token == TLPAREN) {
            output_token();
            token = scan();
            if (parse_expression() == ERROR) return ERROR;
            while (token == TCOMMA) {
                output_token();
                token = scan();
                if (parse_expression() == ERROR) return ERROR;
            }
            if (token != TRPAREN) return error_msg("')' expected.");
            output_token();
            token = scan();
        }
        return NORMAL;
    }

    if (token == TRETURN || token == TBREAK) {
        output_token();
        token = scan();
        return NORMAL;
    }

    if (token == TREAD || token == TREADLN || token == TWRITE || token == TWRITELN) {
        output_token();
        token = scan();
        if (token == TLPAREN) {
            output_token();
            token = scan();
            if (parse_expression() == ERROR) return ERROR; /* 簡易的に式として処理 */
            while (token == TCOMMA) {
                output_token();
                token = scan();
                if (parse_expression() == ERROR) return ERROR;
            }
            if (token != TRPAREN) return error_msg("')' expected.");
            output_token();
            token = scan();
        }
        return NORMAL;
    }

    if (token == TBEGIN) {
        return parse_compound_statement();
    }

    return NORMAL; /* 空文 */
}

/* 変数参照 (配列対応) */
static int parse_variable(void) {
    if (token != TNAME) return error_msg("Name expected.");
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

/* 式の処理群 */

static int parse_condition(void) {
    if (parse_expression() == ERROR) return ERROR;
    if (token == TEQUAL || token == TNOTEQ || token == TLE || 
        token == TLEEQ || token == TGR || token == TGREQ) {
        output_token();
        token = scan();
        if (parse_expression() == ERROR) return ERROR;
    }
    return NORMAL;
}

static int parse_expression(void) {
    if (parse_simple_expression() == ERROR) return ERROR;
    if (token == TEQUAL || token == TNOTEQ || token == TLE || 
        token == TLEEQ || token == TGR || token == TGREQ) {
        output_token();
        token = scan();
        if (parse_simple_expression() == ERROR) return ERROR;
    }
    return NORMAL;
}

static int parse_simple_expression(void) {
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

static int parse_term(void) {
    if (parse_factor() == ERROR) return ERROR;
    while (token == TSTAR || token == TDIV || token == TAND) {
        output_token();
        token = scan();
        if (parse_factor() == ERROR) return ERROR;
    }
    return NORMAL;
}

static int parse_factor(void) {
    switch(token) {
        case TNAME:
            return parse_variable();
        case TNUMBER: case TSTRING: case TTRUE: case TFALSE:
            output_token();
            token = scan();
            return NORMAL;
        case TLPAREN:
            output_token();
            token = scan();
            if (parse_expression() == ERROR) return ERROR;
            if (token != TRPAREN) return error_msg("')' expected.");
            output_token();
            token = scan();
            return NORMAL;
        case TNOT:
            output_token();
            token = scan();
            return parse_factor();
        case TINTEGER: case TBOOLEAN: case TCHAR:
            output_token();
            token = scan();
            if (token != TLPAREN) return error_msg("'(' expected.");
            output_token();
            token = scan();
            if (parse_expression() == ERROR) return ERROR;
            if (token != TRPAREN) return error_msg("')' expected.");
            output_token();
            token = scan();
            return NORMAL;
        default:
            return error_msg("Factor expected.");
    }
}