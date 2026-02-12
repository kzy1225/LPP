/**
 * @file debug.c
 * @brief デバッグ用関数群の実装
 * @author 23122013
 * @date 2026-01-20
 */
#include "id-list.h"
#include "parse.h"
#include "scan.h"
#include <stdio.h>
#include <string.h>
#include <time.h>


FILE * debug_fp = NULL;
#define DEBUG_FILE "scan_trace.txt"


void open_debug_file(void) {
	debug_fp = fopen(DEBUG_FILE, "a");
	if (debug_fp == NULL) {
		fprintf(stderr, "Cannot open debug file.\n");
		return;
	}
}

void close_debug_file(void) {
	fclose(debug_fp);
}

void init_debug_file(char * filename) {
	debug_fp = fopen(DEBUG_FILE, "w");
	if (debug_fp == NULL) {
		fprintf(stderr, "Cannot open debug file.\n");
		return;
	}
	time_t now = time(NULL);
	fprintf(debug_fp, "SCANNING %s, %s\n", filename, ctime(&now));
	close_debug_file();
}

void debug_print_int_ch(int i, char const * msg) {
	open_debug_file();

	fprintf(debug_fp, "%2d, %10s\n", i, msg);

	close_debug_file();
}

void debug_print_chars(int c1, int c2, int c3) {
	open_debug_file();

	if (debug_fp == NULL) {
		fprintf(stderr, "Cannot open debug file.\n");
		return;
	}

	char * s1 = format_char_debug(c1);
	char * s2 = format_char_debug(c2);
	char * s3 = format_char_debug(c3);

	fprintf(debug_fp, "line = %2d , chars = %10s, %10s, %10s.\n", linenum, s1, s2, s3);

	close_debug_file();
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

void debug_print(char const * msg) {
	debug_fp = fopen(DEBUG_FILE, "a");

	if (debug_fp == NULL) {
		fprintf(stderr, "Cannot open debug file.\n");
		return;
	}

	fprintf(debug_fp, "%s", msg);
	close_debug_file();
}