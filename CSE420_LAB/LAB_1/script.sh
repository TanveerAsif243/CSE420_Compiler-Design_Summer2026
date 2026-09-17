#!/bin/bash


yacc -d -y 24241370.y


g++ -w -c -o y.o y.tab.c


flex 24241370.l


g++ -w -c -o l.o lex.yy.c


g++ y.o l.o -o parser


./parser input.txt