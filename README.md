# LPP
3年後期。言語処理プログラミング。Language Processing Programing.

# 課題1
字句解析。

# 課題2
構文解析。プリティプリンタ。



# Execute test of kadaiX
lpptest 01test -vv
lpptest 02test -vv

# 特定のテストケースに対してlpptest+実行+中身表示
lpptest 02test -k sample22.mpl
./tc ./testcases/sample011.mpl
cat -n ./testcases/sample011.mpl


# lpptest に-x オプションを付けると，エラーが出た時点で以後のテストを中止します．
lpptest 01test -vv -x

# Execute test of kadai1 for each sample
lpptest 01test -vv -k sample12lf.mpl

# ファイルの中身を表示
cat -n ./testcases/sample011.mpl

tail -n +1 sample22.mpl

# Execute

./tc ./testcases/sample011.mpl

./pp ./testcases/sample2a.mpl

# 実験室環境の再現
## dockerイメージの起動

lppshell

# コンパイル

gcc *.c -o tc
または
make
または
make clean && make

# current dir

cd /home/kzy/KIT_32/12_LPP/kadai2

# zip圧縮

zip kadai.zip *.c *.h 
zip kadai.zip *.c *.h makefile

# gcov

(1) gcov ビルド
(2) 実行する。何を何回でも。
(3) gcov 実行。各行に何回通ったかが「[filename].gcov」に表示される。

make gcov
./tc ../kadai1_testing/sample11pp.mpl
gcov -b *.gcda


# main.c と scan.c の関係を見る場合
cflow main.c scan.c

# Doxygen
doxygen -g
doxygen Doxyfile

# copy files and directories

cp scan.c main.c scan.h id-list.c id-list.h makefile  /home/kzy/KIT_32/12_LPP/kadai2

# make empty files

touch parser.h pretty.h pretty.c parser.c

# Referrence
## Doxygen

https://qiita.com/ryotaro76/items/6730c6d8572e713e79b9



# memo
