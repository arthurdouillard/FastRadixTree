CC=clang++
CXXFLAGS=-std=c++11 -Wall -Wextra -O3

APP=TextMiningApp
CMP=TextMiningCompiler

APPCC=src/app.cpp
CMPCC=src/compiler.cpp

SRC=src/utils/utils.cpp 

# Args for the test suite
# Number of words tested for one distance
NUM_TEST=100 
# Distances to test
DIST=0 1 2 
 # Ref folder
REF=ref

all: app compiler

app:
	${CC} ${CXXFLAGS} ${SRC} ${APPCC} -o ${APP}

compiler:
	${CC} ${CXXFLAGS} ${SRC} ${CMPCC} -o ${CMP}

test:
	./tests/testsuite.py --ref ${REF} --own . --dist ${DIST} --run ${NUM_TEST} --words words.txt

.PHONY: clean
clean:
	${RM} ${APP}
	${RM} ${CMP}
	${RM} dict.bin
