CC=clang++
CXXFLAGS=-std=c++11 -Wall -Wextra -O3
LDFLAGS= -lboost_serialization

APP=TextMiningApp
CMP=TextMiningCompiler

APPCC=src/app.cpp
CMPCC=src/compiler.cpp

SRC=src/utils/utils.cpp #src/trie/trie.hxx src/word/word.hxx


all: app compiler


app:
	${CC} ${CXXFLAGS} -g ${SRC} ${APPCC} ${LDFLAGS} -o ${APP}


compiler:
	${CC} ${CXXFLAGS} -g ${SRC} ${CMPCC} ${LDFLAGS} -o ${CMP}


.PHONY: clean
clean:
	${RM} ${APP}
	${RM} ${CMP}
