# #!/bin/bash
# set -e

# # Lab 4 build/run script. It auto-detects the single .y and .l source files,
# # so the required student-ID renaming will not break the script.
# PARSER_FILE=$(find . -maxdepth 1 -type f -name '*.y' | head -n 1)
# LEXER_FILE=$(find . -maxdepth 1 -type f -name '*.l' | head -n 1)
# INPUT_FILE=${1:-input.c}

# if [ -z "$PARSER_FILE" ] || [ -z "$LEXER_FILE" ]; then
#     echo "Could not find the parser (.y) or lexer (.l) file."
#     exit 1
# fi

# if [ ! -f "$INPUT_FILE" ]; then
#     echo "Input file not found: $INPUT_FILE"
#     exit 1
# fi

# if command -v yacc >/dev/null 2>&1; then
#     yacc -d -y --debug --verbose "$PARSER_FILE"
# elif command -v bison >/dev/null 2>&1; then
#     bison -d -y --debug --verbose "$PARSER_FILE"
# else
#     echo "Install yacc/bison before running this script."
#     exit 1
# fi

# echo 'Generated the parser C file and header file'
# g++ -std=c++17 -w -c -o y.o y.tab.c

# echo 'Generated the parser object file'
# if ! command -v flex >/dev/null 2>&1; then
#     echo "Install flex before running this script."
#     exit 1
# fi
# flex "$LEXER_FILE"

# echo 'Generated the scanner C file'
# g++ -std=c++17 -fpermissive -w -c -o l.o lex.yy.c

# echo 'Generated the scanner object file'
# g++ y.o l.o -o compiler_lab4

# echo 'All ready, running the compiler...'


# echo 'Compilation completed.'
# echo '------------ Log output ------------'
# //cat log.txt
# echo '------------ Error output ------------'
# //cat error.txt
# echo '------------ Three Address Code ------------'
# //cat code.txt




# !/bin/bash

# First pass: Generate AST and symbol table
yacc -d -y --debug --verbose 24241370.y
echo 'Generated the parser C file and header file'
g++ -w -c -o y.o y.tab.c
echo 'Generated the parser object file'
flex 24241370.l
echo 'Generated the scanner C file'
g++ -fpermissive -w -c -o l.o lex.yy.c
echo 'Generated the scanner object file'
g++ y.o l.o -o two_pass_compiler
echo 'All ready, running the two-pass compiler...'

# Run the compiler on the input file
./two_pass_compiler input.c
echo 'Compilation completed.'

# Display output files
echo '------------ Log output ------------'
cat log.txt
echo '------------ Error output ------------'
cat error.txt
echo '------------ Three Address Code ------------'
cat code.txt