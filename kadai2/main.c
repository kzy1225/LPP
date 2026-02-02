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


/* error() が scan.c から呼ばれる場合 */
int error(char * mes) {
	fprintf(stderr, "\nSyntax Error scan at line %d: %s\n", get_linenum(), mes);
	// fprintf(stderr, "Error: %s\n", mes);
	/* 構文解析エラーの場合は S_ERROR を返す設計なら return S_ERROR; */
	/* ここでは致命的エラーとして終了する場合 */
	exit(1);
}

int main(int argc, char * argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		return -1;
	}

	/* 1. スキャナ初期化 */
	if (init_scan(argv[1]) < 0) {
		fprintf(stderr, "Cannot open file: %s\n", argv[1]);
		return -1;
	}

	/* 2. 最初のトークン取得 (sscmain.cでも同様に行っている) */
	token = scan();

	/* 3. 構文解析実行 (parse_program が起点) */
	if (parse_program() == ERROR) {
		/* エラーメッセージは parse.c 内で出力済み */
		end_scan();
		return -1;
	}

	/* 4. 終了処理 */
	end_scan();
	// printf("\nOK\n"); /* 最後に改行と成功メッセージ */
	return 0;
}