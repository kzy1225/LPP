/**
 * @file main.c
 * @brief 構文解析器のメイン処理
 */

#include "parse.h"
#include "scan.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//! キーワードリスト配列
struct KEY key[KEYWORDSIZE] = {
    {      "and",       TAND},
    {    "array",     TARRAY},
    {    "begin",     TBEGIN},
    {  "boolean",   TBOOLEAN},
    {    "break",     TBREAK},
    {     "call",      TCALL},
    {     "char",      TCHAR},
    {      "div",       TDIV},
    {       "do",        TDO},
    {     "else",      TELSE},
    {      "end",       TEND},
    {    "false",     TFALSE},
    {       "if",        TIF},
    {  "integer",   TINTEGER},
    {      "not",       TNOT},
    {       "of",        TOF},
    {       "or",        TOR},
    {"procedure", TPROCEDURE},
    {  "program",   TPROGRAM},
    {     "read",      TREAD},
    {   "readln",    TREADLN},
    {   "return",    TRETURN},
    {     "then",      TTHEN},
    {     "true",      TTRUE},
    {      "var",       TVAR},
    {    "while",     TWHILE},
    {    "write",     TWRITE},
    {  "writeln",   TWRITELN}
};

//! トークンカウンター
int numtoken[NUMOFTOKEN + 1];

//! 各トークンに対応する文字列
char * tokenstr[NUMOFTOKEN + 1]
    = {"",       "NAME",   "program",   "var",     "array",   "of",     "begin",   "end",  "if",
       "then",   "else",   "procedure", "return",  "call",    "while",  "do",      "not",  "or",
       "div",    "and",    "char",      "integer", "boolean", "readln", "writeln", "true", "false",
       "NUMBER", "STRING", "+",         "-",       "*",       "=",      "<>",      "<",    "<=",
       ">",      ">=",     "(",         ")",       "[",       "]",      ":=",      ".",    ",",
       ":",      ";",      "read",      "write",   "break"};

/* 外部エラー関数（scan.cなどで使用する場合、ここで定義が必要なら定義する） */
// int error(char *mes) {
// fprintf(stderr, "Error: %s\n", mes);
// return S_ERROR;
// }

int main(int argc, char * argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		return -1;
	}

	if (init_scan(argv[1]) < 0) {
		fprintf(stderr, "Cannot open file: %s\n", argv[1]);
		return -1;
	}

	token = scan(); /* 最初のトークン取得 */

    // for (size_t i = 0; i < 70; i++)
    // {
    //     	printf("%s", "=");
    // }

	// printf("token: %s\n", tokenstr[token]);

	if (parse_program() == S_ERROR) {
		end_scan();
		return -1;
	}

	end_scan();
	printf("OK\n");
	return 0;
}