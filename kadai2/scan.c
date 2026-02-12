/**
 * @file scan.c
 * @brief 字句解析器
 * @author 23122013
 * @date 2026-01-20
 */
#include "scan.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

/// @brief scan() の戻り値が「符号なし整数」のとき，その値を格納している．
int num_attr;
/// @brief scan() の戻り値が「識別子」または「文字列」のとき，その文字列を格納。
char string_attr[MAXSTRSIZE];

/// @brief 入力ファイル
static FILE * fp = NULL;

/// @brief 現在着目している文字
static int current_char = EOF;

/// @brief current_charの直後の文字
static int cbuf = EOF;

/// @brief 現在の行番号
// static int linenum = 0;
int linenum = 0;

/// @brief トークン開始行番号
static int token_linenum = 0;

/// @brief scan() が一度でも呼ばれた
static int scan_called = 0;

/**
 * @brief ファイルから1文字読み込みそれを返す。
 */
static int raw_getchar(void) {
	return fgetc(fp);
}

/**
 * @brief 1文字進める。行番号を更新する。
 * @details current_char に cbuf を代入。 cbuf に raw_getchar() を代入。改行 (end of line) に応じて行番号を更新する。1文字進める処理は行番号の処理をしてから行う。
 */
static void advance(void) {
	int third_char = raw_getchar();

	debug_print_chars(current_char, cbuf, third_char);

	// 行番号の更新: ここでは3文字連続で確認でき，それぞれcurrent_char, cbuf, third_char  */
	// current_char ==  && cbuf ==  && third_char ==
	if (cbuf == '\n' && third_char == '\r' && current_char != '\r' && current_char != '\n') {
		/* in case of LFCR ‘\n’ ‘\r’ */
		debug_print("LFCR found.\n");
		linenum++;
		// linenum++;
	} else if (cbuf == '\r' && third_char == '\n' && current_char != '\r' && current_char != '\n') {
		/* in case of CRLF ‘\r’ ‘\n’ */
		debug_print("CRLF found.\n");
		linenum++;
		// linenum++;
	} else if (cbuf == '\r' && current_char != '\r' && current_char != '\n') {
		/* in case of CR ‘\r’ */
		debug_print("CR found.\n");
		linenum++;
	} else if (cbuf == '\n' && current_char != '\r' && current_char != '\n') {
		/* in case of LF ‘\n’ */
		debug_print("LF found.\n");
		linenum++;
	}

	current_char = cbuf;
	cbuf         = third_char;
}

/**
 * @brief アルファベット判定
 * @details アルファベット（a-z, A-Z）なら真を返す
 * @param ch 判定対象文字
 * @return int アルファベットなら真
 */
static int isAlpha(int ch) {
	return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z');
}

/**
 * @brief 数字判定
 * @details 数字（0-9）なら真を返す
 * @param ch 判定対象文字
 * @return int 数字なら真
 */
static int isDigit(int ch) {
	return ('0' <= ch && ch <= '9');
}

/**
 * @brief アルファベットまたは数字であることを判定
 * @details アルファベット（a-z, A-Z）または数字（0-9）なら真を返す
 * @param ch 判定対象文字
 * @return int アルファベットまたは数字なら真
 */
static int isAlphaNum(int ch) {
	return isAlpha(ch) || isDigit(ch);
}

/**
 * @brief 分離子判定
 * @details 空白・改行・EOF をすべて読み飛ばす対象とする
 * @param ch 判定対象文字
 * @return int 分離子なら真
 */
static int isSeparator(int ch) {
	return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == EOF;
}

/**
 * @brief キーワードまたは識別子の判定
 * @details 引数で与えられた文字列がキーワードであればそのトークンを，そうでなければ TNAME を返す
 * @param s 判定対象文字列
 * @return int キーワードのトークンまたは TNAME
 */
static int keyword_or_name(char const * s) {
	for (int i = 0; i < KEYWORDSIZE; i++) {
		if (strcmp(s, key[i].keyword) == 0) { return key[i].keytoken; }
	}
	return TNAME;
}

/**
 * @brief 字句解析器の初期化関数
 * @details 指定されたファイルを入力ファイルとして開き，スキャナの初期化を行う
 * @param filename
 * @return int
 */
int init_scan(char * filename) {
	if (!filename) { return -1; }

	fp = fopen(filename, "rb");
	if (!fp) { return -1; }

	linenum       = 1;
	token_linenum = 0;
	scan_called   = 0;

	/* 2文字先読み状態を構築 */
	current_char = raw_getchar();
	cbuf         = raw_getchar();

	/* debug file initialize */
	FILE * debug_fp = fopen("scan_trace.txt", "w");
	if (debug_fp == NULL) { return 0; }
	time_t now = time(NULL);
	fprintf(debug_fp, "SCANNING %s, %s\n", filename, ctime(&now));
	fclose(debug_fp);

	return 0;
}

/**
 * @brief スキャナの終了
 * @details スキャナの終了処理を行う
 */
void end_scan(void) {
	if (fp) { fclose(fp); }
	fp           = NULL;
	current_char = cbuf = EOF;
	linenum = token_linenum = scan_called = 0;
	// printf("End of scan.\n");
}

/**
 * @brief 行番号を返す関数
 * @details 直近のscan() で返されたトークンが存在した行の番号を返す．まだ一度もscan() が呼ばれていないときには 0 を返す．
 * @return int 行番号 または scan()が呼ばれていないことを示す0
 */
int get_linenum(void) {
	if (!scan_called) { return 0; }
	return token_linenum;
}

/**
 * @brief 文字列リテラルを読み取る
 *        string ::= "'" { string_element | "''" } "'"
 * @return TSTRING or S_ERROR
 */
static int scan_string(void) {
	debug_print("scan_string() called.\n");
	int len         = 0;  // string_attr 用
	int logical_len = 0;  // 文字列の「長さ」（{} の回数）

	token_linenum = linenum;

	/* 先頭の ' は scan() 側で確認済み */
	advance();  // 開始 '

	/* {}の中身を確認 */
	while (1) {
		debug_print("scan_string(): scanning...\n");
		/* EOF */
		if (current_char == EOF) {
			error("String not closed.");
			return S_ERROR;
		}

		/* 改行は禁止 */
		if (current_char == '\n' || current_char == '\r') {
			error("Newline in string literal.");
			return S_ERROR;
		}

		/* 2連続アポストロフィー */
		if (current_char == '\'' && cbuf == '\'') {
			string_attr[len++] = '\'';
			string_attr[len++] = '\'';
			logical_len++;
			advance();
			advance();
			continue;
		}

		/* string element: アポストロフィ，改行以外の任意の表示文字*/
		if (current_char != '\'' && current_char != '\n' && current_char != '\r') {
			string_attr[len++] = current_char;
			logical_len++;

			advance();
			continue;
		}

		/* 長さ確認 */
		if (logical_len >= MAXSTRSIZE - 1) {
			error("String literal too long.");
			return S_ERROR;
		}

		/* 終端のアポストロフィ */
		if (current_char == '\'') {
			// string_attr[len++] = current_char;
			break;
		}

		advance();
	}

	/* 終端 */
	string_attr[len] = '\0';

	char msg[1024];
	snprintf(msg, sizeof(msg), "scan_string(): string scanned: %s, %d\n", string_attr, logical_len);
	debug_print(msg);

	return TSTRING;
}

/**
 * @brief スキャナ本体
 * @details トークンを1つ読み取り，そのトークンを返す
 * @return int トークンまたは-1（EOF），S_ERROR（エラー）
 */
int scan(void) {
	char msg[128];
	snprintf(msg, sizeof(msg), "scan() called. linenum = %d\n", linenum);
	debug_print(msg);

	while (1) {
		/* ============================================================
		 * EOF : これ以上トークンは存在しない */
		/* ============================================================ */
		if (current_char == EOF) {
			scan_called = 1;
			return -1;
		}

		/* ============================================================
		 * 分離子: 空白・改行・EOF を読み飛ばす
		 * ============================================================ */
		if (isSeparator(current_char)) {
			advance();
			continue;
		}

		/* ============================================================
		 * { ... } コメント: 全体を読み飛ばす
		 * ============================================================ */
		if (current_char == '{') {
			advance();
			while (current_char != EOF && current_char != '}') {
				advance();
			}
			if (current_char == '}') { advance(); }

			continue;
		}

		/* ============================================================
		 * / / コメント: 全体を読み飛ばす
		 * ============================================================ */
		if (current_char == '/' && cbuf == '*') {
			advance();
			advance();
			while (current_char != EOF) {
				if (current_char == '*' && cbuf == '/') {
					advance();
					advance();
					break;
				}
				advance();
			}
			continue;
		}

		/* ============================================================
		 * 文字列: '...' を STRING トークンとして読み取る。
		 * 文字列 (string) ::= "’" { 文字列要素 | "’" "’" } "’"
		 * 文字列要素 (string element)：アポストロフィ"’"，改行以外の任意の表示文字
		 * ============================================================ */
		if (current_char == '\'') {
			/* First single quote found */
			debug_print("scan(): First single quote found.\n");
			return scan_string();
		}

		/* token started */
		token_linenum = linenum;
		scan_called   = 1;

		/* ============================================================
		 * キーワード
		 * ============================================================ */
		if (isAlpha(current_char)) {
			int i = 0;
			while (current_char != EOF && isAlphaNum(current_char)) {
				if (i >= MAXSTRSIZE - 1) {
					error("Identifier too long.");
					return S_ERROR;
				}
				string_attr[i++] = (char)current_char;
				advance();
			}
			string_attr[i] = '\0';
			return keyword_or_name(string_attr);
		}

		/* 数値 */
		if (isDigit(current_char)) {
			long val = 0;
			int  i   = 0;
			while (current_char != EOF && isDigit(current_char)) {
				if (i >= MAXSTRSIZE - 1) {
					error("Number too long.");
					return S_ERROR;
				}
				string_attr[i++] = (char)current_char;
				val              = val * 10 + (current_char - '0');
				if (val > 32768) {
					error("Number value exceeds 32768.");
					return S_ERROR;
				}
				advance();
			}
			string_attr[i] = '\0';
			num_attr       = (int)val;
			return TNUMBER;
		}

		/* 記号類 */
		switch (current_char) {
			case ':':
				advance();
				if (current_char == '=') {
					advance();
					return TASSIGN;
				}
				return TCOLON;
			case '<':
				if (cbuf == '=') {
					advance();
					advance();
					return TLEEQ;
				}
				if (cbuf == '>') {
					advance();
					advance();
					return TNOTEQ;
				}
				advance();
				return TLE;
			case '>':
				advance();
				if (current_char == '=') {
					advance();
					return TGREQ;
				}
				return TGR;
			case '+': advance(); return TPLUS;
			case '-': advance(); return TMINUS;
			case '*': advance(); return TSTAR;
			case '=': advance(); return TEQUAL;
			case '(': advance(); return TLPAREN;
			case ')': advance(); return TRPAREN;
			case '[': advance(); return TLSQPAREN;
			case ']': advance(); return TRSQPAREN;
			case '.': advance(); return TDOT;
			case ',': advance(); return TCOMMA;
			case ';': advance(); return TSEMI;
			default: error("Unknown character."); return S_ERROR;
		}
	}
}