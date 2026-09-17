# Axos Syntax Specification

> [!NOTE]
> **axi** (`.axi`) is a modern systems programming language that serves as an ergonomic, high-level frontend to **C23**. The Community AVM acts as a transpiler, emitting 100% compliant, error-free C23 (with C11 backwards compatibility).

## 1. Language Philosophy & Core Design Principles

Axos is built upon three foundational pillars:

1. **Expressive Safety with Bare-Metal Performance:** Axos provides a modern syntax that eliminates boilerplate without introducing hidden runtime overhead. Abstractions in Axos are completely transparent.
2. **Explicit Memory Management:** There is no garbage collector and no hidden allocations. Allocation, deallocation, and lifetimes are managed explicitly by the programmer. axi embraces predictable performance.
3. **Direct 1:1 C23 Mapping:** axi is designed to emit beautiful, human-readable C23 code. Features like `nullptr`, `constexpr`, `typeof`, standard fixed-width types, and C23 attributes have direct representations in axi.

---

## 2. Lexical & Type System

### 2.1. Primitives
axi replaces C's ambiguous integer types with explicit, fixed-width equivalents mapping to `<stdint.h>` and standard C types.

* **Integers:** `i8`, `i16`, `i32`, `i64` (Signed) | `u8`, `u16`, `u32`, `u64` (Unsigned)
* **Floating Point:** `f32` (maps to `float`), `f64` (maps to `double`)
* **Booleans:** `bool` (maps to C23 `bool`)
* **Characters:** `char` (maps to C23 `char` / `char8_t`)
* **Strings:** `str` (alias for `[]const u8` slice). String literals infer to `str` by default, but can infer to `*char` for direct C interop. Includes `.as_ptr()` helper to convert to raw C strings.
* **Null:** `nullptr` (maps exactly to C23 `nullptr`)

### 2.2. Variable Declaration & Inference
Variables are declared using `let`. Mutability is explicit via the `mut` keyword. By default, bindings are immutable.

```rust
// axi (.axi)
let x: i32 = 42;          // Immutable
let mut y = 100;          // Mutable, type inferred as i32
let text = "Hello";       // Inferred as str ([]const u8)
let c_str: *char = "C";   // Explicitly a C string
```

```c
// Emitted C23 (.c)
const int32_t x = 42;
int32_t y = 100;
slice_u8_t text = { .ptr = (const uint8_t*)"Hello", .len = 5 };
const char* c_str = "C";
```

### 2.3. Pointers & Memory
Pointer syntax uses `*T`. Mutability applies to both the pointer and the data it points to.

```rust
// axi (.axi)
let mut value = 10;
let ptr: mut *i32 = &mut value; // Mutable pointer to mutable data
let const_ptr: *i32 = &value;   // Pointer to const data

// Array Slices (Fat Pointers mapping to a C struct)
let slice: []u8 = get_buffer();
```

```c
// Emitted C23 (.c)
int32_t value = 10;
int32_t* ptr = &value;
const int32_t* const_ptr = &value;

// Slices map to a standardized struct
typedef struct { const uint8_t* ptr; size_t len; } slice_u8_t;
slice_u8_t slice = get_buffer();
```

### 2.4. Aggregates (Structs, Unions, Enums)
axi unifies aggregate initialization and modernizes C's tag namespaces.

```rust
// axi (.axi)
struct Vector3 {
    x: f32,
    y: f32,
    z: f32,
}

// Tagged Enums (Sum Types)
enum Result {
    Ok(i32),
    Err(*char),
}
```

```c
// Emitted C23 (.c)
typedef struct Vector3 {
    float x;
    float y;
    float z;
} Vector3;

// axi tagged enums transpile to C unions with a tag
typedef struct Result {
    enum { RESULT_OK, RESULT_ERR } tag;
    union {
        int32_t ok;
        const char* err;
    } payload;
} Result;
```

### 2.5. Heap Memory Abstraction
Since axi embraces explicit memory management, it provides standard core bindings for dynamic allocation. This is critical for self-hosting the compiler (e.g., building dynamic AST node buffers).
* `core::mem::alloc(size: u64) -> mut *u8` (maps to C `malloc`)
* `core::mem::realloc(ptr: mut *u8, size: u64) -> mut *u8` (maps to C `realloc`)
* `core::mem::free(ptr: mut *u8)` (maps to C `free`)

For specialized memory workloads like compiling, the standard library also provides **Arena Allocators** (`core::mem::Arena`) to allow fast, bulk AST node allocations without individual `free` tracking.

---

## 3. Control Flow & Expressions

### 3.1. Branching (If / Else Expressions)
`if` statements in axi are expressions, allowing them to return values directly, transpiling cleanly to the C ternary operator `?:` or standard `if/else` blocks.

```rust
// axi (.axi)
let state = if count > 10 { "high" } else { "low" };
```

```c
// Emitted C23 (.c)
const char* state = (count > 10) ? "high" : "low";
```

### 3.2. Match Statements
axi's `match` acts as a powerful `switch` replacement. It forces exhaustiveness and transpiles to jump tables (`switch`) in C.

```rust
// axi (.axi)
match status {
    0 => print("Success"),
    1, 2 => print("Warning"),
    _ => print("Error"),
}
```

```c
// Emitted C23 (.c)
switch (status) {
    case 0:
        print("Success");
        break;
    case 1:
    case 2:
        print("Warning");
        break;
    default:
        print("Error");
        break;
}
```

### 3.3. Loops
axi supports `while` loops, infinite `loop` blocks, and `for` loops. Range-based `for` loops translate into optimized C-style index loops.

```rust
// axi (.axi)
for i in 0..10 {
    process(i);
}
```

```c
// Emitted C23 (.c)
for (int32_t i = 0; i < 10; ++i) {
    process(i);
}
```

### 3.4. Defer / Cleanup
axi uses `defer` to execute code at the end of the current scope, guaranteeing resource cleanup. This transpiles elegantly into duplicated cleanup code or C23 cleanup attributes if specified.

```rust
// axi (.axi)
let f = file_open("data.txt");
defer file_close(f);

process_file(f);
```

```c
// Emitted C23 (.c)
FILE* f = file_open("data.txt");
process_file(f);

// Deferred statements appended before scope exit
file_close(f);
```

---

## 4. Functions & Modules

### 4.1. Function Signatures
Functions use the `fn` keyword. Multiple return values are supported and transpile to anonymous structs.

```rust
// axi (.axi)
pub inline fn calculate(x: i32, y: i32) -> (i32, bool) {
    let sum = x + y;
    return (sum, sum > 0);
}
```

```c
// Emitted C23 (.c)
typedef struct { int32_t _0; bool _1; } _axi_tuple_int32_bool;

inline _axi_tuple_int32_bool calculate(int32_t x, int32_t y) {
    const int32_t sum = x + y;
    return (_axi_tuple_int32_bool){ sum, sum > 0 };
}
```

### 4.2. Modules and Visibility
Each `.axi` file is a module. Items are private by default and exported using `pub`. When transpiling, axi generates a corresponding `.h` header file containing only the `pub` signatures, and a `.c` file with the implementations.

```rust
// math.axi
import "system.axi"

pub fn add(a: i32, b: i32) -> i32 {
    return a + b;
}

fn internal_helper() {}
```

```c
// Generated math.h
#pragma once
#include <stdint.h>
int32_t math_add(int32_t a, int32_t b);

// Generated math.c
#include "math.h"
#include "system.h"

int32_t math_add(int32_t a, int32_t b) {
    return a + b;
}

static void internal_helper(void) {}
```

---

## 5. C Interop & Low-Level Escape Hatches

axi prioritizes frictionless C interoperability. You can include C headers directly, link seamlessly, and utilize C23 features like `#embed`.

### 5.1. Direct C Invocation
Functions defined in C can be declared using `extern "C"`.

```rust
// axi (.axi)
extern "C" fn malloc(size: u64) -> mut *u8;

let buffer = malloc(1024);
```

### 5.2. Native Binary Embeds
axi directly exposes the C23 `#embed` feature to load binary files into byte arrays at compile time.

```rust
// axi (.axi)
const SHADER_SOURCE: []u8 = @embed("shader.spv");
```

```c
// Emitted C23 (.c)
static const uint8_t _axi_embed_0[] = {
#embed "shader.spv"
};
const slice_u8_t SHADER_SOURCE = { .ptr = _axi_embed_0, .len = sizeof(_axi_embed_0) };
```

### 5.3. Inline C
For platform-specific hacks or utilizing complex C macros, axi allows raw C blocks.

```rust
// axi (.axi)
fn trigger_interrupt() {
    c_inline! {
        __asm__ volatile ("int $0x03");
    }
}
```

---

## 6. Transpilation Mapping Reference Table

| axi Construct (`.axi`) | Target C23 Equivalent (`.c`) | Notes |
| :--- | :--- | :--- |
| `let x = 5;` | `const int32_t x = 5;` | `const` by default. |
| `let mut x = 5;` | `int32_t x = 5;` | `mut` drops the `const` qualifier. |
| `nullptr` | `nullptr` | Native C23 support. |
| `*T` / `mut *T` | `const T*` / `T*` | Pointer mutability. |
| `[]T` (Slice) | `struct { const T* ptr; size_t len; }` | Standardized bounds-checked view. |
| `if cond { a } else { b }` | `cond ? a : b` | Transpiles to ternary if used as expression. |
| `match expr { ... }` | `switch (expr) { ... }` | Forced exhaustiveness in axi. |
| `for i in 0..N {}` | `for (int32_t i = 0; i < N; ++i) {}` | Direct counter mapping. |
| `defer foo();` | `foo();` | Expanded at all exit points of the scope. |
| `@embed("file")` | `#embed "file"` | Direct C23 preprocessor utilization. |
| `fn() -> (A, B)` | `struct { A _0; B _1; } fn(void)` | Tuples become anonymous structs. |
| `import "foo.axi"` | `#include "foo.h"` | Generates corresponding headers. |
| `pub fn func()` | `void module_func(void)` | `pub` implies external linkage (header). |
| `fn internal()` | `static void internal(void)` | Lack of `pub` implies `static` linkage. |



