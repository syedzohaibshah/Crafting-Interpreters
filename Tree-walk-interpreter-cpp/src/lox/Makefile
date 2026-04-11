CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -MMD -MP -fsanitize=address -g

TARGET = lox

SRCS = main.cpp Lox.cpp Parser.cpp Scanner.cpp Token.cpp Interpreter.cpp Environment.cpp Resolver.cpp LoxClass.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) *.d

-include $(OBJS:.o=.d)
