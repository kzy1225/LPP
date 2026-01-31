#!/bin/bash

# 引数チェック
if [ "$#" -eq 0 ]; then
    echo "Usage: $0 filename1 [filename2 ...]"
    exit 1
fi

make clean && make

for filename in "$@"; do
    echo "=================================================="
    echo "TEST FILE: $filename"
    echo "=================================================="

    echo "[1] lpptest 02test"
    lpptest 02test -vv -k "$filename"
    echo

    echo "[2] Executed result of ./pp $filename"
    ./pp "../testcases/$filename"
    echo

    echo "[3] Content of $filename (cat -n)"
    cat -n "../testcases/$filename"
    echo
done
