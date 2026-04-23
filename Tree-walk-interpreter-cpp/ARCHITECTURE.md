# Tree-Walk Interpreter Architecture Analysis

## Overview

This is a **tree-walk interpreter** for the Lox language, written in C++. It follows the classic interpreter pipeline: source code → tokens → AST → semantic analysis → execution. The interpreter uses a visitor pattern to traverse and interpret the abstract syntax tree (AST).

---

## 1. Main Pipeline: Source to Execution

```
Source Code
    ↓
[SCANNER] - Lexical Analysis
    ↓
Token Stream
    ↓
[PARSER] - Syntax Analysis
    ↓
Abstract Syntax Tree (AST)
    ↓
[RESOLVER] - Semantic Analysis & Variable Resolution
    ↓
[INTERPRETER] - Execution
    ↓
Results/Output
```

### 1.1 Execution Entry Point

**File**: [main.cpp](src/lox/main.cpp) → [Lox.cpp](src/lox/Lox.cpp)

The `main()` function creates a `Lox` instance and routes execution based on command-line arguments:
- **No arguments**: Interactive REPL mode (`run_prompt()`)
- **One argument**: File execution mode (`run_file()`)
- **Multiple arguments**: Error

**Key Method**: `Lox::run(const std::string& source)` - Orchestrates the entire pipeline:

```cpp
void Lox::run(const std::string& source) {
    // 1. Tokenize
    Scanner scanner(source);
    std::vector<Token> tokens = scanner.scan_tokens();
    
    // 2. Parse
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();
    if (had_error) return;
    
    // 3. Resolve variables and scope
    auto interpreter = std::make_shared<Interpreter>();
    Resolver resolver(interpreter);
    resolver.resolve(statements);
    if (had_error) return;
    
    // 4. Interpret
    interpreter->interpret(statements);
}
```

Error handling halts processing at each stage if errors occur (parse errors, resolution errors, runtime errors).

---

## 2. Core Components

### 2.1 Scanner (Lexical Analysis)

**File**: [Scanner.h](src/lox/Scanner.h) / [Scanner.cpp](src/lox/Scanner.cpp)

**Responsibility**: Convert source code into tokens.

**Key Methods**:
- `scan_tokens()` - Main entry point; processes entire source
- `scan_token()` - Processes a single character/lexeme
- Helper methods: `match()`, `advance()`, `peek()`, `is_digit()`, `is_alpha()`, `string()`, `number()`, `identifier()`

**Data Members**:
- `source` - Raw source code string
- `tokens` - Vector of generated tokens
- `start`, `current` - Position tracking in source
- `line` - Line number tracking for error reporting
- `keywords` - Static map of reserved keywords

**Lexeme Types Recognized**:
- Single-character tokens: `(){},.;*-+/?:`
- Two-character operators: `!=`, `==`, `<=`, `>=`, `!`, `=`
- Literals: strings (quoted), numbers (float), identifiers
- Keywords: `and`, `class`, `else`, `false`, `for`, `fun`, `if`, `nil`, `or`, `print`, `return`, `super`, `this`, `true`, `var`, `while`, `break`, `static`, `trait`, `with`

**Output**: `std::vector<Token>` with an `END_OF_FILE` token marking the end.

---

### 2.2 Parser (Syntax Analysis)

**File**: [Parser.h](src/lox/Parser.h) / [Parser.cpp](src/lox/Parser.cpp)

**Responsibility**: Build an abstract syntax tree (AST) from tokens using recursive descent parsing.

**Grammar Implementation** (simplified):
```
program     → declaration* EOF
declaration → varDeclaration | functionDecl | classDecl | statement
statement   → expressionStmt | printStmt | ifStmt | whileStmt | forStmt | block | returnStmt | breakStmt
expression  → assignment
assignment  → conditional (= assignment)?
conditional → orExpr (? expression : conditional)?
orExpr      → andExpr (OR andExpr)*
andExpr     → equality (AND equality)*
equality    → comparison ((== | !=) comparison)*
comparison  → term ((> | >= | < | <=) term)*
term        → factor ((+ | -) factor)*
factor      → unary ((* | /) unary)*
unary       → (! | -) unary | call
call        → primary (. IDENTIFIER | ( arguments? ))*
primary     → NUMBER | STRING | TRUE | FALSE | NIL | ( expression )
```

**Key Methods**:
- `parse()` - Main entry; returns vector of statements
- `declaration()` - Top-level declarations
- `statement()` - Control flow and expression statements
- Expression parsing hierarchy: `expression()` → `assignment()` → `conditional()` → `orExpr()` → ... → `primary()`

**Precedence Levels** (lowest to highest):
1. Assignment (`=`)
2. Conditional (`? :`)
3. Logical OR (`||`)
4. Logical AND (`&&`)
5. Equality (`==`, `!=`)
6. Comparison (`<`, `>`, `<=`, `>=`)
7. Addition/Subtraction (`+`, `-`)
8. Multiplication/Division (`*`, `/`)
9. Unary (`!`, `-`)
10. Call and member access (`.`, `()`)
11. Primary (literals, variables, grouping)

**Error Handling**: Uses `synchronize()` to recover from parse errors and continue parsing for better error reporting.

**Output**: `std::vector<std::unique_ptr<Stmt>>` - the AST as statements.

---

### 2.3 Resolver (Semantic Analysis)

**File**: [Resolver.h](src/lox/Resolver.h) / [Resolver.cpp](src/lox/Resolver.cpp)

**Responsibility**: Perform variable scope resolution and semantic validation **before** execution.

**Key Concepts**:
- **Scope Chain**: Stack of scopes (using `std::deque<std::unordered_map<std::string, bool>>`)
- **Variable Tracking**: Maps variable names to depth (how many scope levels up they are defined)
- **Context Tracking**: Tracks if we're inside functions, classes, or traits

**Visitor Pattern**: Implements both `ExprVisitor` and `StmtVisitor` interfaces to traverse the entire AST.

**Key Operations**:
- `resolve(const std::vector<std::unique_ptr<Stmt>>& statements)` - Entry point
- `beginScope()` / `endScope()` - Manage scope stack
- `declare(Token name)` - Mark variable as declared but not yet defined
- `define(Token name)` - Mark variable as fully defined
- `resolveLocal(const Expr& expr, const Token& name)` - Determine variable distance (for interpreter optimization)
- `resolveFunction(const Function& function, FunctionType func)` - Handle function scopes

**State Tracking**:
- `currentFunction` - Is resolver in a function? (NONE, FUNCTION, INITIALIZER)
- `currentClass` - Is resolver in a class? (NONE, CLASS, SUBCLASS)
- `currentTrait` - Is resolver in a trait? (NONE, TRAIT)

**Validation**:
- Prevents use of variables before declaration
- Detects "this" usage outside classes
- Detects "super" usage outside classes
- Validates return statements only in functions
- Prevents functions from returning from class methods improperly

**Output**: Populates `Interpreter::locals` map (maps AST nodes to variable depths for optimized lookup).

---

### 2.4 Interpreter (Execution Engine)

**File**: [Interpreter.h](src/lox/Interpreter.h) / [Interpreter.cpp](src/lox/Interpreter.cpp)

**Responsibility**: Execute the AST using the visitor pattern.

**Key Visitor Methods**:

**Expression Visitors** (return `VisitorReturn` = `std::variant<std::string, Object, nullptr_t>`):
- `visitLiteralExpr()` - Numbers, strings, booleans, nil
- `visitBinaryExpr()` - Arithmetic and logical binary operations
- `visitUnaryExpr()` - Unary operators (!, -)
- `visitVariableExpr()` - Variable lookup
- `visitAssignExpr()` - Variable assignment
- `visitLogicalExpr()` - Short-circuit logical operators (&&, ||)
- `visitConditionalExpr()` - Ternary operator (? :)
- `visitCallExpr()` - Function/method calls
- `visitGroupingExpr()` - Parenthesized expressions
- `visitGetExpr()` - Object property access (obj.property)
- `visitSetExpr()` - Object property assignment (obj.property = value)
- `visitThisExpr()` - Class instance self-reference
- `visitSuperExpr()` - Superclass method access

**Statement Visitors** (return void):
- `visitExpressionStmt()` - Expression statements
- `visitPrintStmt()` - Print statements
- `visitVarStmt()` - Variable declaration and initialization
- `visitBlockStmt()` - Block scopes
- `visitIfStmt()` - If-else statements
- `visitWhileStmt()` - While loops
- `visitForStmt()` (implied) - For loops (desugared to while)
- `visitBreakStmt()` - Break statements (throws `BreakException`)
- `visitFunctionStmt()` - Function declaration
- `visitReturnStmt()` - Return statements (throws `ReturnVal`)
- `visitClassStmt()` - Class declaration
- `visitTraitStmt()` - Trait declaration

**Core State**:
- `globals` - Global environment (contains built-in functions like `clock()`)
- `environment` - Current environment (changes as scopes are entered/exited)
- `locals` - Map from AST nodes to variable depths (populated by Resolver)

**Helper Methods**:
- `evaluate(const Expr& expr)` - Evaluate an expression by calling `accept()`
- `execute(const Stmt& stmt)` - Execute a statement by calling `accept()`
- `stringify(const Object&)` - Convert runtime values to strings
- `isTruthy(const Object&)` - Truthy/falsy evaluation
- `isEqual(const Object&, const Object&)` - Equality comparison
- `lookUpVariable()` - Variable lookup with distance optimization
- `executeBlock()` - Execute statements in a new scope
- `interpret()` - Main entry point; executes all statements

**Control Flow**:
- **Break**: Throws `BreakException` caught in while loop
- **Return**: Throws `ReturnVal` exception caught in function call

**Output**: Prints to stdout (via `print` statements) or returns values via return statements.

---

## 3. Abstract Syntax Tree (AST)

### 3.1 Expression AST Nodes

**File**: [Expr.h](src/lox/Expr.h)

All expression nodes inherit from `Expr` base class and implement the visitor pattern (`VisitorReturn accept(ExprVisitor&)`).

**Expression Types**:

| Type | Purpose | Children |
|------|---------|----------|
| `Literal` | Constants (numbers, strings, booleans, nil) | `Object value` |
| `Binary` | Binary operations (+, -, *, /, ==, !=, <, >, <=, >=) | `Expr left, Token op, Expr right` |
| `Unary` | Unary operations (!, -) | `Token op, Expr right` |
| `Grouping` | Parenthesized expressions | `Expr expression` |
| `Conditional` | Ternary operator (? :) | `Expr condition, Expr thenBranch, Expr elseBranch` |
| `Variable` | Variable reference | `Token name` |
| `Assign` | Variable assignment | `Token name, Expr value` |
| `Logical` | Short-circuit logical operators (&&, \|\|) | `Expr left, Token optr, Expr right` |
| `Call` | Function/method invocation | `Expr callee, Token paren, vector<Expr> arguments` |
| `Get` | Property access (obj.prop) | `Expr object, Token name` |
| `Set` | Property assignment (obj.prop = value) | `Expr object, Token name, Expr value` |
| `This` | Class instance self-reference | `Token keyword` |
| `Super` | Superclass method access | `Token keyword, Token method` |

**Visitor Pattern**: Each expression node implements `accept(ExprVisitor&)` which dispatches to the appropriate visitor method.

### 3.2 Statement AST Nodes

**File**: [Stmt.h](src/lox/Stmt.h)

All statement nodes inherit from `Stmt` base class and implement the visitor pattern (`void accept(StmtVisitor&)`).

**Statement Types**:

| Type | Purpose | Children |
|------|---------|----------|
| `Expression` | Expression statement | `Expr expression` |
| `Print` | Print statement | `Expr expression` |
| `Var` | Variable declaration | `Token name, Expr initializer` |
| `Block` | Block scope | `vector<Stmt> statements` |
| `If` | If-else conditional | `Expr condition, Stmt thenBranch, Stmt elseBranch` |
| `While` | While loop | `Expr condition, Stmt body` |
| `Break` | Break loop control | (no children) |
| `Function` | Function declaration | `Token name, vector<Token> params, vector<Stmt> body` |
| `Return` | Return statement | `Token keyword, Expr value` |
| `Class` | Class declaration | `Token name, vector<Function> methods, vector<Function> staticMethods, vector<Expr> traits, Variable superclass` |
| `Trait` | Trait declaration | `Token name, vector<Function> methods` |

---

## 4. Runtime Value System

### 4.1 Object Type

**File**: [Object.h](src/lox/Object.h)

`Object` is a discriminated union (C++ `std::variant`) that can hold any runtime value:

```cpp
using Object = std::variant<
    std::monostate,                  // nil
    double,                          // numbers
    std::string,                     // strings
    bool,                            // booleans
    std::shared_ptr<LoxCallable>,   // functions/classes
    std::shared_ptr<LoxInstance>,   // class instances
    std::shared_ptr<LoxTrait>       // traits
>;
```

**Advantages**:
- Type-safe (no void pointers)
- Compile-time checked
- Zero runtime overhead for type information
- Variant can store only one value at a time

### 4.2 Callable Objects

**File**: [LoxCallable.h](src/lox/LoxCallable.h)

Base interface for all callable objects:

```cpp
class LoxCallable {
    virtual int arity() = 0;  // Number of parameters
    virtual Object call(std::shared_ptr<Interpreter> interpreter,
                       const std::vector<Object>& arguments) = 0;
};
```

**Concrete Implementations**:

#### LoxFunction
**File**: [LoxFunction.h](src/lox/LoxFunction.h) / [LoxFunction.cpp](src/lox/LoxFunction.cpp)

User-defined functions. Stores:
- `declaration` - AST Function node
- `closure` - Environment where function was defined (for closures)
- `isInitializer` - Whether this is a class initializer/constructor

**Key Methods**:
- `arity()` - Returns number of parameters
- `call()` - Executes function body with new environment
- `bind()` - Creates method bound to an instance

#### LoxClass
**File**: [LoxClass.h](src/lox/LoxClass.h) / [LoxClass.cpp](src/lox/LoxClass.cpp)

Classes (also callable - the call constructor). Stores:
- `name` - Class name
- `methods` - Map of method name → LoxFunction
- `superclass` - Superclass reference (for inheritance)
- `metaclass` - Metaclass for static methods

**Key Methods**:
- `arity()` - Returns initializer arity (or 0)
- `call()` - Creates new instance and calls initializer
- `findMethod()` - Method lookup with inheritance chain

#### LoxInstance
**File**: [LoxInstance.h](src/lox/LoxInstance.h)

Class instances. Stores:
- `klass` - Reference to the class
- `fields` - Map of property name → Object (instance state)

**Key Methods**:
- `get()` - Property access (checks fields, then methods)
- `set()` - Property assignment

### 4.3 Token System

**File**: [Token.h](src/lox/Token.h) / [Token.cpp](src/lox/Token.cpp)

Each token represents a single lexical unit:

```cpp
struct Token {
    TokenType type;      // The token type (from TokenType enum)
    std::string lexeme;  // The actual text (e.g., "x", "42", "hello")
    Object literal;      // Literal value (for strings/numbers)
    int line;           // Line number for error reporting
};
```

**TokenType Enum** [TokenType.h](src/lox/TokenType.h):
- Single-char: `(`, `)`, `{`, `}`, `,`, `.`, `-`, `+`, `;`, `/`, `*`, `?`, `:`
- Operators: `!`, `!=`, `=`, `==`, `>`, `>=`, `<`, `<=`
- Literals: `IDENTIFIER`, `STRING`, `NUMBER`
- Keywords: `AND`, `CLASS`, `ELSE`, `FALSE`, `FUN`, `FOR`, `IF`, `NIL`, `OR`, `PRINT`, `RETURN`, `SUPER`, `THIS`, `TRUE`, `VAR`, `WHILE`, `BREAK`, `STATIC`, `TRAIT`, `WITH`
- End marker: `END_OF_FILE`

---

## 5. Environment & Scope Management

**File**: [Environment.h](src/lox/Environment.h) / [Environment.cpp](src/lox/Environment.cpp)

The `Environment` class implements a **scope chain** for variable management.

**Structure**:
```
Global Environment
    ↓
Function Scope 1
    ↓
Block Scope 1
    ↓
Local Scope
```

**Key Members**:
- `values` - Map of variable name → Object (local variables)
- `enclosing` - Pointer to parent environment (or nullptr for global)

**Key Methods**:
- `define(name, value)` - Add variable to current scope
- `get(Token name)` - Look up variable (searches current and parent scopes)
- `assign(Token name, value)` - Update variable (must exist in current or parent scope)
- `getAt(distance, name)` - Direct access at specific scope distance
- `assignAt(distance, name, value)` - Direct assignment at specific scope distance
- `ancestor(distance)` - Get environment N levels up the scope chain

**Scope Chain Example**:
```cpp
// Global scope
var x = 5;

{
    // Block scope (enclosing = global)
    var x = 10;
    print x;  // prints 10
}

fun foo() {
    // Function scope (enclosing = global)
    print x;  // prints 5
    var y = 20;
}
```

**Optimization**: The `Resolver` pre-computes variable depths, so the `Interpreter` can use `getAt(distance, name)` for O(1) lookups instead of searching the scope chain.

---

## 6. Error Handling

**Error Types**:

1. **Scan Errors**: Invalid characters, unclosed strings
2. **Parse Errors**: Invalid syntax, missing tokens
3. **Resolution Errors**: Variables used before declaration, invalid contexts (this/super outside class)
4. **Runtime Errors**: Type mismatches, undefined variables, division by zero

**Error Management**:

**File**: [Lox.h](src/lox/Lox.h) / [Lox.cpp](src/lox/Lox.cpp)

```cpp
class Lox {
    static bool had_error;           // Any error occurred during current run
    static bool had_runtime_error;   // Runtime error occurred
    
    static void error(int line, const std::string& message);
    static void report(int line, const std::string& where, const std::string& message);
    static void error(const Token& token, const std::string& message);
    static void runtimeError(const RuntimeError& error);
};
```

**Exit Codes**:
- `0` - Success
- `65` - Parse error
- `70` - Runtime error

---

## 7. Design Patterns

### 7.1 Visitor Pattern

Used for AST traversal:

```
ExprVisitor Interface
    ↑
    |
Interpreter (implements ExprVisitor)
    ↑
    |
expr.accept(*this)  ←→  visitLiteralExpr(*expr)
```

Each AST node has an `accept()` method that dispatches to the appropriate visitor method based on node type. This decouples AST structure from operations on it.

**Benefits**:
- Easy to add new operations (create new visitor)
- AST nodes don't need to know about specific operations
- Type-safe dispatch

### 7.2 Environment Chain Pattern

Creates a linked list of scopes for variable lookup:

```cpp
std::shared_ptr<Environment> scope = std::make_shared<Environment>(parent_scope);
// Variables in scope can shadow parent scope variables
```

**Benefits**:
- Supports lexical scoping
- Supports closures (functions capture their closure environment)
- Natural implementation of nested scopes

### 7.3 AST Interpretation Pattern

Execute by walking the tree:

```
Interpreter visits nodes
    ↓
Evaluates expressions → Objects
    ↓
Executes statements → Side effects
    ↓
Produces results
```

Alternative to bytecode compilation or JIT.

**Benefits**:
- Simplicity
- Easy to debug
- Direct mapping to source code

**Drawbacks**:
- Slower than compiled code
- Repeats parsing of expressions in loops

### 7.4 Smart Pointer Usage

Extensive use of `std::shared_ptr` and `std::unique_ptr`:

- `std::unique_ptr` - AST ownership (Parser creates, Interpreter reads)
- `std::shared_ptr` - Shared objects (functions, classes, instances may be referenced from multiple places)

---

## 8. Data Flow Diagrams

### 8.1 Simple Expression Evaluation

```
Source: "2 + 3"
    ↓
Scanner → [Token(2, NUMBER), Token(+, PLUS), Token(3, NUMBER), Token(EOF)]
    ↓
Parser → Binary(Literal(2), +, Literal(3))
    ↓
Resolver → (no variables, no resolution needed)
    ↓
Interpreter.evaluate(Binary(...))
    → visitBinaryExpr()
    → evaluate(left) → 2.0
    → evaluate(right) → 3.0
    → 2.0 + 3.0 = 5.0
    ↓
Result: 5.0
```

### 8.2 Variable Assignment and Lookup

```
Source: "var x = 10; print x;"
    ↓
Scanner → Tokens
    ↓
Parser → [Var("x", Literal(10)), Print(Variable("x"))]
    ↓
Resolver:
  - Declares "x" in global scope
  - Associates Variable("x") expr node with depth 0
  ↓
Interpreter:
  - visitVarStmt() → globals.define("x", 10)
  - visitPrintStmt() → visitVariableExpr()
    → lookUpVariable("x") → globals.get("x") → 10
    → print "10"
```

### 8.3 Function Call

```
Source: "fun add(a, b) { return a + b; } print add(2, 3);"
    ↓
Parser → [Function("add", [a, b], [Return(Binary(a, +, b))]),
          Print(Call(Variable("add"), [2, 3]))]
    ↓
Resolver:
  - Declares "add" function in global scope
  - Resolves function scope and parameters
  - Associates Call node with depth info
  ↓
Interpreter:
  - visitFunctionStmt() → LoxFunction created, stored as "add"
  - visitCallExpr() → 
    - evaluate(callee) → LoxFunction("add")
    - evaluate args → [2, 3]
    - LoxFunction.call(interpreter, [2, 3])
      → Creates new Environment(closure)
      → Binds parameters a=2, b=3
      → executeBlock(body, new_env)
      → evaluate Binary(2, +, 3) → 5
      → throw ReturnVal(5)
    - Caught by function call, returns 5
  - visitPrintStmt() → print "5"
```

### 8.4 Class Definition and Instantiation

```
Source: "class Dog { bark() { print "woof"; } } var d = Dog();"
    ↓
Parser → [Class("Dog", [Function("bark", ...)]),
          Var("d", Call(Variable("Dog"), []))]
    ↓
Resolver:
  - Declares class in global scope
  - Resolves methods within class context
  ↓
Interpreter:
  - visitClassStmt() → LoxClass created, stored as "Dog"
  - visitVarStmt() → Call LoxClass("Dog")
    - LoxClass.call() →
      - Creates LoxInstance("Dog")
      - Calls initializer if exists
      - Returns instance
    - "d" = LoxInstance("Dog")
  - Later: d.bark() → Get(d, "bark")
    → LoxInstance.get("bark")
    → Finds method, binds to instance
    → Calls method with "this" = d
```

---

## 9. Key Language Features & Implementation

### 9.1 Variables

**Implementation**: 
- Storage in `Environment::values` map
- Scope chain resolution in `Environment::get()/assign()`
- Pre-computed depths in `Resolver` for optimization

**Features**:
- Block scope (lexical scoping)
- Shadowing (inner variables shadow outer)
- Uninitialized variables = `nil`

### 9.2 Functions

**Implementation**:
- Parsed as `Function` statements
- Converted to `LoxFunction` at runtime
- Closures via `closure` environment capture

**Features**:
- First-class functions (can be passed, returned, stored)
- Closures (capture enclosing scope)
- Recursion support
- Return values
- Variable parameters (via `...params` in some variants)

### 9.3 Control Flow

**If Statements**:
- Parsed as `If` statements
- Evaluated by `visitIfStmt()`
- Both branches optional

**While Loops**:
- Parsed as `While` statements
- Breaks implemented via `BreakException`
- Condition evaluated each iteration

**For Loops** (likely desugared to While):
- Convert to While with block scope and increment

### 9.4 Classes

**Implementation**:
- Parsed as `Class` statements
- Converted to `LoxClass` at runtime
- Methods stored as `LoxFunction`
- Instances as `LoxInstance`

**Features**:
- Inheritance (via `superclass`)
- Methods with `this` reference
- Initializers (constructor)
- Static methods
- Property access and assignment
- Traits/mixins support

### 9.5 Operators

**Arithmetic**: `+`, `-`, `*`, `/`
**Comparison**: `<`, `>`, `<=`, `>=`, `==`, `!=`
**Logical**: `&&`, `||`, `!`
**Ternary**: `condition ? thenVal : elseVal`

All implemented in `visitBinaryExpr()` and `visitUnaryExpr()`.

---

## 10. Component Interaction Summary

| Component | Input | Output | Dependencies |
|-----------|-------|--------|--------------|
| **Scanner** | Source string | `vector<Token>` | Lox (error reporting) |
| **Parser** | `vector<Token>` | `vector<Stmt>` | Scanner output, Lox (error reporting) |
| **Resolver** | `vector<Stmt>` (AST) | `locals` map (Interpreter) | Parser output, Interpreter |
| **Interpreter** | `vector<Stmt>` + `locals` | Output/Results | Resolver output, Environment, LoxCallable types |
| **Environment** | Variables | Scope chains | (used by Interpreter) |
| **LoxCallable** | Arguments | Object result | (used by Interpreter) |
| **AST (Expr/Stmt)** | None (passive) | Visitor dispatch | Used by Parser, Resolver, Interpreter |

**Execution Order**:
1. **Lox.run()** orchestrates
2. **Scanner** processes source
3. **Parser** builds AST
4. **Resolver** validates and optimizes
5. **Interpreter** executes

Each stage can fail and halt processing.

---

## 11. Memory Management

**Ownership Model**:

- **Parser owns AST**: Uses `std::unique_ptr` for tree nodes
- **Interpreter reads AST**: Borrows references to nodes
- **Environment chain**: `std::shared_ptr` for scope chain (can be referenced from multiple functions)
- **Callable objects**: `std::shared_ptr` (can be stored, passed, returned)

**Key Classes Using Smart Pointers**:
- `Parser::parse()` returns `vector<unique_ptr<Stmt>>`
- `LoxFunction`, `LoxClass`, `LoxInstance` stored in `shared_ptr`
- `Environment` uses `shared_ptr` for enclosing scope

**Garbage Collection**: C++ RAII handles cleanup automatically when shared_ptrs go out of scope.

---

## 12. Testing Infrastructure

**Files**:
- [tests/ScannerTest.cpp](tests/ScannerTest.cpp)
- [tests/ParserTest.cpp](tests/ParserTest.cpp)
- [tests/ResolverTest.cpp](tests/ResolverTest.cpp)
- [tests/InterpreterTest.cpp](tests/InterpreterTest.cpp)
- [tests/IntegrationTest.cpp](tests/IntegrationTest.cpp)

Uses **Google Test** framework (included in [googletest/](src/lox/googletest/) folder).

---

## 13. Architecture Strengths & Tradeoffs

### Strengths:
✅ **Clear separation of concerns** - Each stage has single responsibility  
✅ **Extensibility** - Visitor pattern makes adding features easier  
✅ **Debuggability** - Direct AST interpretation matches source code  
✅ **Type-safe** - Heavy use of C++ type system and variants  
✅ **Memory-safe** - Smart pointers prevent leaks  

### Tradeoffs:
⚠️ **Slower execution** - Tree-walking is slower than bytecode or JIT  
⚠️ **Memory overhead** - Each AST node is a separate object  
⚠️ **No optimization** - AST evaluated as-is without optimization passes  
⚠️ **Loop overhead** - Expressions in loops reparsed/reevaluated each iteration  

---

## 14. Extension Points

**To add new features**:

1. **New expression type**: Add class to Expr.h, visitor method, implement in Interpreter
2. **New statement type**: Add class to Stmt.h, visitor method, implement in Interpreter
3. **New token type**: Add to TokenType enum, handle in Scanner
4. **New operator**: Handle in visitBinaryExpr() / visitUnaryExpr()
5. **New built-in function**: Add to Interpreter::globals
6. **New keyword**: Add to Scanner::keywords, handle in Parser

All changes require coordinating across Scanner → Parser → Resolver → Interpreter pipeline.

