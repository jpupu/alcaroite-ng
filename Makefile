CXX = c++

CXXFLAGS = -std=c++11 -Wall -O3

main: main.cpp
	$(CXX) -o main main.cpp $(CXXFLAGS)