# Tree-Walk Interpreter: Code Tracing Guide

Quick reference for understanding how specific features are implemented across the architecture.

---

## How to Trace Any Feature

**General approach:**
1. Start with the Token/TokenType in Scanner
2. Follow parsing in Parser
3. Check AST classes in Expr.h/Stmt.h
4. See resolver logic in Resolver
5. See execution in Interpreter visitor methods

---

## Feature: Variable Declaration & Access

### Source Code
```lox
var x = 42;
print x;
```

### Trace Through Architecture

**SCANNER**
- `x` → Token(IDENTIFIER, "x", nil, 1)
- `42` → Token(NUMBER, "42", 42.0, 1)
- `=` → Token(EQUAL, "=", nil, 1)
- `;` → Token(SEMICOLON, ";", nil, 1)

**PARSER**
- Calls `declaration()` → `varDeclaration()`
- Parses: `Var(Token("x"), Literal(42))`
- For `print x`: `Print(Variable(Token("x")))`

Files: [Parser.h L32-34](src/lox/Parser.h#L32-L34)

**AST STRUCTURE**
```cpp
Var stmt:
├─ name: Token("x")
└─ initializer: Literal(42.0)

Print stmt:
└─ expression: Variable(Token("x"))
```

Files: [Stmt.h L87-100, 64-78](src/lox/Stmt.h#L87-L100)

**RESOLVER**
```cpp
visitVarStmt(const Var& stmt):
├─ declare(name) - mark "x" as declared
├─ resolve(*initializer) - resolve Literal(42)
└─ define(name) - mark "x" as defined

visitVariableExpr(const Variable& expr):
└─ resolveLocal(expr, name) - compute distance
    └─ stores in interpreter.locals[expr*] = 0
```

Files: [Resolver.cpp](src/lox/Resolver.cpp)

**INTERPRETER**
```cpp
visitVarStmt(const Var& stmt):
└─ environment->define("x", 42.0)

visitPrintStmt(const Print& stmt):
├─ evaluate(expression)
│  └─ visitVariableExpr()
│     └─ lookUpVariable("x")
│        ├─ Check locals["x"] - distance = 0
│        └─ getAt(0, "x") - direct access
│           └─ environment->get("x") → 42.0
├─ stringify(42.0) → "42"
└─ print "42"
```

Files: [Interpreter.cpp](src/lox/Interpreter.cpp)

**ENVIRONMENT** 
```cpp
environment->define("x", 42.0)
  ├─ values["x"] = 42.0
  └─ stored in current scope

environment->get(Token("x"))
  ├─ found in values["x"] → 42.0
  └─ return 42.0
```

Files: [Environment.h](src/lox/Environment.h) / [Environment.cpp](src/lox/Environment.cpp)

---

## Feature: Binary Arithmetic

### Source Code
```lox
print 2 + 3;
```

### Parser Precedence Path

```
expression()
  ├─ assignment()
  │  └─ conditional()
  │     └─ orExpr()
  │        └─ andExpr()
  │           └─ equality()
  │              └─ comparison()
  │                 └─ term() ◄─ ADDITION/SUBTRACTION
  │                    ├─ factor() → Literal(2)
  │                    ├─ match(PLUS)? YES
  │                    ├─ factor() → Literal(3)
  │                    └─ return Binary(2, +, 3)
```

Files: [Parser.cpp](src/lox/Parser.cpp)

### AST Node
```cpp
Binary {
  left: Literal(2.0),
  op: Token(PLUS, "+", nil, 1),
  right: Literal(3.0)
}
```

Files: [Expr.h L56-68](src/lox/Expr.h#L56-L68)

### Interpreter Execution
```cpp
visitBinaryExpr(const Binary& expr):
├─ left = evaluate(Literal(2)) → 2.0
├─ right = evaluate(Literal(3)) → 3.0
├─ op.type == PLUS?
├─ check_numbered_operand(op, 2.0, 3.0)
├─ return 2.0 + 3.0 = 5.0
└─ In print: stringify(5.0) → "5"
```

Files: [Interpreter.cpp](src/lox/Interpreter.cpp)

---

## Feature: If Statement

### Source Code
```lox
if (x > 0) {
  print "positive";
} else {
  print "not positive";
}
```

### Parser Flow
```
statement()
  └─ if_statement()
     ├─ consume(IF) ✓
     ├─ consume(LEFT_PAREN)
     ├─ expression() → Binary(x, >, Literal(0))
     ├─ consume(RIGHT_PAREN)
     ├─ statement() → Block([Print(...)])
     ├─ match(ELSE)?
     ├─ statement() → Block([Print(...)])
     └─ return If(condition, thenBranch, elseBranch)
```

Files: [Parser.cpp](src/lox/Parser.cpp)

### AST Structure
```cpp
If {
  condition: Binary(Variable(x), >, Literal(0)),
  thenBranch: Block([Print(...)]),
  elseBranch: Block([Print(...)])
}
```

Files: [Stmt.h L115-130](src/lox/Stmt.h#L115-L130)

### Interpreter Execution
```cpp
visitIfStmt(const If& stmt):
├─ condition = evaluate(Binary(...))
├─ if (isTruthy(condition))
│  └─ execute(thenBranch)
├─ else if (elseBranch != nullptr)
│  └─ execute(elseBranch)
└─ return
```

Files: [Interpreter.cpp](src/lox/Interpreter.cpp)

---

## Feature: Function Definition & Call

### Source Code
```lox
fun add(a, b) {
  return a + b;
}
print add(2, 3);
```

### Scanner & Parser

**Parser flow for function definition:**
```
declaration()
  └─ functionDecl()
     ├─ consume(FUN)
     ├─ name = Token("add")
     ├─ consume(LEFT_PAREN)
     ├─ Parse params: [Token("a"), Token("b")]
     ├─ consume(RIGHT_PAREN)
     ├─ consume(LEFT_BRACE)
     ├─ Parse body: [Return(Binary(a, +, b))]
     └─ return Function("add", [a, b], [...])
```

Files: [Parser.cpp](src/lox/Parser.cpp)

**Parser flow for function call:**
```
expression()
  └─ ... → call()
     ├─ primary() → Variable("add")
     ├─ match(LEFT_PAREN)?
     ├─ finishCall()
     │  ├─ Parse args: [Literal(2), Literal(3)]
     │  └─ return Call(Variable("add"), [...])
```

### Resolver

```cpp
visitFunctionStmt(const Function& stmt):
├─ declare("add")
├─ define("add")
├─ resolveFunction(stmt, FUNCTION)
│  ├─ beginScope()
│  ├─ for each param: declare/define
│  ├─ resolve(body)
│  └─ endScope()

visitCallExpr(const Call& expr):
├─ resolve(callee) - resolve Variable("add")
├─ for each arg: resolve(arg)
└─ Associate expr with distance info
```

Files: [Resolver.cpp](src/lox/Resolver.cpp)

### Interpreter Execution

**Definition:**
```cpp
visitFunctionStmt(const Function& stmt):
├─ function_obj = LoxFunction(
│     declaration: &stmt,
│     closure: environment,
│     isInitializer: false
│  )
└─ environment->define("add", function_obj)
```

**Call:**
```cpp
visitCallExpr(const Call& expr):
├─ callee = evaluate(Variable("add"))
│  └─ lookUpVariable("add")
│     └─ globals.get("add") → LoxFunction
├─ arguments = [evaluate(Literal(2)), evaluate(Literal(3))]
│           = [2.0, 3.0]
├─ LoxFunction.call(interpreter, [2.0, 3.0])
│  ├─ Create new Environment(closure)
│  ├─ define("a", 2.0) in new env
│  ├─ define("b", 3.0) in new env
│  ├─ executeBlock(body, new_env)
│  │  ├─ execute(Return(...))
│  │  ├─ visitReturnStmt()
│  │  │  └─ throw ReturnVal(5.0)
│  │  └─ (exception thrown up)
│  └─ catch(ReturnVal ex) → return 5.0
└─ Result: 5.0
```

Files: [Interpreter.cpp](src/lox/Interpreter.cpp), [LoxFunction.cpp](src/lox/LoxFunction.cpp)

---

## Feature: Class Definition & Instantiation

### Source Code
```lox
class Dog {
  init(name) {
    this.name = name;
  }
  
  bark() {
    print this.name;
  }
}

var dog = Dog("Fido");
dog.bark();
```

### Parser Flow

**Class declaration:**
```
classDeclaration()
├─ consume(CLASS)
├─ name = Token("Dog")
├─ consume(LEFT_BRACE)
├─ while not RIGHT_BRACE:
│  └─ Parse methods (as functions):
│     ├─ Function("init", [name], [...])
│     └─ Function("bark", [], [...])
└─ return Class("Dog", [init_fn, bark_fn], ...)
```

**Instantiation `Dog("Fido")`:**
```
call()
├─ primary() → Variable("Dog")
├─ finishCall()
│  └─ Call(Variable("Dog"), [Literal("Fido")])
```

**Method call `dog.bark()`:**
```
call()
├─ primary() → Variable("dog")
├─ match(DOT)?
├─ name = Token("bark")
├─ Get(Variable("dog"), Token("bark"))
├─ match(LEFT_PAREN)?
└─ finishCall()
   └─ Call(Get(Variable("dog"), "bark"), [])
```

Files: [Parser.cpp](src/lox/Parser.cpp)

### Resolver

```cpp
visitClassStmt(const Class& stmt):
├─ declare(name) → "Dog"
├─ define(name)
├─ Scope check for class context
├─ for each method:
│  ├─ currentClass = CLASS
│  ├─ if method.name == "init": currentFunction = INITIALIZER
│  ├─ else: currentFunction = FUNCTION
│  ├─ resolveFunction(method, ...)
│  └─ restore state
└─ resolve Get/Set expressions with "this" tracking
```

Files: [Resolver.cpp](src/lox/Resolver.cpp)

### Interpreter Execution

**Definition:**
```cpp
visitClassStmt(const Class& stmt):
├─ Create LoxClass("Dog")
├─ methods_map["init"] = LoxFunction(init_fn, closure, isInitializer=true)
├─ methods_map["bark"] = LoxFunction(bark_fn, closure, isInitializer=false)
├─ class_obj = LoxClass("Dog", methods_map, superclass)
└─ environment->define("Dog", class_obj)
```

**Instantiation `Dog("Fido")`:**
```cpp
visitCallExpr(const Call& expr):
├─ callee = LoxClass("Dog")
├─ LoxClass.call(interpreter, ["Fido"])
│  ├─ instance = LoxInstance("Dog")
│  ├─ Find method "init"
│  ├─ Bind init to instance: init.bind(instance)
│  ├─ bound_init.call(interpreter, ["Fido"])
│  │  ├─ Create new Environment(init.closure)
│  │  ├─ define("this", instance)
│  │  ├─ define("name", "Fido")
│  │  ├─ executeBlock(init_body, new_env)
│  │  │  ├─ visitSetExpr(this.name = "Fido")
│  │  │  │  └─ instance.fields["name"] = "Fido"
│  │  │  └─ (implicit return nil for initializer)
│  │  └─ throw ReturnVal(nil) - caught
│  └─ return instance
└─ Result: LoxInstance("Dog") with fields["name"]="Fido"
```

**Method call `dog.bark()`:**
```cpp
visitCallExpr(const Call& expr):
├─ callee = evaluate(Get(dog, "bark"))
│  ├─ visitGetExpr()
│  ├─ object = LoxInstance("Dog")
│  ├─ object.get("bark")
│  │  ├─ Check fields["bark"] - not found
│  │  ├─ Check class.findMethod("bark")
│  │  ├─ Found! bark_function
│  │  ├─ bark_function.bind(instance)
│  │  │  └─ Create bound LoxFunction with "this" = dog
│  │  └─ return bound_function
│  └─ return bound_function
├─ arguments = []
├─ bound_function.call(interpreter, [])
│  ├─ Create new Environment(bark_fn.closure)
│  ├─ define("this", dog)
│  ├─ executeBlock(bark_body, new_env)
│  │  ├─ visitPrintStmt()
│  │  ├─ evaluate(Variable(this))
│  │  ├─ visitThisExpr()
│  │  │  └─ lookUpVariable("this") → dog
│  │  ├─ dog.get("name") → "Fido"
│  │  └─ print "Fido"
└─ Result: void (printed "Fido")
```

Files: [Interpreter.cpp](src/lox/Interpreter.cpp), [LoxClass.cpp](src/lox/LoxClass.cpp), [LoxInstance.h](src/lox/LoxInstance.h)

---

## Feature: While Loop & Break

### Source Code
```lox
var i = 0;
while (i < 3) {
  print i;
  i = i + 1;
  if (i == 2) break;
}
```

### Parser
```
whileStatement()
├─ consume(WHILE)
├─ consume(LEFT_PAREN)
├─ condition = expression() → Binary(i, <, 3)
├─ consume(RIGHT_PAREN)
├─ loopDepth++
├─ body = statement() → Block([...])
├─ loopDepth--
└─ return While(condition, body)

break_statement()
├─ consume(BREAK)
├─ consume(SEMICOLON)
├─ if (loopDepth == 0) ERROR
└─ return Break()
```

Files: [Parser.cpp](src/lox/Parser.cpp)

### Resolver
```cpp
visitWhileStmt(const While& stmt):
├─ resolve(condition)
└─ resolve(body)

visitBreakStmt(const Break& stmt):
├─ if (currentLoopDepth == 0)
│  └─ ERROR "break outside loop"
└─ return (break not resolved, handled at runtime)
```

Files: [Resolver.cpp](src/lox/Resolver.cpp)

### Interpreter Execution
```cpp
visitWhileStmt(const While& stmt):
├─ while (isTruthy(evaluate(condition)))
│  └─ try:
│     └─ execute(body)
│     catch(BreakException):
│        └─ break  ◄─ Exits while loop!
└─ return

visitBreakStmt(const Break& stmt):
├─ throw BreakException()
└─ (caught by while loop above)
```

Files: [Interpreter.cpp](src/lox/Interpreter.cpp), [Interpreter.h L17](src/lox/Interpreter.h#L17)

---

## Feature: Closures

### Source Code
```lox
fun makeAdder(x) {
  fun adder(y) {
    return x + y;
  }
  return adder;
}

var add5 = makeAdder(5);
print add5(3);  // Should print 8
```

### Key Insight: Closure Capture

When `adder` function is created, it captures the `closure` environment:
```
LoxFunction("adder"):
├─ declaration: &adder_stmt
├─ closure: Environment(parent=global)
│           ├─ x = 5  ◄─ CAPTURED!
│           └─ (other vars from makeAdder call)
└─ isInitializer: false
```

### Execution Flow

**Call `makeAdder(5)`:**
```
1. Create new Environment(closure=global)
2. define("x", 5) in new env
3. Execute makeAdder body
4. Parse & execute: fun adder(y) { return x + y; }
   └─ LoxFunction("adder", 
      declaration=adder_stmt,
      closure=Environment with x=5  ◄─ THIS!
    )
5. return adder
```

**Call `add5(3)`:**
```
1. add5 is LoxFunction("adder", closure with x=5)
2. Create new Environment(closure=that closure with x=5)
3. define("y", 3)
4. Execute body: return x + y
   ├─ evaluate Variable(x)
   │  └─ lookUpVariable("x")
   │     └─ Find in parent scope: x = 5
   ├─ evaluate Variable(y)
   │  └─ lookUpVariable("y")
   │     └─ Find in local scope: y = 3
   └─ return 5 + 3 = 8
```

Files: [LoxFunction.cpp](src/lox/LoxFunction.cpp), [Interpreter.cpp](src/lox/Interpreter.cpp)

---

## Feature: Short-Circuit Logical Operators

### Source Code
```lox
print false && print "never";  // Doesn't print "never"
print true || print "never";   // Doesn't print "never"
```

### Parser
```
andExpr() / orExpr()
  └─ Parses as Logical(left, op, right)
     (Same precedence, same structure as Binary)
```

Files: [Expr.h L144-155](src/lox/Expr.h#L144-L155)

### Interpreter (Key: Short-Circuit!)
```cpp
visitLogicalExpr(const Logical& expr):
├─ left = evaluate(left)  ◄─ Always evaluate left
├─ if (optr.type == OR)
│  ├─ if (isTruthy(left))
│  │  └─ return left  ◄─ Short-circuit! Right NOT evaluated
│  └─ else: continue
├─ else if (optr.type == AND)
│  ├─ if (!isTruthy(left))
│  │  └─ return left  ◄─ Short-circuit! Right NOT evaluated
│  └─ else: continue
└─ right = evaluate(right)  ◄─ Only if didn't short-circuit
```

Files: [Interpreter.cpp](src/lox/Interpreter.cpp)

---

## Feature: Variable Shadowing

### Source Code
```lox
var x = "outer";
{
  var x = "inner";
  print x;  // prints "inner"
}
print x;    // prints "outer"
```

### Resolver

```cpp
Scope Stack DURING RESOLUTION:

After global declaration:
[{"x": true}]  ◄─ x is defined in global

Enter block:
[{"x": true}, {"x": true}]  ◄─ x declared/defined in block scope

Exit block:
[{"x": true}]  ◄─ block scope popped

Variable lookup for inner print:
  └─ Search current scope first
     └─ Found x = true (inner x)
     └─ Store in locals map: distance = 0

Variable lookup for outer print:
  └─ Search current scope first
     └─ Not found in block scope
     └─ Search parent: found x = true (outer x)
     └─ Store in locals map: distance = 1
```

Files: [Resolver.cpp](src/lox/Resolver.cpp)

### Interpreter

```cpp
Inner Variable (distance 0):
  └─ lookUpVariable("x", distance=0)
     └─ getAt(0, "x")
        └─ environment.get("x") → "inner"

Outer Variable (distance 1):
  └─ lookUpVariable("x", distance=1)
     └─ getAt(1, "x")
        └─ ancestor(1).get("x") → "outer"
```

Files: [Interpreter.cpp](src/lox/Interpreter.cpp), [Environment.cpp](src/lox/Environment.cpp)

---

## Feature: Inheritance (with super)

### Source Code
```lox
class Animal {
  speak() { print "sound"; }
}

class Dog < Animal {
  speak() { print "woof"; }
  
  animalSpeak() {
    super.speak();  // Calls Animal.speak()
  }
}
```

### Parser
```
classDeclaration()
├─ ... parse class name
├─ match(LESS)?
│  ├─ superclass = Variable(Token("Animal"))
├─ ... parse methods
└─ return Class(name, methods, superclass)

# In method body, super.method() parses to:
call()
├─ Super(Token("super"), Token("method"))
├─ match(LEFT_PAREN)
└─ Call(Super(...), arguments)
```

### Resolver
```cpp
visitClassStmt(const Class& stmt):
├─ currentClass = (superclass != nullptr) ? SUBCLASS : CLASS
├─ if (superclass != nullptr)
│  └─ resolve(*superclass)  ◄─ Resolve the superclass reference
└─ (Resolve methods as before)

visitSuperExpr(const Super& expr):
└─ resolveLocal(expr, keyword)  ◄─ Store distance to "super"
```

### Interpreter
```cpp
visitClassStmt(const Class& stmt):
├─ superclass_obj = evaluate(*superclass)  ◄─ Evaluate to LoxClass
│  └─ Convert to LoxClass
├─ methods = Process all methods
├─ LoxClass(name, methods, superclass_obj)

visitSuperExpr(const Super& expr):
├─ distance = locals[expr*]
├─ superclass = ancestor(distance).get("super")
│  └─ "super" is implicitly defined during class eval
├─ instance = ancestor(distance).get("this")
├─ method = superclass.findMethod(method.lexeme)
├─ return method.bind(instance)  ◄─ Bind to CURRENT instance!

visitGetExpr() / visitCallExpr() (for super):
├─ If object is bound method from super
├─ Call it with "this" bound to actual instance
```

Files: [Interpreter.cpp](src/lox/Interpreter.cpp), [Resolver.cpp](src/lox/Resolver.cpp)

---

## How to Add New Language Features

Use the table below to understand what needs to be modified:

| Feature Type | Scanner | Parser | AST | Resolver | Interpreter |
|---|---|---|---|---|---|
| **New Token** | Add to Scanner::keywords or scan_token() | Handle in expression/statement | - | - | - |
| **New Operator** | Add token type | Add precedence level | Binary/Unary | Resolve operands | visitBinaryExpr/Unary |
| **New Keyword** | Add to keywords | New statement method | New Stmt class | New visitor method | New visitStmt method |
| **New Expression Type** | (via keywords) | New method in precedence chain | New Expr class | New visitor method | New visitExpr method |
| **New Built-in Function** | - | - | - | - | Add to Interpreter::globals |
| **New Language Construct** | (if needed) | New statement parser | New Stmt class | Resolve scopes/context | Execute with correct semantics |

---

## Debugging Tips

1. **Print AST**: Use [AstPrinter.h](src/lox/AstPrinter.h) to visualize parsed tree
2. **Trace Scopes**: Add debug output in Resolver to see scope chain
3. **Print Tokens**: Add debug output in Scanner to see tokenization
4. **Parse Errors**: Check Parser::synchronize() recovery logic
5. **Runtime Errors**: Check Interpreter error messages with line numbers
6. **Variable Lookup**: Enable debug in Environment::get/assign
7. **Type Issues**: Check std::holds_alternative usage in Interpreter
