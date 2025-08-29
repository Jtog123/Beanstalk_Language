CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -O2

# Just list all your cpp files here:
SOURCES = main.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXEC = program

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXEC)
