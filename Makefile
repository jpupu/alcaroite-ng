CXX = c++

CXXFLAGS = -g -ggdb -std=c++14 -Wall -O3 -I.

%.o: %.cpp
	$(CXX) -MMD -MP -c $(CXXFLAGS) $< -o $@

srcs = $(wildcard *.cpp)
objs = $(srcs:.cpp=.o)
deps = $(srcs:.cpp=.d)

default: main

.PHONY: clean

clean:
	$(RM) $(objs) $(deps) main

main: $(objs)
	$(CXX) -o $@ $^ $(CXXFLAGS)

-include $(deps)
