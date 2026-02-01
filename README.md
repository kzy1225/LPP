# LPP
3年後期。言語処理プログラミング。Language Processing Programing.

# 課題1
字句解析。

# 課題2
構文解析。プリティプリンタ。


# コマンド一覧
| genre | command | simple explanation of command |
|------|---------|--------------------------------|
| File viewing | `tail -n +1 sample22.mpl` | Display the contents of multiple files (show from the first line) |
| Testing | `lpptest 01test -vv -x` | Run lpptest and stop testing immediately when an error occurs |
| Testing | `lpptest 01test -vv -k sample12lf.mpl` | Execute tests of kadai1 for a specific sample file |
| Execution | `./tc ./testcases/sample011.mpl` | Execute program for assignment 1 |
| Execution | `./pp ./testcases/sample2a.mpl` | Execute program for assignment 2 |
| Environment | `lppshell` | Start the docker image |
| Build | `gcc *.c -o tc` | Compile all C files into an executable named tc |
| Build | `make` | Build the project using the Makefile |
| Build | `make clean && make` | Clean build files and recompile from scratch |
| Directory | `cd /home/kzy/KIT_32/12_LPP/kadai2` | Change current directory |
| Archive | `zip kadai.zip *.c *.h` | Create a zip archive containing C and header files |
| Archive | `zip kadai.zip *.c *.h makefile` | Create a zip archive including the Makefile |
| File operation | `cp scan.c main.c scan.h id-list.c id-list.h makefile /home/kzy/KIT_32/12_LPP/kadai2` | Copy files to the specified directory |
| File operation | `touch parser.h pretty.h pretty.c parser.c` | Create empty files |
| Coverage (gcov) | `make gcov` | Build the program with gcov (coverage) enabled |
| Coverage (gcov) | `./tc ../kadai1_testing/sample11pp.mpl` | Execute the program to generate coverage data |
| Coverage (gcov) | `gcov -b *.gcda` | Run gcov and output execution counts per line into `.gcov` files |
| Documentation | `doxygen -g` | Generate a default Doxygen configuration file |
| Documentation | `doxygen Doxyfile` | Generate documentation using Doxygen |


# memo

## 2026-02-01
sample24a.mplでParseError.

### 実行結果
```
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
```
