CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -O2

SOURCES = main.cpp chunk.cpp memory.cpp debug.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXEC = program

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXEC)
