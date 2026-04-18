#include <gtest/gtest.h>
#include "../Lox.h"

class IntegrationTest : public ::testing::Test {
protected:
    Lox lox;

    // Helper to run code and capture stdout for value verification
    std::string runAndCapture(const std::string& code) {
        testing::internal::CaptureStdout();
        lox.run(code);
        return testing::internal::GetCapturedStdout();
    }
};

// ===== VALUE VERIFICATION TESTS =====
// These tests verify the EXACT output/values produced

TEST_F(IntegrationTest, FibonacciCorrectValue) {
    std::string output = runAndCapture(
        "fun fib(n) { "
        "  if (n <= 1) return n; "
        "  return fib(n - 1) + fib(n - 2); "
        "} "
        "print fib(6);"
    );
    EXPECT_EQ(output, "8\n"); // fib(6) = 8
}

TEST_F(IntegrationTest, FactorialCorrectValue) {
    std::string output = runAndCapture(
        "fun factorial(n) { "
        "  if (n <= 1) return 1; "
        "  return n * factorial(n - 1); "
        "} "
        "print factorial(5);"
    );
    EXPECT_EQ(output, "120\n"); // 5! = 120
}

TEST_F(IntegrationTest, SumCorrectValue) {
    std::string output = runAndCapture(
        "fun sum(n) { "
        "  var total = 0; "
        "  for (var i = 1; i <= n; i = i + 1) { "
        "    total = total + i; "
        "  } "
        "  return total; "
        "} "
        "print sum(10);"
    );
    EXPECT_EQ(output, "55\n"); // sum(1..10) = 55
}

TEST_F(IntegrationTest, LoopCountCorrect) {
    std::string output = runAndCapture(
        "var count = 0; "
        "for (var i = 0; i < 5; i = i + 1) { "
        "  count = count + 1; "
        "} "
        "print count;"
    );
    EXPECT_EQ(output, "5\n");
}

TEST_F(IntegrationTest, ClassMethodCorrectValue) {
    std::string output = runAndCapture(
        "class Rectangle { "
        "  init(width, height) { "
        "    this.width = width; "
        "    this.height = height; "
        "  } "
        "  area() { "
        "    return this.width * this.height; "
        "  } "
        "} "
        "var rect = Rectangle(5, 10); "
        "print rect.area();"
    );
    EXPECT_EQ(output, "50\n");
}

TEST_F(IntegrationTest, ClosureCorrectValue) {
    std::string output = runAndCapture(
        "fun makeAdder(x) { "
        "  fun adder(y) { "
        "    return x + y; "
        "  } "
        "  return adder; "
        "} "
        "var add5 = makeAdder(5); "
        "print add5(3);"
    );
    EXPECT_EQ(output, "8\n");
}

TEST_F(IntegrationTest, InheritanceMethodCorrectValue) {
    std::string output = runAndCapture(
        "class Animal { "
        "  speak() { return \"sound\"; } "
        "} "
        "class Dog < Animal { "
        "  speak() { return \"woof\"; } "
        "} "
        "var d = Dog(); "
        "print d.speak();"
    );
    EXPECT_EQ(output, "woof\n");
}

TEST_F(IntegrationTest, SuperMethodCorrectValue) {
    std::string output = runAndCapture(
        "class Animal { "
        "  speak() { return \"sound\"; } "
        "} "
        "class Dog < Animal { "
        "  speak() { return super.speak() + \" woof\"; } "
        "} "
        "var d = Dog(); "
        "print d.speak();"
    );
    EXPECT_EQ(output, "sound woof\n");
}

TEST_F(IntegrationTest, NestedScopeCorrectValue) {
    std::string output = runAndCapture(
        "var x = 10; "
        "{ "
        "  var x = 20; "
        "  { "
        "    var x = 30; "
        "    print x; "
        "  } "
        "  print x; "
        "} "
        "print x;"
    );
    EXPECT_EQ(output, "30\n20\n10\n");
}

TEST_F(IntegrationTest, CounterMutableCapture) {
    std::string output = runAndCapture(
        "fun counter() { "
        "  var count = 0; "
        "  fun increment() { "
        "    count = count + 1; "
        "    return count; "
        "  } "
        "  return increment; "
        "} "
        "var c = counter(); "
        "print c(); "
        "print c(); "
        "print c();"
    );
    EXPECT_EQ(output, "1\n2\n3\n");
}

TEST_F(IntegrationTest, StringConcatCorrectValue) {
    std::string output = runAndCapture(
        "var msg = \"Hello\"; "
        "msg = msg + \" \" + \"World\"; "
        "print msg;"
    );
    EXPECT_EQ(output, "Hello World\n");
}

TEST_F(IntegrationTest, ConditionalCorrectPath) {
    std::string output = runAndCapture(
        "var x = 5; "
        "if (x > 10) { print \"big\"; } "
        "else if (x > 0) { print \"small\"; } "
        "else { print \"negative\"; }"
    );
    EXPECT_EQ(output, "small\n");
}



// ===== SANITY TESTS (EXPECT_NO_THROW) =====
// These verify the program runs without errors (don't verify exact values)

TEST_F(IntegrationTest, FibonacciProgram) {
    EXPECT_NO_FATAL_FAILURE(lox.run(
        "fun fib(n) { "
        "  if (n <= 1) return n; "
        "  return fib(n-1) + fib(n-2); "
        "} "
        "print fib(10);"
    ));
}

TEST_F(IntegrationTest, GameOfLife) {
    EXPECT_NO_FATAL_FAILURE(lox.run(
        "class Point { "
        "  init(x, y) { this.x = x; this.y = y; } "
        "} "
        "var p = Point(1, 2); "
        "print p.x;"
    ));
}

TEST_F(IntegrationTest, AnimalHierarchy) {
    EXPECT_NO_THROW(lox.run(
        "class Animal { "
        "  init(name) { "
        "    this.name = name; "
        "  } "
        "  speak() { "
        "    return \"Some sound\"; "
        "  } "
        "} "
        "class Dog < Animal { "
        "  speak() { "
        "    return \"Woof! I am \" + this.name; "
        "  } "
        "} "
        "class Cat < Animal { "
        "  speak() { "
        "    return \"Meow! I am \" + this.name; "
        "  } "
        "} "
        "var dog = Dog(\"Buddy\"); "
        "var cat = Cat(\"Whiskers\"); "
        "print dog.speak(); "
        "print cat.speak();"
    ));
}

TEST_F(IntegrationTest, PersonClass) {
    EXPECT_NO_THROW(lox.run(
        "class Person { "
        "  init(name, age) { "
        "    this.name = name; "
        "    this.age = age; "
        "  } "
        "  greet() { "
        "    return \"Hello, I am \" + this.name + \" and \" + this.age + \" years old\"; "
        "  } "
        "  haveBirthday() { "
        "    this.age = this.age + 1; "
        "  } "
        "} "
        "var p = Person(\"Alice\", 30); "
        "print p.greet(); "
        "p.haveBirthday(); "
        "print p.greet();"
    ));
}

TEST_F(IntegrationTest, Counter) {
    EXPECT_NO_THROW(lox.run(
        "class Counter { "
        "  init() { "
        "    this.count = 0; "
        "  } "
        "  increment() { "
        "    this.count = this.count + 1; "
        "    return this.count; "
        "  } "
        "  getValue() { "
        "    return this.count; "
        "  } "
        "} "
        "var c = Counter(); "
        "print c.increment(); "
        "print c.increment(); "
        "print c.increment(); "
        "print c.getValue();"
    ));
}

TEST_F(IntegrationTest, FunctionFactory) {
    EXPECT_NO_THROW(lox.run(
        "fun makeAdder(x) { "
        "  fun adder(y) { "
        "    return x + y; "
        "  } "
        "  return adder; "
        "} "
        "var add5 = makeAdder(5); "
        "var add10 = makeAdder(10); "
        "print add5(3); "
        "print add10(3);"
    ));
}

TEST_F(IntegrationTest, EmptyBlocks) {
    EXPECT_NO_THROW(lox.run(
        "if (true) { } "
        "while (false) { } "
        "for (var i = 0; i < 0; i = i + 1) { }"
    ));
}

TEST_F(IntegrationTest, TruthyFalsy) {
    EXPECT_NO_THROW(lox.run(
        "if (true) { print \"true\"; } "
        "if (false) { } else { print \"false\"; } "
        "if (nil) { } else { print \"nil\"; } "
        "if (0) { print \"0 is truthy\"; } "
        "if (\"\") { print \"empty string is truthy\"; }"
    ));
}
