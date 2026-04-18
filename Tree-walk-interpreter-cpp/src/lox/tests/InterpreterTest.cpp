#include <gtest/gtest.h>
#include "../Scanner.h"
#include "../Parser.h"
#include "../Resolver.h"
#include "../Interpreter.h"

// ===== HELPER FUNCTIONS FOR OBJECT VARIANT =====
bool isNumber(const Object& obj) {
    return std::holds_alternative<double>(obj);
}

double asNumber(const Object& obj) {
    return std::get<double>(obj);
}

bool isBoolean(const Object& obj) {
    return std::holds_alternative<bool>(obj);
}

bool asBoolean(const Object& obj) {
    return std::get<bool>(obj);
}

bool isString(const Object& obj) {
    return std::holds_alternative<std::string>(obj);
}

std::string asString(const Object& obj) {
    return std::get<std::string>(obj);
}

bool isNil(const Object& obj) {
    return std::holds_alternative<std::monostate>(obj);
}

// Helper to get a variable from interpreter
Object getVariable(std::shared_ptr<Interpreter> interp, const std::string& name) {
    return interp->globals->get(Token(TokenType::IDENTIFIER, name, "", 1));
}

class InterpreterValueTest : public ::testing::Test {
protected:
    // Helper to parse and evaluate an expression
    Object evalExpression(const std::string& code) {
        Scanner scanner(code);
        auto tokens = scanner.scan_tokens();
        Parser parser(tokens);
        
        // Parse as a statement with print to evaluate
        auto stmts = parser.parse();
        auto interpreter = std::make_shared<Interpreter>();
        
        for(auto& stmt : stmts) {
            interpreter->execute(*stmt);
        }
        return Object(); // Would need to modify to return value
    }

    // Helper to evaluate a full program
    std::shared_ptr<Interpreter> executeProgram(const std::string& code) {
        Scanner scanner(code);
        auto tokens = scanner.scan_tokens();
        Parser parser(tokens);
        auto stmts = parser.parse();
        
        auto interpreter = std::make_shared<Interpreter>();
        Resolver resolver(interpreter);
        
        for(auto& stmt : stmts) {
            resolver.resolve(*stmt);
        }
        
        for(auto& stmt : stmts) {
            interpreter->execute(*stmt);
        }
        
        return interpreter;
    }
};

// ===== ARITHMETIC WITH VALUE VERIFICATION =====
TEST_F(InterpreterValueTest, AdditionCorrect) {
    auto interp = executeProgram("var result = 2 + 3;");
    // Access the variable
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_TRUE(isNumber(val));
    EXPECT_EQ(asNumber(val), 5.0);
}

TEST_F(InterpreterValueTest, SubtractionCorrect) {
    auto interp = executeProgram("var result = 10 - 3;");
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_EQ(asNumber(val), 7.0);
}

TEST_F(InterpreterValueTest, MultiplicationCorrect) {
    auto interp = executeProgram("var result = 4 * 5;");
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_EQ(asNumber(val), 20.0);
}

TEST_F(InterpreterValueTest, DivisionCorrect) {
    auto interp = executeProgram("var result = 20 / 4;");
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_EQ(asNumber(val), 5.0);
}



TEST_F(InterpreterValueTest, NegationCorrect) {
    auto interp = executeProgram("var result = -42;");
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_EQ(asNumber(val), -42.0);
}

TEST_F(InterpreterValueTest, ComplexArithmeticCorrect) {
    auto interp = executeProgram("var result = 2 + 3 * 4;"); // Should be 14, not 20
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_EQ(asNumber(val), 14.0);
}

// ===== COMPARISON WITH VALUE VERIFICATION =====
TEST_F(InterpreterValueTest, GreaterThanTrue) {
    auto interp = executeProgram("var result = 5 > 3;");
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_TRUE(isBoolean(val));
    EXPECT_TRUE(asBoolean(val));
}

TEST_F(InterpreterValueTest, GreaterThanFalse) {
    auto interp = executeProgram("var result = 3 > 5;");
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_FALSE(asBoolean(val));
}

TEST_F(InterpreterValueTest, EqualityTrue) {
    auto interp = executeProgram("var result = 5 == 5;");
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_TRUE(asBoolean(val));
}

TEST_F(InterpreterValueTest, EqualityFalse) {
    auto interp = executeProgram("var result = 5 == 3;");
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_FALSE(asBoolean(val));
}

TEST_F(InterpreterValueTest, NotEqualTrue) {
    auto interp = executeProgram("var result = 5 != 3;");
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_TRUE(asBoolean(val));
}

// ===== LOGICAL OPERATORS =====
TEST_F(InterpreterValueTest, LogicalAndBothTrue) {
    auto interp = executeProgram("var result = true and true;");
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_TRUE(asBoolean(val));
}

TEST_F(InterpreterValueTest, LogicalAndOneFalse) {
    auto interp = executeProgram("var result = true and false;");
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_FALSE(asBoolean(val));
}

TEST_F(InterpreterValueTest, LogicalOrBothFalse) {
    auto interp = executeProgram("var result = false or false;");
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_FALSE(asBoolean(val));
}

TEST_F(InterpreterValueTest, LogicalOrOneTrue) {
    auto interp = executeProgram("var result = false or true;");
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_TRUE(asBoolean(val));
}

TEST_F(InterpreterValueTest, LogicalNotTrue) {
    auto interp = executeProgram("var result = !true;");
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_FALSE(asBoolean(val));
}

TEST_F(InterpreterValueTest, LogicalNotFalse) {
    auto interp = executeProgram("var result = !false;");
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_TRUE(asBoolean(val));
}

// ===== STRING OPERATIONS =====
TEST_F(InterpreterValueTest, StringConcatenation) {
    auto interp = executeProgram("var result = \"hello\" + \" \" + \"world\";");
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_TRUE(isString(val));
    EXPECT_EQ(asString(val), "hello world");
}

TEST_F(InterpreterValueTest, NumberToStringConcat) {
    auto interp = executeProgram("var result = \"number: \" + 42;");
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_TRUE(isString(val));
    EXPECT_EQ(asString(val), "number: 42");
}

// ===== VARIABLE OPERATIONS =====
TEST_F(InterpreterValueTest, VariableAssignment) {
    auto interp = executeProgram("var x = 10; var result = x;");
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_EQ(asNumber(val), 10.0);
}

TEST_F(InterpreterValueTest, VariableReassignment) {
    auto interp = executeProgram("var x = 10; x = 20; var result = x;");
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_EQ(asNumber(val), 20.0);
}

// ===== CONTROL FLOW =====
TEST_F(InterpreterValueTest, IfTrueBranch) {
    auto interp = executeProgram(
        "var result = nil; "
        "if (5 > 3) { result = 42; }"
    );
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_EQ(asNumber(val), 42.0);
}

TEST_F(InterpreterValueTest, IfFalseBranch) {
    auto interp = executeProgram(
        "var result = nil; "
        "if (5 < 3) { result = 10; } else { result = 20; }"
    );
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_EQ(asNumber(val), 20.0);
}

TEST_F(InterpreterValueTest, WhileLoopCounts) {
    auto interp = executeProgram(
        "var count = 0; "
        "while (count < 5) { count = count + 1; } "
        "var result = count;"
    );
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_EQ(asNumber(val), 5.0);
}

TEST_F(InterpreterValueTest, ForLoopSum) {
    auto interp = executeProgram(
        "var sum = 0; "
        "for (var i = 1; i <= 5; i = i + 1) { sum = sum + i; } "
        "var result = sum;"
    );
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_EQ(asNumber(val), 15.0); // 1+2+3+4+5
}

// ===== FUNCTIONS =====
TEST_F(InterpreterValueTest, FunctionReturnsCorrectValue) {
    auto interp = executeProgram(
        "fun add(a, b) { return a + b; } "
        "var result = add(3, 4);"
    );
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_EQ(asNumber(val), 7.0);
}

TEST_F(InterpreterValueTest, FunctionWithMultipleStatements) {
    auto interp = executeProgram(
        "fun compute(x) { "
        "  var temp = x * 2; "
        "  return temp + 5; "
        "} "
        "var result = compute(10);"
    );
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_EQ(asNumber(val), 25.0); // 10*2+5
}

TEST_F(InterpreterValueTest, FibonacciCorrect) {
    auto interp = executeProgram(
        "fun fib(n) { "
        "  if (n <= 1) return n; "
        "  return fib(n - 1) + fib(n - 2); "
        "} "
        "var result = fib(6);"
    );
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_EQ(asNumber(val), 8.0); // 0,1,1,2,3,5,8
}

TEST_F(InterpreterValueTest, FactorialCorrect) {
    auto interp = executeProgram(
        "fun factorial(n) { "
        "  if (n <= 1) return 1; "
        "  return n * factorial(n - 1); "
        "} "
        "var result = factorial(5);"
    );
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_EQ(asNumber(val), 120.0);
}

// ===== CLOSURES =====
TEST_F(InterpreterValueTest, ClosureCaptureSingleVar) {
    auto interp = executeProgram(
        "fun outer(x) { "
        "  fun inner() { return x; } "
        "  return inner; "
        "} "
        "var f = outer(42); "
        "var result = f();"
    );
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_EQ(asNumber(val), 42.0);
}

TEST_F(InterpreterValueTest, ClosureCaptureMultipleVars) {
    auto interp = executeProgram(
        "fun outer(x) { "
        "  var y = x + 10; "
        "  fun inner() { return x + y; } "
        "  return inner; "
        "} "
        "var f = outer(5); "
        "var result = f();"
    );
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_EQ(asNumber(val), 20.0); // 5 + (5+10)
}

TEST_F(InterpreterValueTest, ClosureMutableCapture) {
    auto interp = executeProgram(
        "fun counter() { "
        "  var count = 0; "
        "  fun increment() { "
        "    count = count + 1; "
        "    return count; "
        "  } "
        "  return increment; "
        "} "
        "var c = counter(); "
        "c(); c(); "
        "var result = c();"
    );
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_EQ(asNumber(val), 3.0);
}

// ===== CLASSES =====
TEST_F(InterpreterValueTest, ClassFieldAccess) {
    auto interp = executeProgram(
        "class Point { "
        "  init(x, y) { this.x = x; this.y = y; } "
        "} "
        "var p = Point(10, 20); "
        "var result = p.x;"
    );
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_EQ(asNumber(val), 10.0);
}

TEST_F(InterpreterValueTest, ClassMethodReturn) {
    auto interp = executeProgram(
        "class Circle { "
        "  init(r) { this.r = r; } "
        "  area() { return 3.14 * this.r * this.r; } "
        "} "
        "var c = Circle(5); "
        "var result = c.area();"
    );
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_NEAR(asNumber(val), 78.5, 0.1);
}

TEST_F(InterpreterValueTest, ClassFieldModification) {
    auto interp = executeProgram(
        "class Box { "
        "  init(size) { this.size = size; } "
        "} "
        "var b = Box(5); "
        "b.size = 10; "
        "var result = b.size;"
    );
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_EQ(asNumber(val), 10.0);
}

// ===== INHERITANCE =====
TEST_F(InterpreterValueTest, InheritanceMethodCall) {
    auto interp = executeProgram(
        "class Animal { "
        "  speak() { return \"sound\"; } "
        "} "
        "class Dog < Animal { } "
        "var d = Dog(); "
        "var result = d.speak();"
    );
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_TRUE(isString(val));
    EXPECT_EQ(asString(val), "sound");
}

TEST_F(InterpreterValueTest, InheritanceMethodOverride) {
    auto interp = executeProgram(
        "class Animal { "
        "  speak() { return \"sound\"; } "
        "} "
        "class Dog < Animal { "
        "  speak() { return \"woof\"; } "
        "} "
        "var d = Dog(); "
        "var result = d.speak();"
    );
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_EQ(asString(val), "woof");
}

// ===== EDGE CASES =====



TEST_F(InterpreterValueTest, NegativeNumbers) {
    auto interp = executeProgram("var result = -5 + 3;");
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_EQ(asNumber(val), -2.0);
}

TEST_F(InterpreterValueTest, ChainedComparisons) {
    auto interp = executeProgram(
        "var x = 5; "
        "var result = x > 0 and x < 10;"
    );
    auto val = interp->globals->get(Token(TokenType::IDENTIFIER, "result", "", 1));
    EXPECT_TRUE(asBoolean(val));
}

// ===== SANITY TESTS (EXPECT_NO_THROW) =====
// These verify the program runs without errors

TEST_F(InterpreterValueTest, PrintFunction) {
    EXPECT_NO_THROW(executeProgram("print 42;"));
    EXPECT_NO_THROW(executeProgram("print \"hello\";"));
    EXPECT_NO_THROW(executeProgram("print true;"));
}

TEST_F(InterpreterValueTest, IfStatement) {
    EXPECT_NO_THROW(executeProgram("if (true) { print \"yes\"; }"));
    EXPECT_NO_THROW(executeProgram("if (false) { print \"yes\"; } else { print \"no\"; }"));
}

TEST_F(InterpreterValueTest, IfElseIfElse) {
    EXPECT_NO_THROW(executeProgram(
        "var x = 5; "
        "if (x > 10) { print \"big\"; } "
        "else if (x > 0) { print \"small\"; } "
        "else { print \"negative\"; }"
    ));
}

TEST_F(InterpreterValueTest, WhileLoop) {
    EXPECT_NO_THROW(executeProgram("var i = 0; while(i < 3) { print i; i = i + 1; }"));
}

TEST_F(InterpreterValueTest, ForLoop) {
    EXPECT_NO_THROW(executeProgram("for(var i = 0; i < 3; i = i + 1) { print i; }"));
}

TEST_F(InterpreterValueTest, NestedLoops) {
    EXPECT_NO_THROW(executeProgram(
        "for(var i = 0; i < 2; i = i + 1) { "
        "  for(var j = 0; j < 2; j = j + 1) { "
        "    print i; print j; "
        "  } "
        "}"
    ));
}

TEST_F(InterpreterValueTest, FunctionDeclaration) {
    EXPECT_NO_THROW(executeProgram("fun greet() { print \"hello\"; }"));
}

TEST_F(InterpreterValueTest, FunctionCall) {
    EXPECT_NO_THROW(executeProgram("fun greet() { print \"hello\"; } greet();"));
}

TEST_F(InterpreterValueTest, FunctionWithParameters) {
    EXPECT_NO_THROW(executeProgram("fun add(a, b) { print a + b; } add(2, 3);"));
}

TEST_F(InterpreterValueTest, FunctionWithReturn) {
    EXPECT_NO_THROW(executeProgram("fun add(a, b) { return a + b; } print add(2, 3);"));
}

TEST_F(InterpreterValueTest, NestedFunctions) {
    EXPECT_NO_THROW(executeProgram(
        "fun outer() { "
        "  fun inner() { return 42; } "
        "  return inner(); "
        "} "
        "print outer();"
    ));
}

TEST_F(InterpreterValueTest, SimpleClosure) {
    EXPECT_NO_THROW(executeProgram(
        "fun outer(x) { "
        "  fun inner() { return x; } "
        "  return inner; "
        "} "
        "var f = outer(5); "
        "print f();"
    ));
}

TEST_F(InterpreterValueTest, ClosureMultipleVariables) {
    EXPECT_NO_THROW(executeProgram(
        "fun outer(x) { "
        "  var y = x + 1; "
        "  fun inner() { return x + y; } "
        "  return inner; "
        "} "
        "var f = outer(5); "
        "print f();"
    ));
}

TEST_F(InterpreterValueTest, ClosureCaptureMutable) {
    EXPECT_NO_THROW(executeProgram(
        "fun counter() { "
        "  var count = 0; "
        "  fun increment() { count = count + 1; return count; } "
        "  return increment; "
        "} "
        "var c = counter(); "
        "print c(); print c(); print c();"
    ));
}

TEST_F(InterpreterValueTest, ClassDeclaration) {
    EXPECT_NO_THROW(executeProgram("class Point {}"));
}

TEST_F(InterpreterValueTest, ClassInstantiation) {
    EXPECT_NO_THROW(executeProgram("class Point {} var p = Point();"));
}

TEST_F(InterpreterValueTest, ClassWithConstructor) {
    EXPECT_NO_THROW(executeProgram(
        "class Point { "
        "  init(x, y) { "
        "    this.x = x; "
        "    this.y = y; "
        "  } "
        "} "
        "var p = Point(1, 2);"
    ));
}

TEST_F(InterpreterValueTest, ClassFieldAccessSanity) {
    EXPECT_NO_THROW(executeProgram(
        "class Point { "
        "  init(x, y) { this.x = x; this.y = y; } "
        "} "
        "var p = Point(1, 2); "
        "print p.x; print p.y;"
    ));
}

TEST_F(InterpreterValueTest, ClassMethod) {
    EXPECT_NO_THROW(executeProgram(
        "class Point { "
        "  init(x, y) { this.x = x; this.y = y; } "
        "  distance() { return this.x + this.y; } "
        "} "
        "var p = Point(3, 4); "
        "print p.distance();"
    ));
}

TEST_F(InterpreterValueTest, ClassFieldModificationSanity) {
    EXPECT_NO_THROW(executeProgram(
        "class Circle { "
        "  init(r) { this.r = r; } "
        "} "
        "var c = Circle(5); "
        "c.r = 10; "
        "print c.r;"
    ));
}

TEST_F(InterpreterValueTest, ClassInheritance) {
    EXPECT_NO_THROW(executeProgram(
        "class Animal { } "
        "class Dog < Animal { }"
    ));
}

TEST_F(InterpreterValueTest, InheritedMethodSanity) {
    EXPECT_NO_THROW(executeProgram(
        "class Animal { "
        "  speak() { return \"sound\"; } "
        "} "
        "class Dog < Animal { } "
        "var d = Dog(); "
        "print d.speak();"
    ));
}

TEST_F(InterpreterValueTest, MethodOverrideSanity) {
    EXPECT_NO_THROW(executeProgram(
        "class Animal { "
        "  speak() { return \"sound\"; } "
        "} "
        "class Dog < Animal { "
        "  speak() { return \"woof\"; } "
        "} "
        "var d = Dog(); "
        "print d.speak();"
    ));
}

// ===== COMPLEX PROGRAMS (END-TO-END INTEGRATION) =====

TEST_F(InterpreterValueTest, Fibonacci) {
    EXPECT_NO_THROW(executeProgram(
        "fun fib(n) { "
        "  if (n <= 1) return n; "
        "  return fib(n - 1) + fib(n - 2); "
        "} "
        "print fib(10);"
    ));
}

TEST_F(InterpreterValueTest, Factorial) {
    EXPECT_NO_THROW(executeProgram(
        "fun factorial(n) { "
        "  if (n <= 1) return 1; "
        "  return n * factorial(n - 1); "
        "} "
        "print factorial(5);"
    ));
}



TEST_F(InterpreterValueTest, SumNumbers) {
    EXPECT_NO_THROW(executeProgram(
        "fun sum(n) { "
        "  var total = 0; "
        "  for (var i = 1; i <= n; i = i + 1) { "
        "    total = total + i; "
        "  } "
        "  return total; "
        "} "
        "print sum(100);"
    ));
}

TEST_F(InterpreterValueTest, SimpleClass) {
    EXPECT_NO_THROW(executeProgram(
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
    ));
}

TEST_F(InterpreterValueTest, AnimalHierarchy) {
    EXPECT_NO_THROW(executeProgram(
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

TEST_F(InterpreterValueTest, PersonClass) {
    EXPECT_NO_THROW(executeProgram(
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

TEST_F(InterpreterValueTest, Counter) {
    EXPECT_NO_THROW(executeProgram(
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

TEST_F(InterpreterValueTest, FunctionFactory) {
    EXPECT_NO_THROW(executeProgram(
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


TEST_F(InterpreterValueTest, NestedScoping) {
    EXPECT_NO_THROW(executeProgram(
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
    ));
}

TEST_F(InterpreterValueTest, EmptyBlocks) {
    EXPECT_NO_THROW(executeProgram(
        "if (true) { } "
        "while (false) { } "
        "for (var i = 0; i < 0; i = i + 1) { }"
    ));
}

TEST_F(InterpreterValueTest, StringConcat) {
    EXPECT_NO_THROW(executeProgram(
        "var msg = \"Hello\"; "
        "msg = msg + \" \" + \"World\"; "
        "print msg;"
    ));
}

TEST_F(InterpreterValueTest, TruthyFalsy) {
    EXPECT_NO_THROW(executeProgram(
        "if (true) { print \"true\"; } "
        "if (false) { } else { print \"false\"; } "
        "if (nil) { } else { print \"nil\"; } "
        "if (0) { print \"0 is truthy\"; } "
        "if (\"\") { print \"empty string is truthy\"; }"
    ));
}
