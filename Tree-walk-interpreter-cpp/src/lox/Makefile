# CXX = clang++
# CXXFLAGS =  -std=c++20 -Wall -Wextra -MMD -MP \
#            -fsanitize=address,undefined \
#            -fno-omit-frame-pointer \
#            -g -O1
# TARGET = lox

# SRCS = main.cpp Lox.cpp Parser.cpp Scanner.cpp Token.cpp Interpreter.cpp Environment.cpp Resolver.cpp LoxClass.cpp LoxFunction.cpp
# OBJS = $(SRCS:.cpp=.o)

# all: $(TARGET)

# $(TARGET): $(OBJS)
# 	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# %.o: %.cpp
# 	$(CXX) $(CXXFLAGS) -c $< -o $@

# clean:
# 	rm -f $(OBJS) $(TARGET) *.d

# -include $(OBJS:.o=.d)
#
#
#
CXX = clang++
CXXFLAGS = -std=c++20 -Wall -Wextra -MMD -MP \
           -fsanitize=address,undefined \
           -fno-omit-frame-pointer \
           -g -O1

# Paths
GTEST_DIR = ./googletest
GTEST_BUILD = $(GTEST_DIR)/build

# Google Test flags
GTEST_CXXFLAGS = $(CXXFLAGS) -I$(GTEST_DIR)/googletest/include -I$(GTEST_DIR)/googlemock/include
GTEST_LDFLAGS = -L$(GTEST_BUILD)/lib -lgtest -lgtest_main -pthread

# Main project
TARGET = lox
MAIN_SRCS = main.cpp Lox.cpp Parser.cpp Scanner.cpp Token.cpp Interpreter.cpp Environment.cpp Resolver.cpp LoxClass.cpp LoxFunction.cpp
MAIN_OBJS = $(MAIN_SRCS:.cpp=.o)

# Tests
TEST_TARGET = lox_test
TEST_SRCS = $(wildcard tests/*.cpp)
SHARED_SRCS = Lox.cpp Parser.cpp Scanner.cpp Token.cpp Interpreter.cpp Environment.cpp Resolver.cpp LoxClass.cpp LoxFunction.cpp
TEST_OBJS = $(TEST_SRCS:.cpp=.o) $(SHARED_SRCS:.cpp=.o)

.PHONY: all test clean

all: $(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TARGET): $(MAIN_OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(MAIN_OBJS)

$(TEST_TARGET): $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) $(GTEST_LDFLAGS) -o $(TEST_TARGET) $(TEST_OBJS)

%.o: %.cpp
	$(CXX) $(GTEST_CXXFLAGS) -c $< -o $@

tests/%.o: tests/%.cpp
	$(CXX) $(GTEST_CXXFLAGS) -c $< -o $@

clean:
	rm -f $(MAIN_OBJS) $(TEST_OBJS) $(TARGET) $(TEST_TARGET) *.d tests/*.o

-include $(MAIN_OBJS:.o=.d)
-include $(TEST_OBJS:.o=.d)

.PRECIOUS: $(BUILD_DIR) $(TEST_BUILD_DIR)