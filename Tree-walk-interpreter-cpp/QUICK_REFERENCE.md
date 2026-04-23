# Tree-Walk Interpreter: Quick Reference Guide

A one-page summary to orient you in the codebase.

---

## TL;DR: What This Interpreter Does

This is a **tree-walk interpreter** for the Lox language. It reads source code and executes it step by step:

1. **Tokenize** source code (Scanner) → tokens
2. **Parse** tokens (Parser) → abstract syntax tree (AST)
3. **Resolve** variables and scopes (Resolver) → scope analysis
4. **Execute** AST by visiting each node (Interpreter) → results

---

## File Organization

```
src/lox/
├── MAIN EXECUTION
│   ├─ main.cpp            Entry point, calls Lox::run()
│   └─ Lox.h/cpp           Orchestrates Scanner → Parser → Resolver → Interpreter
│
├── LEXICAL ANALYSIS
│   ├─ Scanner.h/cpp       Source code → Tokens
│   ├─ Token.h/cpp         Token structure
│   └─ TokenType.h         Token type enum
│
├── SYNTAX ANALYSIS
│   ├─ Parser.h/cpp        Tokens → AST (recursive descent)
│   ├─ Expr.h              Expression AST nodes (Binary, Literal, etc.)
│   └─ Stmt.h              Statement AST nodes (Var, If, While, etc.)
│
├── SEMANTIC ANALYSIS
│   ├─ Resolver.h/cpp      Variable scope resolution & validation
│   └─ Types.h             FunctionType, ClassType enums
│
├── EXECUTION
│   ├─ Interpreter.h/cpp   AST execution (visitor pattern)
│   ├─ Environment.h/cpp   Scope chain for variable lookup
│   ├─ Object.h            Runtime value type (variant)
│   ├─ RuntimeError.h      Runtime error representation
│   └─ ReturnVal.h         Exception wrapper for return values
│
├── RUNTIME TYPES
│   ├─ LoxCallable.h       Base interface for callable objects
│   ├─ LoxFunction.h/cpp   User-defined functions
│   ├─ LoxClass.h/cpp      Classes (callable)
│   ├─ LoxInstance.h       Class instances
│   ├─ LoxTrait.h          Trait definitions
│   └─ ClockFunction.h     Built-in clock() function
│
└── UTILITIES
    ├─ AstPrinter.h        Debug: print AST structure
    ├─ Makefile            Build configuration
    └─ test.txt            Test input file
```

---

## 10-Minute Understanding

### The Pipeline

```
"var x = 5; print x;"
        ↓
    [Scanner: char-by-char analysis]
        ↓
   [var, x, =, 5, ;, print, x, ;]
        ↓
    [Parser: token pattern matching]
        ↓
    [Var(x, 5), Print(x)]  ← AST
        ↓
   [Resolver: scope analysis]
        ↓
    x is at distance 0 in scope
        ↓
  [Interpreter: execute]
        ↓
    globals.define("x", 5)
    x = globals.get("x")
        ↓
        5
```

### Core Classes You'll See

| Class | Purpose | Key Method |
|-------|---------|------------|
| `Scanner` | Convert source → tokens | `scan_tokens()` |
| `Parser` | Convert tokens → AST | `parse()` |
| `Resolver` | Validate scopes & resolve vars | `resolve(Stmt)` |
| `Interpreter` | Execute AST | `interpret(Stmts)` |
| `Environment` | Manage variable scopes | `define()`, `get()`, `assign()` |
| `Expr` / `Stmt` | AST nodes | `accept(Visitor)` |
| `LoxCallable` | Functions & classes | `call()` |
| `Object` | Runtime values | `std::variant<...>` |

### Design Pattern: Visitor

Used everywhere for AST traversal:

```cpp
// In AST node (e.g., Binary):
Binary::accept(ExprVisitor& v) {
    return v.visitBinaryExpr(*this);
}

// In Interpreter (implements ExprVisitor):
VisitorReturn Interpreter::visitBinaryExpr(const Binary& expr) {
    // Execute operation
}

// Usage:
binary_expr.accept(interpreter);  // Dispatches to right method!
```

---

## Quick Lookup: Where is X?

| Question | Answer |
|----------|--------|
| **How is `var x = 5` parsed?** | [Parser.cpp](src/lox/Parser.cpp) - `varDeclaration()` |
| **How are variables looked up?** | [Interpreter.cpp](src/lox/Interpreter.cpp) - `lookUpVariable()` |
| **How are functions called?** | [Interpreter.cpp](src/lox/Interpreter.cpp) - `visitCallExpr()` |
| **How are scopes managed?** | [Environment.h/cpp](src/lox/Environment.h) - scope chain |
| **How are operators (+, -, *, /) executed?** | [Interpreter.cpp](src/lox/Interpreter.cpp) - `visitBinaryExpr()` |
| **How are classes defined?** | [Interpreter.cpp](src/lox/Interpreter.cpp) - `visitClassStmt()` |
| **How are class instances created?** | [LoxClass.cpp](src/lox/LoxClass.cpp) - `call()` |
| **How are methods called?** | [Interpreter.cpp](src/lox/Interpreter.cpp) - `visitGetExpr()` then `visitCallExpr()` |
| **How are if statements executed?** | [Interpreter.cpp](src/lox/Interpreter.cpp) - `visitIfStmt()` |
| **How are loops handled?** | [Interpreter.cpp](src/lox/Interpreter.cpp) - `visitWhileStmt()` |
| **How are breaks handled?** | [Interpreter.cpp](src/lox/Interpreter.cpp) - throws `BreakException` |
| **How are returns handled?** | [LoxFunction.cpp](src/lox/LoxFunction.cpp) - throws `ReturnVal` |
| **How are closures captured?** | [LoxFunction.h](src/lox/LoxFunction.h) - stores `closure` environment |

---

## Key Data Structures

### Token
```cpp
Token {
    type: TokenType,           // IDENTIFIER, NUMBER, IF, etc.
    lexeme: string,            // The actual text ("x", "42", "if")
    literal: Object,           // Value for strings/numbers
    line: int                  // Line number for errors
}
```

### Object (Runtime Value)
```cpp
Object = variant<
    monostate,                 // nil
    double,                    // Numbers
    string,                    // Strings
    bool,                      // Booleans
    shared_ptr<LoxCallable>,  // Functions/Classes
    shared_ptr<LoxInstance>,  // Class instances
    shared_ptr<LoxTrait>      // Traits
>
```

### Environment (Scope)
```cpp
Environment {
    values: map<string, Object>    // Variables in this scope
    enclosing: Environment*        // Parent scope (or nullptr)
}

// Scope chain: Global → Function → Block → ...
```

---

## Supported Language Features

✅ **Variables**: `var x = 5;`  
✅ **Arithmetic**: `2 + 3`, `x * y`  
✅ **Comparison**: `x > 5`, `x == 5`  
✅ **Logical**: `x && y`, `x || y`, `!x`  
✅ **Control Flow**: `if/else`, `while`, `for`, `break`  
✅ **Functions**: `fun add(a, b) { return a + b; }`  
✅ **Closures**: Functions capture enclosing scope  
✅ **Classes**: `class Dog { init() { } method() { } }`  
✅ **Inheritance**: `class Dog < Animal`  
✅ **Instance Variables**: `this.name`  
✅ **Traits/Mixins**: `class X with T`  
✅ **Ternary**: `condition ? true_val : false_val`  
✅ **Built-ins**: `clock()`, `print`  

---

## Execution Flow for Different Constructs

### Variable Declaration
```
var x = 5;
    ↓
Parser → Var(Token("x"), Literal(5))
    ↓
Resolver → declare/define "x" in current scope
    ↓
Interpreter → environment.define("x", 5)
```

### Function Call
```
add(2, 3)
    ↓
Parser → Call(Variable("add"), [Literal(2), Literal(3)])
    ↓
Resolver → Associate with distance info
    ↓
Interpreter:
  • Get LoxFunction("add")
  • Create new Environment
  • Bind parameters: a=2, b=3
  • Execute body
  • Return value
```

### Class Instantiation
```
Dog()
    ↓
Parser → Call(Variable("Dog"), [])
    ↓
Interpreter:
  • Get LoxClass("Dog")
  • Create LoxInstance("Dog")
  • Find "init" method
  • Call init bound to instance
  • Return instance
```

---

## Common Patterns in Code

### Visitor Pattern
All AST node traversal uses visitors. When you encounter:
```cpp
expr.accept(interpreter)
```
It calls the appropriate `visit*` method based on the expression type.

### Environment Chain
Variable lookup walks up the scope chain:
```cpp
environment → enclosing → enclosing → ... → global
```

### Error Handling
Three types of errors:
1. **Parse errors**: Caught by Parser, stored in `had_error`
2. **Resolution errors**: Caught by Resolver, stored in `had_error`
3. **Runtime errors**: Thrown as `RuntimeError`, caught in `Lox::runtimeError()`

### Exceptions for Control Flow
- **Break**: Throws `BreakException`, caught by while loop
- **Return**: Throws `ReturnVal`, caught by function call

---

## Testing

Uses **Google Test** framework:
- [tests/ScannerTest.cpp](tests/ScannerTest.cpp) - Tokenization tests
- [tests/ParserTest.cpp](tests/ParserTest.cpp) - Parsing tests
- [tests/ResolverTest.cpp](tests/ResolverTest.cpp) - Resolution tests
- [tests/InterpreterTest.cpp](tests/InterpreterTest.cpp) - Execution tests
- [tests/IntegrationTest.cpp](tests/IntegrationTest.cpp) - End-to-end tests

Build and run tests with:
```bash
cd src/lox
make test
```

---

## Development Workflow

To **add a new feature**:

1. **Understand the current flow** - Read ARCHITECTURE.md
2. **Modify Scanner** - If new token type needed
3. **Modify Parser** - Add grammar rule and parsing logic
4. **Add AST node** - New Expr/Stmt class if needed
5. **Modify Resolver** - Handle new construct (scopes, contexts)
6. **Modify Interpreter** - Implement visitor method to execute
7. **Write tests** - Add test cases in tests/

Then follow the code trace in CODE_TRACING_GUIDE.md to verify.

---

## Performance Characteristics

| Operation | Time | Notes |
|-----------|------|-------|
| Variable lookup | O(scope depth) | Optimized to O(1) via Resolver distance |
| Method lookup | O(inheritance chain) | Walks superclass chain |
| AST traversal | O(tree size) | Direct tree-walk interpretation |
| Loop iteration | Full re-evaluation | No bytecode optimization |

---

## Tips for Reading Code

1. **Start with main.cpp** - Trace the entry point
2. **Follow one statement** - Pick `var x = 5` and trace through all stages
3. **Look at visitor methods** - Each `visit*` shows how constructs execute
4. **Check Environment** - Understand scope chain for variable magic
5. **Read error messages** - They often contain hints about what failed

---

## Additional Resources

- **Complete Architecture**: [ARCHITECTURE.md](ARCHITECTURE.md)
- **Visual Diagrams**: [ARCHITECTURE_DIAGRAMS.md](ARCHITECTURE_DIAGRAMS.md)
- **Code Tracing**: [CODE_TRACING_GUIDE.md](CODE_TRACING_GUIDE.md)
- **Language Spec**: Likely in [test.txt](src/lox/test.txt) or project documentation

---

## Quick Debug Commands

```bash
# Build
cd src/lox && make

# Run REPL
./lox

# Run file
./lox program.lox

# Run tests
make test

# Print AST (if AstPrinter is working)
# Modify main.cpp, uncomment AstPrinter code
```

---

## "I'm Lost" Recovery Steps

1. ✅ Read this file (you're doing it!)
2. ✅ Look at ARCHITECTURE_DIAGRAMS.md for visual flow
3. ✅ Pick a simple feature in CODE_TRACING_GUIDE.md and trace it
4. ✅ Search for the class/method name in the codebase
5. ✅ Read the .h file first (interface) then .cpp file (implementation)
6. ✅ Follow one test case from [tests/](tests/) to understand expected behavior
7. ✅ Run interpreter with simple input and add debug output

---

**Welcome to the Tree-Walk Interpreter! Start with ARCHITECTURE.md for deep understanding.**
