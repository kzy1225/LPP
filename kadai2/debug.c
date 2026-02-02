/**
 * @file debug.c
 * @brief デバッグ用関数群の実装
 * @author 23122013
 * @date 2026-01-20
 */
#include "scan.h"
#include "parse.h"
#include "id-list.h"
#include <stdio.h>
#include <string.h>
#include <time.h>


void debug_print_chars(int c1, int c2, int c3) {
	FILE * debug_fp = fopen("scan_trace.txt", "a");
	if (debug_fp == NULL) {
		fprintf(stderr, "Cannot open debug file.\n");
		return;
	}

	 char *s1 = format_char_debug(c1);
	 char *s2 = format_char_debug(c2);
	 char *s3 = format_char_debug(c3);

	fprintf(
	    debug_fp,
	    "line = %2d , chars = %10s, %10s, %10s.\n",
	    linenum, s1, s2, s3
	);

	fclose(debug_fp);
}
char * format_char_debug(int ch) {
	static char buf[4][32];
	static int  idx = 0;
	char *      p   = buf[idx++ & 3]; /* 0~3のバッファを順繰りに使う */

	if (ch == EOF) {
		snprintf(p, 32, "EOF (0xFF)");
	} else if (ch == '\r') {
		snprintf(p, 32, "CR  (0x0D)");
	} else if (ch == '\n') {
		snprintf(p, 32, "LF  (0x0A)");
	} else if (ch == '\t') {
		snprintf(p, 32, "TAB (0x09)");
	} else if (ch >= 0x20 && ch <= 0x7E) {
		snprintf(p, 32, "'%c' (0x%02X)", ch, ch);
	} else {
		snprintf(p, 32, "CTRL(0x%02X)", ch & 0xFF);
	}
	return p;
}

void debug_print(const char * msg) {
	/* 1. ファイルを「追加書き込みモード("a")」で開く */
	/* ※ファイル名は任意に変更してください */
	FILE * debug_fp = fopen("scan_trace.txt", "a");

	if (debug_fp == NULL) {
		fprintf(stderr, "Cannot open debug file.\n");
		return;
	}

	/* 2. ファイルポインタ(fp)に対して書き込む */
	fprintf(debug_fp, "%s", msg);

	/* 3. ファイルを閉じる */
	fclose(debug_fp);
}