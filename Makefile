CC=clang++
CXXFLAGS=-std=c++11 -Wall -Wextra

APP=TextMiningApp
CMP=TextMiningCompiler

APPCC=src/app.cpp
CMPCC=src/compiler.cpp

SRC=src/trie/trie.hxx src/word/word.hxx


all: app compiler


app:
	${CC} ${CXXFLAGS} ${SRC} ${APPCC} -o ${APP}


compiler:
	${CC} ${CXXFLAGS} ${SRC} ${CMPCC} -o ${CMP}


.PHONY: clean
clean:
	${RM} ${APP}
	${RM} ${CMP}
