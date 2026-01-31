# LPP
3年後期。言語処理プログラミング。Language Processing Programing.

# 課題1
字句解析。

# 課題2
構文解析。プリティプリンタ。


# コマンド一覧

Original shellscript:
./run_test_kadai2.sh sample24a.mpl



Execute test:
lpptest 01test -vv

Show contents of *.mpl:
cat -n ./testcases/sample011.mpl

複数のファイルの中身を表示:
tail -n +1 sample22.mpl

lpptest に-x オプションを付けると，エラーが出た時点で以後のテストを中止します．:
lpptest 01test -vv -x

Execute test of kadai1 for each sample:
lpptest 01test -vv -k sample12lf.mpl


実行(課題1):
./tc ./testcases/sample011.mpl

実行(課題2):
./pp ./testcases/sample2a.mpl



dockerイメージの起動:
lppshell

Compile:
gcc *.c -o tc
または
make
または
make clean && make

current dir:
cd /home/kzy/KIT_32/12_LPP/kadai2

zip圧縮:
zip kadai.zip *.c *.h 
または
zip kadai.zip *.c *.h makefile


copy files and directories:
cp scan.c main.c scan.h id-list.c id-list.h makefile  /home/kzy/KIT_32/12_LPP/kadai2

Make empty files:
touch parser.h pretty.h pretty.c parser.c


## gcov

(1) gcov ビルド
(2) 実行する。何を何回でも。
(3) gcov 実行。各行に何回通ったかが「[filename].gcov」に表示される。

make gcov
./tc ../kadai1_testing/sample11pp.mpl
gcov -b *.gcda



## Doxygen
doxygen -g
doxygen Doxyfile

# memo

## 2026-02-01
sample24a.mplでParseError.

### 実行結果

E           02_pp_mm_test.ParseError: 
E           Syntax Error at line 3: ')' expected.

02test/02_pp_mm_test.py:67: ParseError
======================================================= short test summary info ========================================================
FAILED 02test/01_pp_run_test.py::test_run[sample24a.mpl] - 01_pp_run_test.ParseError: 
FAILED 02test/02_pp_mm_test.py::test_idempotency[sample24a.mpl] - 02_pp_mm_test.ParseError: 
================================================== 2 failed, 79 deselected in 24.66s ===================================================

[2] Executed result of ./pp sample24a.mpl
program sample24a;
    var
        a : integer;
    begin
        a := 1;
        writeln ( a );

Syntax Error at line 3: ')' expected.
        writeln ( ''''
[3] Content of sample24a.mpl (cat -n)
     1	program sample24a; {single quote as char}
     2	var a:integer;
     3	begin a := 1;writeln(a); writeln('''':3) end.

kzy@Pvln:~/KIT_32/12_LPP/kadai2$ 

