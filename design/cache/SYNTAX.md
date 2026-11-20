# Syntax Examples

## Minimall Program

```cora
import "std::debug::println";

pub func main() -> i32 {
    let msg: u8[] = "Hello World!";
    println("{msg}");
    return 0;
}
```

### 2.3 Examples

```ferro
@inline(always)
@must_use
pub fn distance(x: f64, y: f64) -> f64 {
    sqrt(x * x + y * y)
}

[[deprecated(since = "0.2", note = "Use parse_new")]]
pub fn parse_old(s: str) -> i32 { parse_new(s) }

@allow(unused)
fn intentionally_unused() {}
```

### 5.2 Procedural macros

```ferro
@macro
fn make_getters(input: Syntax) -> Syntax {
    let fields = reflect(input).fields;
    syntax {
        $(for field in fields {
            pub fn $(field.name)(self: &Self) -> $(field.type) {
                self.$(field.name)
            }
        })
    }
}
```

# Part III — Worked integrated example

The following file is an illustrative integration sample. Some library calls (`Vec`, `task_scope`, `Writer`, `GlobalAllocator`, and platform functions) are standard-library APIs whose exact signatures are defined by the selected Ferro distribution.

```ferro
package ferro_demo;
module core {
    pub struct Point { pub x: f64, pub y: f64 }

    @op("+")
    pub fn add(self: Point, rhs: Point) -> Point {
        Point { x: self.x + rhs.x, y: self.y + rhs.y }
    }

    pub fn magnitude(p: &Point) -> f64 {
        sqrt(p.x * p.x + p.y * p.y)
    }
}

module io {
    import c "stdio.h";

    pub fn write_line(message: str) {
        unsafe {
            printf("%.*s\n", message.len, message.ptr);
        }
    }
}

module data {
    @comptime
    pub fn IsCopy<T>() -> bool {
        reflect(T).copy
    }

    @methods
    pub struct Sequence<T> {
        values: Vec<T, GlobalAllocator>
    }

    pub fn new<T>() -> Sequence<T> {
        Sequence { values: Vec<T, GlobalAllocator>.new(GlobalAllocator.new()) }
    }

    pub fn push<T>(self: &mut Sequence<T>, value: T) {
        self.values.push(value);
    }

    pub fn len<T>(self: &Sequence<T>) -> usize {
        self.values.len()
    }
}

module layout {
    @repr(C)
    pub struct Header {
        pub tag: u16,
        pub size: u32
    }

    @comptime
    pub fn HeaderLayout(info: TypeInfo, target: Target) -> Layout {
        let a = align_up(0, 2);
        let b = align_up(a + 2, 4);
        Layout {
            size = align_up(b + 4, 4),
            align = 4,
            offsets = [a, b]
        }
    }

    @layout(HeaderLayout)
    pub struct WireHeader {
        tag: u16,
        size: u32
    }
}

@comptime
fn IsNumeric<T>() -> bool {
    reflect(T).has_attribute("numeric")
}

fn twice<T>(value: T) -> T
    @require(IsNumeric<T>())
{
    value + value
}

@macro
fn make_accessors(input: Syntax) -> Syntax {
    let fields = reflect(input).fields;
    syntax {
        $(for field in fields {
            pub fn $(field.name)(self: &Self) -> $(field.type) {
                self.$(field.name)
            }
        })
    }
}

@derive(make_accessors)
struct Config {
    port: u16,
    workers: usize
}

@methods
struct Renderable {
    fn render(self: &Self);
}

@implements(Renderable)
struct Dot {
    position: core::Point
}

fn render(self: &Dot) {
    io::write_line("dot");
}

@extends(Renderable, mode = "flatten")
struct SpecialDot {
    position: core::Point,
    color: u32
}

@erase(Renderable)
fn render_plugin(item: Renderable) {
    item.render();
}

@deprecated(note = "Use checked_sum")
fn old_sum(values: &[i32]) -> i32 {
    let mut total = 0;
    for value in values { total += *value; }
    total
}

@must_use
fn checked_sum(values: &[i32]) -> Result<i32, Error> {
    let mut total = 0;
    for value in values {
        total = total.checked_add(*value)?;
    }
    Ok(total)
}

@diagnostic_scope
@deny(arithmetic_overflow)
fn safe_math(a: i32, b: i32) -> i32 {
    a + b
}

@optimize(3)
@inline(always)
fn hot_kernel(value: i256) -> i256 {
    value * 3i256 + 1i256
}

fn wide_float(value: f128) -> f128 {
    value * 1.5f128
}

@repr(C)
struct CPacket {
    kind: u16,
    length: u32,
    flags: u8
}

@align(64)
struct AlignedCounter {
    value: u64
}

fn aligned_size() -> usize {
    align_of(AlignedCounter)
}

fn pointer_read(bytes: &[u8], index: usize) -> u8 {
    if index >= bytes.len() { return 0; }
    @audit(invariants = "index was checked against slice length; pointer remains within allocation")
    unsafe {
        *bytes.as_ptr().add(index)
    }
}

@cfg(target.arch == "x86_64")
fn cpu_path() { x86_fast_path(); }

@cfg(target.arch == "aarch64")
fn cpu_path() { arm_fast_path(); }

@cfg(target.hosted)
fn platform_log(message: str) { io::write_line(message); }

@cfg(not(target.hosted))
fn platform_log(message: str) { serial_write(message); }

struct MoveOnly {
    resource: *mut u8
}

@drop
fn release(self: MoveOnly) {
    @audit(invariants = "resource was allocated by the matching allocator and is uniquely owned")
    unsafe { raw_dealloc(self.resource); }
}

fn move_example() {
    let first = acquire();
    let second = first;
    consume(second);
    // first is moved; using it here is rejected.
}

@copy
struct SmallPair {
    x: u32,
    y: u32
}

fn generic_copy<T>(value: T) -> T
    @require(reflect(T).copy)
{
    value
}

fn visit(values: &[i32], callback: fn(i32) -> i32) -> i32 {
    let mut total = 0;
    for value in values {
        total += callback(*value);
    }
    total
}

fn closure_examples() {
    let factor = 4;
    let shared = |x: i32| x * factor;

    let mut total = 0;
    let exclusive = |x: i32| { total += x; };

    let owned = acquire_buffer();
    let moved = @move |()| consume_buffer(owned);

    let pointer: fn(i32) -> i32 = |x| x + 1;
    use_callback(shared);
    use_mut_callback(exclusive);
    use_once_callback(moved);
    use_function(pointer);
}

@no_alloc
fn stack_checksum(data: &[u8]) -> u64 {
    let mut value = 0u64;
    for byte in data {
        value = value.wrapping_add(*byte as u64);
    }
    value
}

async fn pipeline(input: &[Record]) -> Result<Report, Error> {
    task_scope(|scope| {
        for chunk in input.chunks(128) {
            scope.spawn(async |part| process_chunk(part), chunk);
        }
        scope.join_all()
    })
}

fn custom_layout_example() -> usize {
    size_of(layout::WireHeader)
}

fn unsafe_copy(dst: *mut u8, src: *const u8, count: usize) {
    @audit(invariants = "both ranges are valid for count bytes, aligned as required, and non-overlapping")
    unsafe {
        copy_nonoverlapping(dst, src, count);
    }
}

@forbid(unsafe)
module safe_business_logic {
    pub fn calculate(values: &[i32]) -> Result<i32, Error> {
        checked_sum(values)
    }
}

@cfg(target.hosted)
pub fn main() -> i32 {
    platform_log("Ferro hosted demo");
    let point = core::Point { x: 3.0, y: 4.0 };
    platform_log(format("magnitude={}", core::magnitude(&point)));
    let result = twice(21i32);
    platform_log(format("twice={}", result));
    0
}

@cfg(not(target.hosted))
@export("_start")
@no_mangle
pub fn start() -> never {
    let result = stack_checksum(&[1, 2, 3, 4]);
    unsafe {
        platform_exit(result as i32);
    }
}
```

### Define a type and method

```ferro
struct User { name: String<GlobalAllocator> }

fn greet(self: &User) {
    println("Hello {}", self.name);
}
```

### Write a generic function

```ferro
fn identity<T>(value: T) -> T {
    value
}
```

### Require a compile-time property

```ferro
fn duplicate<T>(value: T) -> (T, T)
    @require(reflect(T).copy)
{
    (value, value)
}
```

### Borrow without transferring ownership

```ferro
fn inspect(value: &LargeObject) {
    println("inspected");
}
```

### Mutably borrow

```ferro
fn increment(value: &mut i32) {
    *value += 1;
}
```

### Allocate explicitly

```ferro
fn make_buffer<A: type>(allocator: A) -> Vec<u8, A> {
    Vec.new(allocator)
}
```

### Select a target implementation

```ferro
@cfg(target.endian == "little")
fn read_word(bytes: &[u8]) -> u32 { read_le(bytes) }

@cfg(target.endian == "big")
fn read_word(bytes: &[u8]) -> u32 { read_be(bytes) }
```

### Audit an unsafe operation

```ferro
@audit(invariants = "the range is initialized, valid, and uniquely writable")
unsafe {
    write_bytes(ptr, 0, length);
}
```

# nn

## 5. Unified comptime and metaprogramming

`@comptime fn` and `@macro fn` use exactly the syntax of runtime code and run in a deterministic, fuel-limited VM that sees the target, not the host.

**Syntax**

```
@comptime fn fib(n: u32) -> u64 {
    if n < 2 { n as u64 } else { fib(n - 1) + fib(n - 2) }
}
const F20: u64 = fib(20);                   // evaluated at compile time

@comptime(dual) fn clamp(x: i32, lo: i32, hi: i32) -> i32 {   // also compiled for runtime; pure subset only
    if x < lo { lo } else if x > hi { hi } else { x }
}

@macro(fn) fn square(x: Expr<i32>) -> Expr<i32> {
    quote!{ { let t = $x; t * t } }         // `t` is hygienic
}
let n = square!(3 + 1);                     // 16, not 3 + 1 * 3 + 1
```

## 7. OOP without trait, impl or interface

A method set is `type T { ... }`, a concept is an `@concept type`, and conformance is `type T: C { ... }`; these three forms give nominal and structural polymorphism, default methods, associated types and open or sealed extension.

**Syntax**

```
struct Circle { r: f32 }

type Circle {                                  // inherent method set of an existing type
    pub fn new(r: f32) -> Self { Circle { r } }
    pub const PI: f32 = 3.14159;
}

@concept type Shape {                          // concept: required and default members
    type Unit;                                 // associated type, required
    fn area(&self) -> Self::Unit;              // required method: no body
    fn name(&self) -> &str { "shape" }         // default method
}

type Circle: Shape {                           // nominal conformance
    type Unit = f32;
    fn area(&self) -> f32 { Self::PI * self.r * self.r }
}

fn first_area<S: Shape>(s: &S) -> S::Unit { s.area() }     // static dispatch, monomorphized
```

## Object Oriented Programmaing

## Meta Programming
