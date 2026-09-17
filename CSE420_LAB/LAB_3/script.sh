#!/bin/bash
set -e

# export STUDENT_ID="24241370"

yacc -d -y --debug --verbose syntax_analyzer.y
echo 'Generated parser source and header'

g++ -std=c++17 -w -c -o y.o y.tab.c

flex lex_analyzer.l
echo 'Generated scanner source'

g++ -std=c++17 -fpermissive -w -c -o l.o lex.yy.c

g++ y.o l.o -o parser

echo 'All ready, running'
./parser input.c

echo "===== 24241370_log.txt ====="
cat "24241370_log.txt"

echo "===== 24241370_error.txt ====="
cat "24241370_error.txt"

rm -f y.o l.o y.tab.c y.tab.h y.output lex.yy.c parser parser.exe