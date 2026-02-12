/**
 * @file parse.h
 * @brief 構文解析器インターフェース
 */
#ifndef PARSE_H
#define PARSE_H

#include "scan.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h> /* 可変長引数のために必要 */

#define NORMAL 0
#define ERROR  1
#define FALSE  0
#define TRUE   1

/* 現在のトークンコード */
extern int token;

/* 構文解析のエントリーポイント */
int parse_program(void);


#endif /* PARSE_H */