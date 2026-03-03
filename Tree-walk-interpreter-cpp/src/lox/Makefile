CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra

TARGET = lox

SRCS = main.cpp Lox.cpp Parser.cpp Scanner.cpp Token.cpp Interpreter.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)