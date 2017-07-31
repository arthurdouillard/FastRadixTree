CC=clang++
CXXFLAGS=-std=c++11 -Wall -Wextra -g

APP=TextMiningApp
CMP=TextMiningCompiler

APPCC=src/app.cpp
CMPCC=src/compiler.cpp

SRC=src/utils/utils.cpp 

all: app compiler

app:
	${CC} ${CXXFLAGS} ${SRC} ${APPCC} -o ${APP}


compiler:
	${CC} ${CXXFLAGS} ${SRC} ${CMPCC} -o ${CMP}

test:
	./tests/testsuite.py --ref ref --own . --dist 3 --words words.txt

.PHONY: clean
clean:
	${RM} ${APP}
	${RM} ${CMP}
	${RM} dict.bin
