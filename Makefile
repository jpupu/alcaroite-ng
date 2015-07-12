CXX = c++

CXXFLAGS = -std=c++14 -Wall -O3

all: main scheme

main: main.cpp
	$(CXX) -o main main.cpp $(CXXFLAGS)

scheme: scheme.o lexer.o
	$(CXX) -o scheme scheme.o lexer.o $(CXXFLAGS)