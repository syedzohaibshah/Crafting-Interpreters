# Tree-Walk Interpreter: Visual Architecture Guide

## 1. Execution Pipeline

```
┌─────────────────────────────────────────────────────────────────┐
│                       Source Code Input                         │
└──────────────────────────┬──────────────────────────────────────┘
                           │
                    ┌──────▼──────┐
                    │   SCANNER   │  Lexical Analysis
                    │  (Lox.cpp)  │  ✓ Character → Tokens
                    └──────┬──────┘
                           │
                    ┌──────▼───────────┐
                    │    Tokens List   │  Vector<Token>
                    │ [{, var, x, =... │
                    └──────┬───────────┘
                           │
                    ┌──────▼──────┐
                    │   PARSER    │  Syntax Analysis
                    │ (Parser.cpp)│  ✓ Tokens → AST
                    └──────┬──────┘
                           │
                    ┌──────▼─────────────┐
                    │   AST (Tree)       │  Vector<Stmt>
                    │ Var(x,Literal(42)) │
                    └──────┬─────────────┘
                           │
                    ┌──────▼───────────┐
                    │   RESOLVER       │  Semantic Analysis
                    │(Resolver.cpp)    │  ✓ Variable Resolution
                    └──────┬───────────┘  ✓ Scope Validation
                           │             ✓ Context Checking
                    ┌──────▼──────────────────┐
                    │  Locals Map (to Interp) │
                    │ {expr* → distance}      │
                    └──────┬──────────────────┘
                           │
                    ┌──────▼──────────┐
                    │  INTERPRETER    │  Execution
                    │(Interp.cpp)     │  ✓ AST → Results
                    └──────┬──────────┘
                           │
                    ┌──────▼────────────┐
                    │  Output/Results   │
                    │ Print / Return    │
                    └───────────────────┘
```

## 2. Component Dependency Graph

```
                    ┌─────────────┐
                    │   Lox       │ (Orchestrator)
                    └─┬───────┬───┘
                      │       │
              ┌───────▼┐    ┌─▼────────┐
              │Scanner │    │ Lox.cpp  │ (Error Handling)
              └───┬────┘    └──────────┘
                  │
         ┌────────▼──────────┐
         │   Vector<Token>   │
         └────────┬──────────┘
                  │
              ┌───▼───────┐
              │  Parser   │
              └───┬───────┘
                  │
         ┌────────▼──────────┐
         │   Vector<Stmt>    │ (AST)
         │   (Expr/Stmt)     │
         └────┬───────┬──────┘
              │       │
          ┌───▼──┐ ┌──▼──────────┐
          │Stmts │ │Resolver     │
          └──────┘ │             │
              ┌────▼───────────┐ │
              │Interpreter◄───┘ │
              │  + locals map◄───┘
              └────┬────────────┘
                   │
       ┌───────────┼───────────┐
       │           │           │
    ┌──▼──┐  ┌─────▼────┐  ┌──▼─────┐
    │Env  │  │LoxCallable  │Obj │
    └─────┘  │ ├─────────┼────┘
             │ LoxFunc   │
             │ LoxClass  │
             │ LoxInst   │
             └───────────┘
```

## 3. AST Node Structure

```
AST Nodes (All use Visitor Pattern):

EXPRESSIONS (return Object):
├─ Literal (const Object value)
├─ Binary (Expr* left, Token op, Expr* right)
├─ Unary (Token op, Expr* right)
├─ Grouping (Expr* expression)
├─ Conditional (Expr* cond, Expr* then, Expr* else)
├─ Variable (Token name)
├─ Assign (Token name, Expr* value)
├─ Logical (Expr* left, Token op, Expr* right) [Short-circuit]
├─ Call (Expr* callee, vector<Expr*> arguments)
├─ Get (Expr* object, Token property)
├─ Set (Expr* object, Token property, Expr* value)
├─ This (Token keyword)
└─ Super (Token keyword, Token method)

STATEMENTS (return void):
├─ Expression (Expr* expression)
├─ Print (Expr* expression)
├─ Var (Token name, Expr* initializer)
├─ Block (vector<Stmt*> statements)
├─ If (Expr* cond, Stmt* then, Stmt* else)
├─ While (Expr* cond, Stmt* body)
├─ Break ()
├─ Function (Token name, vector<Token> params, vector<Stmt*> body)
├─ Return (Token keyword, Expr* value)
├─ Class (Token name, vector<Function*> methods, ...)
└─ Trait (Token name, vector<Function*> methods)
```

## 4. Visitor Pattern Flow

```
                    AST Node
                        │
                        ▼
                  ┌──────────────┐
                  │ Expr/Stmt    │
                  │ .accept()    │
                  └──────┬───────┘
                         │ (dispatches based on node type)
                         ▼
            ┌────────────────────────────┐
            │ Visitor Implementation     │
            │ (e.g., Interpreter)       │
            │                           │
            │ visitBinaryExpr()         │
            │ visitVariableExpr()       │
            │ visitLiteralExpr()        │
            │ visitIfStmt()             │
            │ visitBlockStmt()          │
            │ ...                       │
            └────────────────────────────┘
                         │
                         ▼
                  [Operation Executed]
                         │
                         ▼
                  [Result Returned]
```

## 5. Scope Chain Example

```
STATIC VIEW (at time of function def):
┌──────────────────────────────────┐
│     Global Environment           │
│ ├─ var x = 5                     │
│ ├─ fun greet() { ... }           │
│ └─ class Dog { ... }             │
└──────────────────────────────────┘

DYNAMIC VIEW (during greet() execution):
┌──────────────────────────────────┐
│  Function Scope (greet)          │
│  enclosing = Global              │
│  ├─ var msg = "hello"            │
│  │                               │
│  ├─ { BLOCK SCOPE }              │
│  │  enclosing = Function         │
│  │  ├─ var x = 10  (shadows!)    │
│  │  └─ print x  ◄─ resolves to 10 │
│  │                               │
│  └─ print x  ◄─ resolves to 5    │
└──────────────────────────────────┘
         │ references
         ▼
┌──────────────────────────────────┐
│     Global Environment           │
│     x = 5                        │
└──────────────────────────────────┘

ACCESS WITH DISTANCE:
Local Variable: distance = 0
Enclosing Variable: distance = 1
Global Variable: distance = 2
```

## 6. Runtime Value System (Object Variant)

```
Object = std::variant<
    std::monostate,              │ "nil"
    double,                      │ 42, 3.14
    std::string,                 │ "hello", "world"
    bool,                        │ true, false
    std::shared_ptr<LoxCallable>,│ Function / Class
    std::shared_ptr<LoxInstance>,│ Class Instance
    std::shared_ptr<LoxTrait>    │ Trait Definition
>

LoxCallable (Polymorphic)
├─ LoxFunction
│  ├─ declaration: Function* (AST)
│  ├─ closure: Environment* (for closures)
│  └─ isInitializer: bool
│
└─ LoxClass
   ├─ name: string
   ├─ methods: map<string, LoxFunction*>
   ├─ superclass: LoxClass*
   └─ metaclass: LoxClass*

LoxInstance
├─ klass: LoxClass*
├─ fields: map<string, Object>
└─ methods accessed via klass
```

## 7. Parser Precedence Levels

```
LOWEST  ────────────────────────────────────────────  HIGHEST

Level 1:  assignment          (=)
Level 2:  conditional         (? :)
Level 3:  logical OR          (||)
Level 4:  logical AND         (&&)
Level 5:  equality            (==, !=)
Level 6:  comparison          (<, >, <=, >=)
Level 7:  addition/subtraction (+, -)
Level 8:  multiplication/div   (*, /)
Level 9:  unary               (!, -)
Level 10: call/access         (., ())
Level 11: primary             (literals, vars, parens)

Example: 2 + 3 * 4
         = Binary(2, +, Binary(3, *, 4))
         = 2 + 12
         = 14  ✓

Example: 1 + 2 ? 3 : 4
         = Conditional(Binary(1, +, 2), 3, 4)
         = Conditional(3, 3, 4)
         = 3  ✓
```

## 8. Class Instantiation Flow

```
SOURCE: var dog = Dog();

┌─────────────────────────────────────┐
│ Parse: Dog() → Call expr            │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│ Resolve: Verify Dog exists          │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│ Interpret: evaluate Call            │
│  • callee = LoxClass("Dog")         │
│  • Call LoxClass("Dog").call()      │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│ LoxClass.call() executed:           │
│  1. Create LoxInstance("Dog")       │
│  2. Find "init" method if exists    │
│  3. If init: bind to instance,      │
│     call with []  arguments         │
│  4. Return instance                 │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│ Instance bound to variable "dog"    │
│ dog = LoxInstance("Dog")            │
│ dog.fields = {name: "Fido", ...}    │
└─────────────────────────────────────┘
```

## 9. Method Call Flow

```
SOURCE: dog.bark()

┌──────────────────────────────┐
│ Parse: Get(dog, "bark")      │
│        Call(<that>, [])      │
└──────────┬───────────────────┘
           │
┌──────────▼──────────────────────────────┐
│ Interpret: evaluate Call                │
│  • callee = evaluate Get(...)           │
└──────────┬──────────────────────────────┘
           │
┌──────────▼──────────────────────────────┐
│ Interpret: evaluate Get                 │
│  • object = dog (LoxInstance)           │
│  • name = "bark"                        │
│  • dog.get("bark")                      │
└──────────┬──────────────────────────────┘
           │
┌──────────▼──────────────────────────────┐
│ LoxInstance.get("bark"):                │
│  1. Check fields["bark"] - not found    │
│  2. Check klass.methods["bark"] found!  │
│  3. bind(dog) - create bound method     │
│  4. Return bound LoxFunction            │
└──────────┬──────────────────────────────┘
           │
┌──────────▼──────────────────────────────┐
│ Bound LoxFunction.call():               │
│  1. Create scope, bind "this" = dog     │
│  2. Execute method body                 │
│  3. In body, "this" refers to dog       │
│  4. Return result                       │
└─────────────────────────────────────────┘
```

## 10. Error Recovery Mechanism

```
PARSE ERROR:
┌─────────────────────────────┐
│ Parser encounters error     │
│ Throws ParseError           │
└──────────┬──────────────────┘
           │
┌──────────▼──────────────────┐
│ Error handler catches       │
│ Reports error with line #   │
│ Calls synchronize()         │
└──────────┬──────────────────┘
           │
┌──────────▼──────────────────┐
│ synchronize() advances past │
│ error to next statement     │
│ (skips to ;, }, etc)        │
└──────────┬──────────────────┘
           │
┌──────────▼──────────────────┐
│ Parsing resumes            │
│ Had_error flag set         │
└──────────┬──────────────────┘
           │
┌──────────▼──────────────────┐
│ At Lox::run() level:        │
│ Check had_error flag       │
│ Skip resolver/interpreter  │
└─────────────────────────────┘
```

## 11. Token Anatomy

```
Token = { type, lexeme, literal, line }

Examples:

NUMBER: { NUMBER, "42", 42.0, 5 }
STRING: { STRING, "\"hello\"", "hello", 5 }
IDENT:  { IDENTIFIER, "x", nil, 5 }
PLUS:   { PLUS, "+", nil, 5 }
IF:     { IF, "if", nil, 5 }

Used by:
• Parser - token.type to check syntax rules
         - token.line for error reporting
• Scanner - created as lexemes are recognized
• Interpreter - token.literal for values
              - token.lexeme for variable names
```

## 12. Execution State Transitions

```
START
  │
  ├─→ [Scanner] ─────→ [Parse Error] ─────→ STOP (exit 65)
  │                          │ bad syntax
  │                          ▼
  │                  Lox::had_error = true
  │                    (skip rest)
  │
  ├─→ [Parser] ──────→ [Successful Parse]
  │       │
  │       ├─→ AST Generated
  │       │
  │       └─→ [Resolver] ──→ [Resolution Error] ─→ STOP
  │                               │ bad vars
  │                               ▼
  │                       Lox::had_error = true
  │
  ├─→ [Interpreter] ──→ [Runtime Error] ─→ STOP (exit 70)
  │       │                  │ type mismatch
  │       │                  ▼ zero division
  │       │          Lox::had_runtime_error = true
  │       │
  │       ├─→ BreakException (caught by loop)
  │       │
  │       ├─→ ReturnVal (caught by function call)
  │       │
  │       └─→ Normal Completion
  │
  └─→ STOP (exit 0)
```

## 13. Key File Structure

```
src/lox/
├─ Lox.h/cpp              Main orchestrator
├─ Scanner.h/cpp          Lexical analysis
├─ Token.h/cpp            Token representation
├─ TokenType.h            Token type enum
│
├─ Parser.h/cpp           Syntax analysis
├─ Expr.h                 Expression AST nodes
├─ Stmt.h                 Statement AST nodes
│
├─ Resolver.h/cpp         Semantic analysis
├─ Types.h                Type enums (FunctionType, ClassType)
│
├─ Interpreter.h/cpp      Execution engine
├─ Environment.h/cpp      Scope chain
├─ Object.h               Runtime value type
│
├─ LoxCallable.h          Callable interface
├─ LoxFunction.h/cpp      User functions
├─ LoxClass.h/cpp         Classes
├─ LoxInstance.h          Class instances
├─ LoxTrait.h             Trait definitions
│
├─ RuntimeError.h         Runtime error
├─ ReturnVal.h            Return value wrapper
│
├─ AstPrinter.h           Debug AST printing
├─ ClockFunction.h        Built-in clock() function
│
└─ build/                 Build output
└─ tests/                 Google Test files
```
