# ----------------------------------------
#  MPPL 課題1 用 Makefile
# ----------------------------------------

# コンパイラ
CC = gcc

# 通常ビルド用フラグ
CFLAGS = -Wall -Wextra -std=c17

# gcov 用フラグ
GCOV_FLAGS = -fprofile-arcs -ftest-coverage

# ソースファイル
SRCS = main.c scan.c id-list.c

# 実行ファイル名
TARGET = tc

# オブジェクト
OBJS = $(SRCS:.c=.o)

# gcov 用オブジェクト
GCOV_OBJS = $(SRCS:.c=.gcov.o)

# ----------------------------------------
#  通常ビルド
# ----------------------------------------
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ----------------------------------------
#  gcov ビルド
# ----------------------------------------
gcov: CFLAGS += $(GCOV_FLAGS)
gcov: clean $(TARGET)

# ----------------------------------------
#  実行
# ----------------------------------------
run: $(TARGET)
	./$(TARGET) $(FILE)

# ----------------------------------------
#  クリーン
# ----------------------------------------
clean:
	rm -f $(OBJS) $(TARGET) \
	       *.gcda *.gcno *.gcov
