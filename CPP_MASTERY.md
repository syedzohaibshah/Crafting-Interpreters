# C++ Mastery Roadmap & Core Cheatsheet

A structured path from "I know C++ syntax" to **industry-grade Modern C++**, plus a dense cheatsheet of the language's core. Designed around your current stuck points: pointers, ownership, code structure, design decisions.

---

## Part 1 — The Roadmap (12 phases)

Each phase has: **Goal → Resources → Exercises/Projects → Done When**.
Don't skip exercises. Reading C++ and writing C++ are two different skills.

### Phase 0 — Tooling foundation (1 week)
You cannot learn modern C++ without modern tooling.

- **Compiler**: `clang++` and `g++` (use both, warnings differ). Always compile with `-Wall -Wextra -Wpedantic -Werror -std=c++20`.
- **Build system**: CMake (industry standard). Learn `add_library`, `target_link_libraries`, `target_include_directories`, `target_compile_features`.
- **Sanitizers**: `-fsanitize=address,undefined` — catches 80% of pointer bugs automatically. Run your VM under ASan **today**.
- **Debugger**: `lldb` (mac) — `b`, `r`, `bt`, `p`, `frame variable`.
- **Static analysis**: `clang-tidy`, `cppcheck`.
- **Formatting**: `clang-format` (.clang-format file in repo root).
- **Package manager**: `vcpkg` or `Conan` (you'll need this once projects grow).

**Done when**: your `cpp-version/` builds with CMake under `-Wall -Wextra -Werror -fsanitize=address,undefined` and passes.

---

### Phase 1 — The object model & value semantics (2 weeks) ⭐
This is the **single most important** topic in C++. Most of your pointer confusion lives here.

- Stack vs heap, automatic storage duration, object lifetime.
- Value vs reference vs pointer semantics — *what gets copied when*.
- Constructors: default, copy, move, converting, delegating.
- The **Rule of 0 / Rule of 5**.
- `const` correctness — `const T&`, `const` methods, `const` pointers vs pointer-to-const.
- References vs pointers — when each is correct.

**Resources**:
- Stroustrup, Part II (Basic Facilities) + Ch. on classes.
- *Effective Modern C++* (Scott Meyers) — Items 1–12, 17.
- cppreference.com: "Object lifetime", "Copy elision".

**Exercises**:
1. Write a `String` class from scratch with Rule of 5. Test with ASan.
2. Write a `Vector<T>` (std::vector clone) — manage your own buffer, growth, move ctor.
3. Trace, on paper, every constructor/destructor call for: `Foo f = make_foo(); g(f); g(std::move(f));`

**Done when**: you can explain *why* `std::vector<T>` is fast even though it copies, without saying "magic."

---

### Phase 2 — Pointers & ownership (2 weeks) ⭐ ← your stuck point
The mental model: **every resource has exactly one owner**. Pointers are about *who owns* and *who borrows*.

| You want to express... | Use |
|---|---|
| I own this; only one owner | `std::unique_ptr<T>` |
| Shared ownership (rare!) | `std::shared_ptr<T>` |
| Non-owning observer, may be null | raw pointer `T*` |
| Non-owning, never null | reference `T&` |
| Break a `shared_ptr` cycle | `std::weak_ptr<T>` |
| Optional value (not pointer) | `std::optional<T>` |
| Function arg, read-only big object | `const T&` |
| Function arg, will modify | `T&` |
| Function arg, will store/take ownership | `T` by value + `std::move`, or `unique_ptr<T>` |

**Hard rules**:
- `new`/`delete` in application code = code smell. Use `make_unique`/`make_shared`.
- `shared_ptr` is not a default — it's for genuinely shared lifetime. Reach for `unique_ptr` first.
- Returning `unique_ptr` by value is *the* idiomatic factory pattern.
- A function taking `const T&` does NOT own; passing `unique_ptr<T>` by value DOES transfer ownership.

**Exercise** (apply to your VM):
- Audit `vm.cpp` — for every pointer, ask: who owns the object? When does it die? Replace raw owning pointers with `unique_ptr`.

**Done when**: you can read a function signature and immediately know who owns what.

---

### Phase 3 — Move semantics & rvalue references (1 week)
- Lvalues vs rvalues vs xvalues vs prvalues (yes, really).
- `T&&`, `std::move`, `std::forward`.
- Move constructors / move assignment — when compiler auto-generates them, when it doesn't.
- Return value optimization (RVO/NRVO) and copy elision (mandatory in C++17).
- Perfect forwarding (templates).

**Resource**: *Effective Modern C++* Items 23–30. Read three times.

**Done when**: you can explain why `return std::move(local)` is usually wrong.

---

### Phase 4 — Templates & generic programming (3 weeks)
- Function & class templates, template argument deduction.
- Non-type template parameters.
- Specialization (full & partial), SFINAE.
- Variadic templates and parameter packs.
- **C++20 Concepts** — modern replacement for SFINAE; learn this *first* in 2026.
- `constexpr`, `consteval`, `if constexpr`.
- CRTP (Curiously Recurring Template Pattern).

**Resources**: *C++ Templates: The Complete Guide* (Vandevoorde) — reference, not cover-to-cover.

**Exercise**: implement a tiny `expected<T, E>` type using concepts.

---

### Phase 5 — STL deep dive (2 weeks)
You should know each container's complexity, when to choose it, and its iterator invalidation rules.

- Containers: `vector`, `array`, `deque`, `list`, `forward_list`, `map`, `unordered_map`, `set`, `unordered_set`, `string`, `string_view`, `span`.
- Iterators: 6 categories, ranges (C++20).
- Algorithms: `find`, `transform`, `accumulate`, `sort`, `partition`, `lower_bound`, `for_each`, the parallel variants.
- Function objects: lambdas, `std::function`, `std::bind` (avoid), capture rules.
- **C++20 Ranges**: `views::filter`, `views::transform`, pipelines.

**Done when**: you reach for an STL algorithm before writing a raw loop.

---

### Phase 6 — Error handling & resource management (1 week)
- RAII as a discipline, not a feature.
- Exceptions vs error codes vs `std::expected` (C++23) / `tl::expected`.
- Exception safety guarantees: no-throw, strong, basic.
- `noexcept` — when to use, performance impact on move ops.
- Industry positions vary: Google bans exceptions, most others allow. Know both.

---

### Phase 7 — Concurrency & memory model (2 weeks)
- `std::thread`, `std::jthread` (C++20), `std::async`, `std::future`/`promise`.
- Mutexes, `lock_guard`, `unique_lock`, `scoped_lock`.
- `std::atomic`, memory orderings (relaxed, acquire, release, seq_cst).
- Condition variables, the spurious-wakeup pattern.
- Coroutines (C++20) — `co_await`, `co_yield`. Hard but the future.

**Resource**: *C++ Concurrency in Action* (Anthony Williams) 2nd ed.

---

### Phase 8 — Design & architecture (ongoing) ⭐ ← your other stuck point
- **Idioms**: RAII, PIMPL, NVI (non-virtual interface), tag dispatch, type erasure (how `std::function` works).
- **Patterns**: but in C++, prefer composition + value types + lambdas over GoF heavyweight class hierarchies.
- **Polymorphism options** — pick the right tool:
  - Static (templates / concepts) — zero runtime cost.
  - Dynamic (virtual functions) — when you need runtime dispatch over an open set.
  - `std::variant` + `std::visit` — closed set of types, faster than virtual.
  - Type erasure — when interface is fixed but types are unrelated.
- **API design**: prefer free functions, return by value, accept iterators or ranges, fail at compile time when you can.
- Interfaces in headers, implementation in `.cpp`. Forward-declare aggressively.

**Resources**:
- *Effective C++* + *Effective Modern C++* (Meyers) — the canon.
- *API Design for C++* (Reddy).
- Talks: Sean Parent "Better Code" series. Klaus Iglberger on type erasure.

---

### Phase 9 — Performance & low-level (2 weeks)
- Cache lines, false sharing, branch prediction.
- `perf`, Instruments (mac), `valgrind --tool=callgrind`.
- Move vs copy benchmarks — `google-benchmark`.
- Compiler explorer (godbolt.org) — read assembly for your code.
- Undefined behavior: aliasing, signed overflow, lifetime — and why optimizers exploit it.

---

### Phase 10 — Testing & CI (1 week)
- GoogleTest or Catch2 (you already have GoogleTest in the tree).
- Property-based testing (`rapidcheck`).
- Fuzzing (`libFuzzer` — perfect for your VM!).
- CI: GitHub Actions matrix over {gcc, clang} × {Debug, Release+ASan}.

---

### Phase 11 — Capstone projects
Pick 2–3:
1. **Finish your bytecode VM** with: GC, classes, closures, full test suite, fuzzing harness.
2. A lock-free SPSC queue.
3. A small JSON parser using `std::variant` + ranges.
4. A coroutine-based async TCP echo server.
5. Contribute a small fix to LLVM, fmtlib, or nlohmann/json — read industry C++.

---

### Phase 12 — Stay current
- Follow: cppreference, isocpp.org, Herb Sutter's blog, Arthur O'Dwyer's blog.
- Watch: CppCon talks each year (start with "Back to Basics" track).
- Read: one item from *Effective Modern C++* per week, forever.

---

## Part 2 — Core Cheatsheet (the language in one page)

### Pointers & references — decision table
```cpp
void read_only(const Foo& f);          // read, no own, never null
void modify(Foo& f);                   // mutate, no own, never null
void maybe(const Foo* f);              // read, no own, may be null
void take(std::unique_ptr<Foo> f);     // transfer ownership in
std::unique_ptr<Foo> make();           // transfer ownership out (factory)
void share(std::shared_ptr<Foo> f);    // genuine shared lifetime
std::weak_ptr<Foo> observer;           // break shared_ptr cycles
```

### Smart pointer creation
```cpp
auto u = std::make_unique<Foo>(args);            // preferred
auto s = std::make_shared<Foo>(args);            // preferred
auto u2 = std::unique_ptr<Foo>{new Foo{...}};    // only if you must (custom deleter, etc.)
```

### Rule of 5 (or Rule of 0)
```cpp
class Buffer {
  T* data_; size_t n_;
public:
  Buffer();                                      // default
  ~Buffer();                                     // destructor
  Buffer(const Buffer&);                         // copy ctor
  Buffer& operator=(const Buffer&);              // copy assign
  Buffer(Buffer&&) noexcept;                     // move ctor
  Buffer& operator=(Buffer&&) noexcept;          // move assign
};
// Rule of 0: if all members are RAII types (vector, unique_ptr...), define NONE.
```

### const cheatsheet
```cpp
const int* p;          // pointer to const int — *p can't change
int* const p;          // const pointer to int — p can't change
const int* const p;    // both
int foo() const;       // method doesn't modify *this
```

### Move semantics
```cpp
std::string s = "hi";
std::string t = std::move(s);   // s now valid-but-unspecified; don't read it
auto v = make_vec();             // RVO: no copy, no move
return v;                        // NRVO usually; never write `return std::move(v);`
```

### Lambdas
```cpp
auto f = [](int x) { return x*2; };               // no capture
auto g = [&](int x) { count += x; };              // capture all by ref (be careful!)
auto h = [count](int x) mutable { count += x; };  // capture by value, allow mutate
auto i = [p = std::move(ptr)] { use(*p); };       // init capture (move into lambda)
```

### Templates basics + concepts (C++20)
```cpp
template <typename T>
T add(T a, T b) { return a + b; }

template <std::integral T>          // concept constrains T
T inc(T x) { return x + 1; }

template <typename T>
concept Printable = requires(T t) { std::cout << t; };
```

### `std::variant` for closed sums (great for ASTs / VM values)
```cpp
using Value = std::variant<double, bool, std::string, std::nullptr_t>;
std::visit([](auto&& v) { std::cout << v; }, value);
```

### STL container picks
| Need | Use |
|---|---|
| Sequence, fast push_back, random access | `vector` |
| Fixed-size sequence | `array` |
| Key→value, fast lookup, no order needed | `unordered_map` |
| Key→value, ordered iteration | `map` |
| String view, no ownership | `string_view` |
| Borrowed contiguous range | `span` |
| Maybe-a-value | `optional` |
| Either-or-error | `expected` (C++23) / `variant` |

### Idiomatic loops
```cpp
for (const auto& x : container)        // read
for (auto& x : container)              // mutate
for (auto x : small_trivial)           // copy when cheap
// C++20:
for (auto x : v | views::filter(odd) | views::transform(square))
```

### noexcept rules of thumb
- Move ctor/assign: **always** mark `noexcept` if you can — `std::vector` won't move otherwise.
- Destructors: implicitly `noexcept`.
- Don't sprinkle `noexcept` blindly — it's a contract.

### Headers vs implementation
```cpp
// foo.hpp — interface, forward declare what you can
#pragma once
class Bar;                              // forward decl, no #include "bar.hpp"
class Foo {
  std::unique_ptr<Bar> impl_;           // PIMPL
public:
  Foo();
  ~Foo();                               // must be defined in .cpp (Bar complete there)
  void do_thing();
};
```

### Common UB to internalize (do not do these)
- Reading uninitialized variables.
- Dereferencing a null/dangling pointer.
- Signed integer overflow.
- Out-of-bounds array access.
- Returning reference to a local.
- Modifying a `const` object via cast.
- Data race (two threads, ≥1 write, no sync).

### Diagnostic flags worth memorizing
```
-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wnon-virtual-dtor -Werror
-fsanitize=address,undefined          # debug builds
-O2 -DNDEBUG                           # release
-g                                     # debug info either way
```

---

## How to use this document

1. **Pick the phase you're on**, do its exercises, and tick "Done when".
2. **Pin the cheatsheet** — re-read it weekly until it's reflex.
3. As you work on your VM, every time you write a pointer, ask the decision table: *who owns this?*
4. Read 1 item from *Effective Modern C++* with morning coffee. 42 items × 1/week ≈ 10 months to finish — and you'll be solidly in the top 10% of C++ devs.

The path to mastery is reps + reading great code, not more tutorials. Your VM is the perfect lab — refactor it as your understanding grows.
