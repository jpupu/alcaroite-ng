CXX = c++

CXXFLAGS = -std=c++14 -Wall -O3

main: main.cpp
	$(CXX) -o main main.cpp ValueBlock.cpp $(CXXFLAGS)