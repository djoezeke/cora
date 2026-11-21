Runtime Polymorphism Without interface / impl / dyn / abstract
Corrected. The language's item grammar is just struct, enum, fn, template, contract, macro, macro fn, comptime, plus c { } / cimport for C. There is no trait system, no impl block, no dyn type constructor. Method call syntax must be justified by the language as it actually exists.

The design below is strictly Zig's model, using only those primitives:

Zig mechanism	This language's equivalent
switch over tagged union	match over enum with payloads
free functions + UFCS	fn draw(c: *Circle, ...); c.draw(...) desugars
struct of fn pointers	struct DrawableVTable { draw: fn(*void, *Canvas), ... }
@fieldParentPtr	@parent_ptr builtin
anytype	template<T: Contract> or template<T: Any>
@hasDecl / @hasField	@hasFn(name, sig) reflection builtin
explicit fat pointer	struct DrawableRef { data: *void, vtable: *const DrawableVTable }
Everything is spelled out. A macro (macro fn) can generate the boilerplate, but the boilerplate is ordinary code that a human could have written.

1. Method Resolution: UFCS Only
There are no methods. There are functions and there is dot-call desugaring:

text
x.f(a, b)   →   f(x, a, b)      // when no local binding named `f` shadows
x.f         →   f(x)            // field read takes priority
Resolution rules:

If x is a struct and has a field f, x.f reads the field.

Otherwise, x.f(args) resolves to the function f visible in scope whose first parameter accepts x (by *T, *mut T, or by value for Copy types).

If two visible functions named f both accept x, the more specific *mut T > *T > by-value, and ambiguity is a compile error.

This is exactly Zig’s rule. It means:

rust
struct Circle { center: Point, radius: f32 }

fn draw(c: *Circle, canvas: *Canvas) { canvas.circle(c.center, c.radius); }
fn bounds(c: *Circle) -> Rect { Rect { center: c.center, half: c.radius } }

let c = Circle { center: (0.0, 0.0), radius: 1.0 };
c.draw(canvas);            // → draw(&c, canvas)
let r = c.bounds();        // → bounds(&c)
No impl, no self, no receiver syntax.

2. Primary Mechanism: Tagged Unions
Unchanged from the previous chapter, and still the default answer.

rust
enum Expr {
    Lit(f64),
    Var(&'static str),
    Bin { op: BinOp, lhs: *Expr, rhs: *Expr },
    Call { callee: &'static str, args: [*Expr] },
}

fn eval(e: *Expr) -> f64 {
    match e {
        Lit(n)                => n,
        Var(name)             => lookup(name),
        Bin { op, lhs, rhs }  => apply(op, eval(lhs), eval(rhs)),
        Call { callee, args } => dispatch_builtin(callee, args),
    }
}
Exhaustive, inline, no pointer chase, no allocation. This handles every polymorphic use case where the set of variants is known in the current crate.

For C-ABI-facing enums that may grow, open enum (from the pattern chapter) forces a _ arm at every match site.

3. Vtables Are Structs of Function Pointers
The heterogeneous case — implementors spread across crates, plugins loaded at runtime, implementors from C — cannot use a tagged union. It uses a manual vtable, spelled out.

rust
struct DrawableVTable {
    size:    usize,
    align:   usize,
    type_id: usize,                         // @typeId(T)
    draw:    fn(*void, *Canvas),
    bounds:  fn(*void) -> Rect,
    drop:    ?fn(*mut void),                // null when borrowed only
}

struct DrawableRef {
    data:   *void,
    vtable: *const DrawableVTable,
}
These are ordinary struct declarations. DrawableRef is an ordinary type — you write it wherever you would write dyn Drawable.

Dispatch shims (also ordinary functions)
rust
fn draw(d: DrawableRef, canvas: *Canvas) {
    (d.vtable.draw)(d.data, canvas);
}

fn bounds(d: DrawableRef) -> Rect {
    (d.vtable.bounds)(d.data);
}
Because of UFCS, d.draw(canvas) now works — it desugars to draw(d, canvas). The fat pointer looks identical to a concrete type at the call site. That is the entire ergonomic win of dyn, achieved with zero new syntax.

4. Building a Vtable
The vtable is a comptime const. The thunks cast *void back to the concrete pointer and call the free function.

rust
struct Circle { center: Point, radius: f32 }

fn draw(c: *Circle, canvas: *Canvas)        { canvas.circle(c.center, c.radius); }
fn bounds(c: *Circle) -> Rect               { Rect { center: c.center, half: c.radius } }

comptime const CIRCLE_DRAWABLE: DrawableVTable = DrawableVTable {
    size:    @sizeOf(Circle),
    align:   @alignOf(Circle),
    type_id: @typeId(Circle),
    draw:    |data, canvas| draw(@ptrCast(*Circle, data), canvas),
    bounds:  |data| bounds(@ptrCast(*Circle, data)),
    drop:    null,
};
Constructor
Construction is explicit — there is no implicit coercion from *Circle to DrawableRef:

rust
fn as_drawable(c: *Circle) DrawableRef {
    return DrawableRef { data: @ptrCast(*void, c), vtable: &CIRCLE_DRAWABLE };
}

let c = Circle { center: (0.0, 0.0), radius: 1.0 };
let d = as_drawable(&c);
d.draw(canvas);                    // → draw(d, canvas) → (d.vtable.draw)(d.data, canvas)
Trying to write let d: DrawableRef = &c; is a type error, with a hint:

text
error: expected `DrawableRef`, found `*Circle`
help: construct a fat pointer explicitly: `as_drawable(&c)`
This is the deliberate departure from Rust: the conversion site is visible.

5. Eliminating Boilerplate with macro fn
The vtable + constructor + shim functions above are mechanical. A macro fn generates them.

rust
#[derive(Drawable)]
struct Circle { center: Point, radius: f32 }
expands to:

rust
comptime const CIRCLE_DRAWABLE: DrawableVTable = DrawableVTable {
    size:    @sizeOf(Circle),
    align:   @alignOf(Circle),
    type_id: @typeId(Circle),
    draw:    |data, canvas| draw(@ptrCast(*Circle, data), canvas),
    bounds:  |data| bounds(@ptrCast(*Circle, data)),
    drop:    null,
};

fn as_drawable(c: *Circle) DrawableRef {
    return DrawableRef { data: @ptrCast(*void, c), vtable: &CIRCLE_DRAWABLE };
}
The derive macro does not generate draw or bounds — the user writes those as free functions. The macro requires that they exist, which is exactly what the Drawable contract checks. The derive is a macro fn operating on typed AST:

rust
macro fn derive_drawable(def: TypeDef) -> [Item] {
    let T = def.ident;
    quote! {
        comptime const __VTABLE_#T: DrawableVTable = DrawableVTable {
            size:    @sizeOf(#T),
            align:   @alignOf(#T),
            type_id: @typeId(#T),
            draw:    |data, canvas| draw(@ptrCast(*#T, data), canvas),
            bounds:  |data| bounds(@ptrCast(*#T, data)),
            drop:    null,
        };

        fn as_drawable(c: *#T) DrawableRef {
            DrawableRef { data: @ptrCast(*void, c), vtable: &__VTABLE_#T }
        }
    }
}
The user can write the derive by hand and get byte-identical code — nothing is hidden.

6. The Contract: Static Bounds for template
template<T: Drawable> monomorphizes, calling draw(x, canvas) and bounds(x) at instantiation. The Drawable contract tells the compiler those functions must exist for T.

rust
contract Drawable {
    fn is_satisfied(T: type) bool {
        return @hasFn("draw",   fn(*T, *Canvas))
            && @hasFn("bounds", fn(*T) -> Rect);
    }
}
@hasFn(name, sig) is a comptime reflection builtin: it returns true when a function named name with exactly that signature is visible. It is the language's @hasDecl, and it operates on the whole visible function set — the same set UFCS uses.

rust
template<T: Drawable>
fn render_batch(items: [T], canvas: *Canvas) {
    for item in items { item.draw(canvas); }    // → draw(&item, canvas), checked by contract
}
If T fails the contract, the error points at the contract definition, naming the specific failing query:

text
error: `Circle` does not satisfy contract `Drawable`
  in contract Drawable at drawable.lang:2:16
  required: function `draw` with signature `fn(*Circle, *Canvas)`
  help:   define `fn draw(c: *Circle, canvas: *Canvas)` in scope
The two dispatch paths, side by side
rust
// Static: monomorphized, inlined
template<T: Drawable>
fn render_batch(items: [T], canvas: *Canvas) {
    for item in items { item.draw(canvas); }
}

// Dynamic: one instance, indirect calls through vtable
fn render_batch_ref(items: [DrawableRef], canvas: *Canvas) {
    for item in items { draw(item, canvas); }
}
These are template and fn — no interface, no separate "trait object" concept. The user picks which one to call.

Any for duck typing
template<T: Any> is the anytype equivalent — no contract is checked; method presence is verified per call at instantiation.

rust
template<T: Any>
fn show(x: T) -> String {
    x.fmt()                                     // resolved at instantiation
}
7. Downcasting
The vtable carries type_id, so downcasting is safe once a DrawableRef exists:

rust
fn is<T: Any>(d: DrawableRef) -> bool {
    return d.vtable.type_id == @typeId(T);
}

fn downcast_ref<T: Any>(d: DrawableRef) -> Option<*T> {
    if d.vtable.type_id == @typeId(T) {
        return Some(@ptrCast(*T, d.data));
    }
    return None;
}
These are ordinary generic fns (generic via template), not methods, not inherent to any interface.

8. Owned Fat Pointers
DrawableRef borrows. For ownership, a second struct with a drop thunk:

rust
struct OwnedDrawable {
    data:   *void,
    vtable: *const DrawableVTable,
    drop:   fn(*mut void),
}

fn from_owned<T: Drawable>(v: T, alloc: *Allocator) -> OwnedDrawable {
    let p = alloc.alloc(T);
    *p = v;
    return OwnedDrawable {
        data:   @ptrCast(*void, p),
        vtable: &comptime const VTABLE_OF(T),    // vtable generated by macro for T
        drop:   |data| { let t: *T = @ptrCast(*T, data); @dropInPlace(t); alloc.free(t); },
    };
}

fn free(o: *OwnedDrawable) {
    (o.vtable.drop)(o.data);
}
The drop field on the vtable is ?fn(*mut void) — null for borrow-only vtables. Calling free on a DrawableRef’s vtable is impossible because DrawableRef has no drop field.

No Box, no Rc, no hidden allocator. The allocation is the caller’s, chosen explicitly.

9. C Interop: #[repr(C)] Vtables
An extern "C" interface was the previous design’s mistake. There is no such keyword now. A C-compatible vtable is just a #[repr(C)] struct that matches the C header.

rust
cimport "plugin.h" as plugin;
// plugin.h contains:
//   struct plugin_vtable { const char *(*name)(void *); int (*run)(void *, const uint8_t *, size_t); };
//   struct plugin { void *data; const struct plugin_vtable *vt; };

// The imported types are already #[repr(C)]; use them directly
fn plugin_run(p: plugin.plugin, input: []u8) -> i32 {
    let run = p.vt.run;
    return unsafe { run(p.data, input.ptr, input.len) };
}
Because the C type is plugin.plugin, p.vt.run is a field read (no UFCS), and the call goes through the C vtable. No new syntax; the C header did the work.

For a host-side vtable handed to C, use #[repr(C)]:

rust
#[repr(C)]
struct HostVTable {
    name: fn(*const void) -> *const u8,
    run:  fn(*mut void, *const u8, usize) -> i32,
}

#[repr(C)]
struct HostHandle {
    data:   *mut void,
    vtable: *const HostVTable,
}

extern "C" fn host_entry() -> HostHandle {
    return HostHandle { data: @ptrCast(*mut void, &STATE), vtable: &HOST_VTABLE };
}
extern "C" is the calling-convention annotation — it is a modifier on fn, not a new item kind. That is why no extern "C" interface is needed; the language never had interface to modify.

10. Full Example
Mixing all four mechanisms with only the primitives that exist.

rust
// ---- 1. Closed polymorphism: tagged union ----
enum Message {
    Ping,
    Text(&'static str),
    Binary { bytes: [u8], kind: u16 },
    Control { code: u8, payload: [u8] },
}

// ---- 2. Open polymorphism: manual vtable ----
struct HandlerVTable {
    size:    usize,
    align:   usize,
    type_id: usize,
    name:    fn(*const void) -> &'static str,
    handle:  fn(*mut void, Message) -> Result<(), HandleError>,
}

struct HandlerRef {
    data:   *void,
    vtable: *const HandlerVTable,
}

fn name(h: HandlerRef) -> &'static str {
    return (h.vtable.name)(h.data);
}

fn handle(h: HandlerRef, msg: Message) -> Result<(), HandleError> {
    return (h.vtable.handle)(h.data, msg);
}

// ---- 3. Contract: static bound over the same shape ----
contract Handler {
    fn is_satisfied(T: type) bool {
        return @hasFn("name",   fn(*T) -> &'static str)
            && @hasFn("handle", fn(*mut T, Message) -> Result<(), HandleError>);
    }
}

// ---- 4. One concrete handler ----
struct TextEcho { count: u64 }

fn name(_c: *TextEcho) -> &'static str { return "echo"; }

fn handle(c: *mut TextEcho, msg: Message) -> Result<(), HandleError> {
    match msg {
        Message::Text(s) => { c.count += 1; print(s); Ok(()) }
        _                => Err(HandleError::Unsupported),
    }
}

comptime const TEXT_ECHO_VTABLE: HandlerVTable = HandlerVTable {
    size:    @sizeOf(TextEcho),
    align:   @alignOf(TextEcho),
    type_id: @typeId(TextEcho),
    name:    |data| name(@ptrCast(*TextEcho, data)),
    handle:  |data, msg| handle(@ptrCast(*mut TextEcho, data), msg),
};

fn as_handler(h: *mut TextEcho) -> HandlerRef {
    return HandlerRef { data: @ptrCast(*void, h), vtable: &TEXT_ECHO_VTABLE };
}

// ---- 5. Static path ----
template<H: Handler>
fn run_hot(handlers: [*mut H], msg: Message) -> Result<(), HandleError> {
    for h in handlers { handle(h, msg.clone())?; }
    return Ok(());
}

// ---- 6. Dynamic path ----
fn run_dyn(handlers: [HandlerRef], msg: Message) -> Result<(), HandleError> {
    for h in handlers { handle(h, msg.clone())?; }
    return Ok(());
}

// ---- 7. Dispatcher: tagged union for the common case, vtable for the rest ----
struct Dispatcher {
    fast: [*mut TextEcho],
    slow: [HandlerRef],
}

fn dispatch(d: *mut Dispatcher, msg: Message) -> Result<(), HandleError> {
    match msg {
        Message::Ping => { print("pong"); return Ok(()); }
        Message::Control { code, .. } => return handle_control(d, code),
        _ => {}
    }
    return run_dyn(d.slow, msg);
}

fn handle_control(d: *mut Dispatcher, code: u8) -> Result<(), HandleError> {
    match code {
        0x01 => run_hot(d.fast, Message::Control { code, payload: [] }),
        _    => Err(HandleError::UnknownControl),
    }
}

// ---- 8. C-implemented handler, loaded at runtime ----
extern "C" fn load_plugin(path: *const u8) -> Option<HandlerRef> {
    let h = unsafe { dlopen(path) };
    if h.is_null() { return None; }
    let make = unsafe { dlsym(h, c"make_handler") };
    let pair: plugin.handler_pair_t = unsafe { transmute(make) };
    // Construction from a C-supplied vtable is unsafe
    return Some(unsafe { HandlerRef { data: pair.data, vtable: pair.vtable } });
}
Every layer is a struct, an enum, a fn, a comptime const, or a contract — the same primitives every other chapter uses.

11. Updated Comparison
Concern	Rust	Zig	C++	This design
Closed polymorphism	enum + match	tagged union + switch	variants + visit	enum + match
Open polymorphism	trait + dyn	struct of fn pointers	virtual	struct of fn pointers
Coercion to fat pointer	implicit (&x as &dyn T)	none	implicit (upcast)	none — as_handler(&x) explicit
Vtable visibility	opaque	user-written	compiler-generated	user-written, comptime const
Method syntax	inherent + trait methods	UFCS	member functions	UFCS (x.f(a) → f(x, a))
Static dispatch	impl Trait / generics	anytype	templates	template<T: Contract>
Duck typing	none	anytype	SFINAE (errored)	template<T: Any>
Bound failure location	at call site	never (runtime error)	at instantiation	at contract definition
Downcast	Any::downcast_ref	@fieldParentPtr	dynamic_cast	downcast_ref<T> via type_id
Ownership of fat ptr	Box<dyn>, Rc<dyn>	caller-owned	unique_ptr<Base>	DrawableRef borrows; OwnedDrawable owns, drop thunk
C vtable interop	manual FFI	extern + @cImport	extern "C"	#[repr(C)] struct + cimport
Boilerplate removal	#[derive] proc macro	none	macros	#[derive] = macro fn
The design has no interface, no impl, no dyn, no abstract. Vtables are structs; dispatch is a function call; contracts are comptime predicates; UFCS makes the call site look the same for concrete and fat-pointer types. Everything the previous version did with sugar, this version does with the four primitives the language already has.

design syntax for control flow , jump statement, declaration statements, compound statements and more for language
Control Flow, Jump Statements, Declarations, Blocks & More
1. Design Rules
Five rules govern every construct in this chapter. They are derived from the constraints already established (expression-oriented match, comptime phase discipline, unsafe boundary at C interop, no impl/trait/dyn).

Expression-oriented. Every block is an expression; every control-flow construct that has a "value shape" is an expression. Statements are expressions whose value is discarded.

Structured exits only. No goto. Every jump out of a block names its target (the enclosing loop or block via a label) or the enclosing function (return). Deferred actions run at scope exit, in every path, including ?.

comptime is a phase qualifier, not a sub-language. comptime if, comptime for, comptime while, comptime match, comptime { } reuse the same grammar as their runtime counterparts. The phase is what changes: only the taken branch is compiled, loops must be bounded.

No implicit control-flow side effects. A block's value is its trailing expression; if the block ends in a ;-terminated statement, the value is () and that is a type error if the caller needs a value. This is the same "no hidden work" rule used by dyn construction and c_macro.

Items nest in blocks. fn, struct, enum, template, contract, macro, macro fn, comptime const are legal at any block scope — matching Rust and Zig. Items are declarations, not statements; they have no value.

2. Blocks, Scoping, and Compound Statements
2.1 Plain block
rust
{
    let x = 1;
    let y = 2;
    x + y                    // block value: 3
}
A block introduces a new lexical scope.

Bindings inside are not visible outside.

The block's value is the trailing expression (no ;).

If the last item is a ;-terminated statement, the block value is (). If the caller requires a non-unit type, this is a compile error, not an implicit unit coercion.

2.2 Kinds of blocks
Block	Purpose	Runs at	Can ?	Can return	Can break
{ }	scope / compound statement	runtime	yes	yes	to enclosing loop only
'l: { }	labeled block expression	runtime	yes	yes	break 'l v
comptime { }	compile-time block	comptime	no	no	break (with label) only
unsafe { }	unchecked operations	runtime	yes	yes	to enclosing loop only
c { }	C declarations	declaration site	no	no	no
comptime { } and unsafe { } are covered in §12–§13. c { } was defined in the C interop chapter.

2.3 Shadowing
Shadowing is allowed, but only when the shadowing binding introduces a different declaration in the same or a nested scope. This matches Rust and is the model that all prior chapters assume.

rust
let x = 5;
let x = x + 1;              // shadowing, allowed — new binding, type inferred as i32
{
    let x = "str";          // nested-scope shadow, different type — allowed
}
// outer x is still i32 with value 6
Rules:

Same-scope shadowing is allowed only if the new binding has the same type (so it is a "rebinding with the same type" — no type confusion).

Same-scope shadowing with a different type is an error and must be renamed.

This is the middle ground: ergonomic for the common let x = f(x); pattern, but prevents the "wait, x is a string now?" class of bug.

2.4 Nested items
rust
fn process(input: Input) -> Output {
    struct Cache { map: [u32]bool }          // nested item
    fn hash(x: u32) -> u32 { x * 2654435761 } // nested fn
    const SEED: u32 = 0xDEADBEEF;            // nested const

    let mut c = Cache { map: [] };
    c.map[hash(SEED)] = true;
    // ...
}
Nested items do not capture lexical variables (no closures — the language has no closures; anonymous fn pointers are allowed, but they see only their parameters and the module scope, same as Zig).

3. Declarations
3.1 Binding declarations — let
text
let        binding [':' type] ['=' expr] ';'
let mut    binding [':' type] ['=' expr] ';'
let pattern '=' expr else diverging_block ';'
let is immutable. let mut is mutable. Type annotation is optional (inferred); required only at extern "C" boundaries, static, and when inference is ambiguous.

rust
let x = 5;                          // immutable, i32
let mut count = 0;                  // mutable
let name: *const u8 = c"hello";     // explicit type at FFI boundary
let Point { x, y } = origin;        // destructuring let
let Some(v) = opt else { return; }; // refutable let with diverging else
Rules:

let with = may be omitted for uninitialized declarations; the binding must be initialized exactly once before any read, and the compiler tracks this (Rust’s definite-initialization analysis). Uninitialized reads are a compile error, never UB.

let mut with no = is initialized to @undef(T) only when T has no destructor and is unsafe to read before init — which is enforced: reads before init are compile errors, so this is only useful for out parameters from C. For safety, prefer @zeroed(T) for FFI and let x = f(); otherwise.

3.2 Compile-time constants — const and comptime const
rust
const N: usize = 32;                  // comptime-evaluated, inlined at use sites
comptime const VTABLE = ...;          // redundant-but-explicit form
const is comptime-evaluated by default; the value is inlined at use sites. comptime const is accepted as an explicit documentation form; the comptime is not required and does not change semantics. Requires the initializer to be comptime-pure.

Const is legal at function scope and at module scope.

3.3 Runtime module-level storage — static
rust
static COUNTER: AtomicU64 = AtomicU64::new(0);
static BUFFER: [u8; 4096] = @zeroed([u8; 4096]);
static is runtime storage, one instance per program.

static mut is not in the language. Mutable statics must be static with an interior-mutability type (AtomicU64, Mutex<T>) — the type system enforces this; static x: i32 cannot be mutated from a shared context.

static with #[repr(C)] or at an extern "C" site is visible to C by symbol name.

3.4 Item declarations (in any scope)
rust
fn name(params) -> ret { }
struct Name { ... }
enum Name { ... }
template<...> fn name(...) { }
contract Name { fn is_satisfied(T: type) bool { ... } }
macro name { ... }
macro fn name(...) -> ... { ... }
Item declarations have no value (they are not expressions). They must appear at statement position with or without a trailing ; — the ; is optional and has no meaning (matching Rust’s item-position rule).

3.5 Multiple bindings in one let
Only one binding per let is allowed. let x = 1, y = 2; is a syntax error. This avoids the C ambiguity around int a, *b, c[3];.

4. if / if let / let-chains
4.1 if as an expression
rust
let sign = if n < 0 { "neg" } else if n == 0 { "zero" } else { "pos" };
Rules:

Both branches must produce the same type (or one must be !, the never type — allowing if c { 1 } else { panic!() }).

else is required when the if is used as an expression of non-unit type.

else if chains are left-associative.

if as a statement with no else requires both branches to be ().

4.2 if let
rust
if let Some(x) = opt {
    use(x);
}

if let Some(x) = opt {
    use(x);
} else {
    absent();
}
if let pat = e { A } else { B } desugars to:

rust
match e {
    pat => { A }
    _   => { B }
}
Except: the desugaring is not user-visible, and A’s bindings do not escape. else is optional but the if let still occupies a match internally.

4.3 let-chains
Consecutive if let, let, and boolean conditions combine with &&. All conditions must be true; the binding is available to the right of each let.

rust
if let Some(user) = lookup(id)
    && user.active
    && let Ok(token) = validate(user.token)
    && token.expires_at > now
{
    grant(user, token);
}
Rules:

&& is the only combiner. There is no || at the top level of a let-chain (|| exists only inside a condition, not between lets — otherwise binding scopes are unclear).

The desugaring is right-nested if let; short-circuit is preserved.

let in a chain is refutable (let Some(x) = ...), not a plain let (which would make the whole chain useless).

No else if let chaining requirement: else { } runs when any condition fails.

4.4 comptime if (phase-gated)
rust
comptime if cfg(target_os) == "windows" {
    use_windows();
} else {
    use_unix();
}
The condition must be comptime-evaluable.

Only the taken branch is parsed and type-checked. This is the tool for platform-specific code — untaken branches can reference names that don’t exist on this platform.

The result is itself a comptime-evaluable expression if the branches are.

4.5 if comptime e (assertion, not gate)
rust
if comptime compute_flag() { A } else { B }
Asserts that compute_flag() is comptime-evaluable; a violation is a compile error.

Both branches are still type-checked (no gating). Use this when you want to document and enforce that a condition is known at compile time, without changing what is compiled.

Unlike comptime if, it is a comptime assertion, not a phase gate.

5. while / while let
5.1 while
rust
while !queue.is_empty() {
    process(queue.pop());
}
The loop body must be (); break exits with no value.

Loops return (). If you want a while that produces a value, use loop with break value (§7).

5.2 while let
rust
while let Some(item) = iter.next() {
    process(item);
}
Desugars to:

rust
loop {
    match iter.next() {
        Some(item) => { process(item); }
        None       => break,
    }
}
5.3 while let-chains
rust
while let Some(x) = stack.pop()
    && x > 0
{
    use(x);
}
Same &&-chain rules as if let. The chain is re-evaluated each iteration; the loop exits when any condition fails.

5.4 comptime while (bounded)
rust
comptime while SIZE < 128 {
    SIZE = comptime next_size(SIZE);   // must terminate; the compiler enforces a step budget
}
Runs during compilation, subject to the comptime step budget (default 1,000,000 steps).

SIZE must be a comptime-mutable binding (comptime const is not mutable; use comptime let mut).

Non-termination is a compile error naming the loop.

6. for
for in this language is structural only — matching the design’s "no hidden runtime work" rule. It iterates over arrays, slices, and ranges. Custom iteration uses while let.

6.1 Over arrays, slices, ranges
rust
for item in slice        { process(item); }
for item in array        { process(item); }
for i in 0..n            { process(i); }       // exclusive
for i in 0..=n           { process(i); }       // inclusive

for mut item in slice    { item += 1; }        // mutable binding
for Point { x, y } in pts { plot(x, y); }      // destructuring
Desugars to a bounds-checked index loop; the bounds check elides when the compiler proves it is redundant.

6.2 for does not call user code
There is no IntoIterator/Iterator protocol. A custom collection is iterated via a free function that returns a slice, or via while let with a next function:

rust
// Collection that exposes a slice view
fn view(m: *Map) []Entry { ... }
for e in view(m) { ... }

// Collection that is genuinely streaming — no slice available
while let Some(item) = next(&mut iter) { ... }
This composes with template (the next contract is duck-typed) and with cimport-ed iterators (the C iterator's next is a normal unsafe fn call).

6.3 comptime for (unrolled)
rust
comptime for i in 0..N {
    table[i] = comptime compute(i);   // N must be comptime; loop is unrolled
}
Bounds must be comptime-known.

The loop body is unrolled. This is the tool for array table construction, dimension enumeration, and vectorized code generation.

Step budget applies (§5.4).

7. loop
rust
loop {
    let ev = wait_event();
    match ev {
        Event::Quit => break,
        Event::Tick => tick(),
        _           => {}
    }
}
Infinite loop; exits only via break or return.

loop has type () if it has no break value.

7.1 loop with a value
rust
let answer = loop {
    let x = try_get();
    match x {
        Some(v) if v.is_valid() => break v,  // loop produces v
        Some(_) => continue,
        None    => {}
    }
};
break expr gives loop the type of expr. All break arms must agree on the type.

loop with a value must not fall through to its end; the trailing expression of the body is a compile error (the value would be discarded).

8. match (Referenced)
Full grammar was defined in the pattern chapter. Recap of position in control flow:

match is an expression.

comptime match e { } — phase-gated; the scrutinee and patterns are comptime-evaluable, and the compiler performs exhaustive checking on the comptime-known variant set.

match interacts with break/continue/return/? inside arms exactly as in an ordinary block.

rust
let label = match code {
    200..=299 => "ok",
    400..=499 => "client",
    500..=599 => "server",
    _         => "other",
};
9. Labels
Labels bind to blocks and loops. Syntax: 'name.

rust
'outer: loop {
    'inner: while cond {
        if escape { break 'outer; }
        if skip   { continue 'inner; }
    }
}
9.1 Labeled block
rust
let v = 'get: {
    if x < 0 { break 'get 0; }
    if x > 100 { break 'get 100; }
    x
};
Labeled block with break 'label expr is the value-producing early exit. It is the only way to have an early exit from a block expression other than return (which exits the function).

9.2 Labels compose with match, if, for
rust
'outer: for row in grid {
    for cell in row {
        if cell.is_blocking() { break 'outer; }
    }
}
9.3 Label rules
Labels are lexically scoped, one per block/loop.

Duplicate labels in nesting are allowed (inner shadows outer), and the compiler warns.

break 'label is a compile error if 'label is not in an enclosing position.

Labels are not values; they cannot be stored or passed.

10. Jump Statements
10.1 break
text
break 'label? expr? ';'
No label: exits the nearest loop or labeled block.

With label: exits the labeled loop or block.

With expr: only valid on loop and labeled blocks; the expr is the value of the construct.

break; (no expr) is valid everywhere.

break expr; (no label) on a while / for is a compile error — those constructs cannot produce a value.

10.2 continue
text
continue 'label? ';'
No label: next iteration of the nearest loop.

With label: next iteration of the named loop.

Valid only inside a loop body (or a labeled block that is a loop body).

language full syntax code example with comments.
Full Syntax Walkthrough: A Complete Program
Below is one program, logpipe.lang, that exercises every construct the design has defined — C interop, the preprocessor, comptime, macros, patterns, contracts, vtables, control flow, and declarations — with a comment on every section.

rust
// =========================================================================
// logpipe.lang — a small log-processing pipeline.
// One file, every feature. Read top-to-bottom.
// =========================================================================


// =========================================================================
// §1. C INTEROP — typed header imports
// =========================================================================
//
// `cimport` runs the embedded C preprocessor on the header, then translates
// declarations into this language's type system. All symbols are `unsafe` at
// call sites. `with { ... }` is the ONLY way to configure the preprocessor.
//
//   - `defines:`   feeds `-D` to the preprocessor before parsing
//   - `include:`   adds hermetic include search paths (no implicit `-I.`)
//
// Nothing here pollutes the host macro table; only `cimport`ed names exist.

cimport "stdio.h"  as stdio with {
    defines: { "BUFSIZE": "4096" },
};

cimport "string.h" as cstr;
cimport "time.h"   as time;


// =========================================================================
// §2. C PREPROCESSOR — structured directives
// =========================================================================
//
// `c_define`  : object-like macro; also a safe host-language constant.
// `c_macro`   : function-like macro; textual; requires `unsafe` at call site.
// `c_if`      : balanced conditional over the C macro table.
//               ONLY the taken branch is parsed.
// `c { ... }` : verbatim embedded C; the only place `#` appears as a token.

c_define LOG_MAX_LINE = 4096;          // usable as a constant AND as a #define
c_define LOG_VERSION  = "1.0";

// Function-like macros expand textually. Double-eval footgun is preserved,
// so `unsafe` is required at the call site.
c_macro CLAMP(v, lo, hi) = ((v) < (lo) ? (lo) : ((v) > (hi) ? (hi) : (v)));
c_macro STRINGIFY(x)     = #x;

// Platform branch: both branches use the SAME syntax; only the chosen one
// is even parsed. `file_handle_t` will exist after this statement on both.
c_if C_defined("_WIN32") {
    c {
        #include <windows.h>
        typedef HANDLE  file_handle_t;
        typedef DWORD   file_size_t;
    }
    c_macro CLOSE_FILE(h) = CloseHandle(h)
} c_else {
    c {
        #include <unistd.h>
        #include <sys/stat.h>
        typedef int    file_handle_t;
        typedef off_t  file_size_t;
    }
    c_macro CLOSE_FILE(h) = close(h)
}


// =========================================================================
// §3. COMPTIME — phase qualifier, not a sub-language
// =========================================================================
//
// `comptime` runs a pure, bounded subset at compilation time.
// Pure: no I/O, no `extern`, no `unsafe`, no heap.
// Bounded: step budget, non-termination is a compile error.

fn fib(comptime n: usize) usize {
    if n < 2 { return n; }
    return fib(n - 1) + fib(n - 2);
}

const FIB_TABLE_LEN: usize = 32;

// `comptime for` unrolls the loop and evaluates each iteration at compile time.
// `'blk:` is a labeled block — `break 'blk value` is the early-exit form.
comptime const FIB_TABLE: [usize; FIB_TABLE_LEN] = 'blk: {
    let mut t: [usize; FIB_TABLE_LEN] = @zeroed([usize; FIB_TABLE_LEN]);
    comptime for i in 0..FIB_TABLE_LEN {
        t[i] = comptime fib(i);
    }
    break 'blk t;
};


// =========================================================================
// §4. DECLARATIVE MACRO — hygienic, Rust-style
// =========================================================================
//
// `$x:fragment` specifiers are mandatory for captures.
// Hygiene is the default; call-site identifiers cannot be captured.

macro vec {
    ($($x:expr),* $(,)?) => {
        {
            let mut temp = Vec::new();
            $( temp.push($x); )*
            temp
        }
    };
}


// =========================================================================
// §5. TYPES — enums and structs
// =========================================================================

// Closed enum: exhaustive matching required; adding a variant is a
// compile error at every `match` site.
enum LogLevel { Trace, Debug, Info, Warn, Error, Fatal }

// Open enum: parser can see new values from config files or C. Every
// `match` must provide a `_` catch-all. `..` in the declaration marks it.
open enum LogKind { Plain, Json, Syslog, .. }

// `#[repr(C)]` makes the layout C-compatible — used for FFI structs.
#[repr(C)]
struct LogLine {
    level:   LogLevel,
    kind:    LogKind,
    msg:     []u8,       // owned slice
    elapsed: u64,        // nanoseconds
}


// =========================================================================
// §6. CONTRACTS + TEMPLATES — static polymorphism
// =========================================================================
//
// A `contract` is a `comptime` predicate over types. It answers: "does T
// provide these functions?" `@hasFn` is the reflection builtin — it looks
// at the same free-function set that UFCS resolves against.

contract Formatter {
    fn is_satisfied(T: type) bool {
        return @hasFn("format", fn(*T, LogLine) -> []u8);
    }
}

// Two concrete formatter types — no `impl`, no `interface`, no `dyn`.
struct JsonFormatter  { pretty: bool }
struct PlainFormatter { prefix: []u8 }

// Free functions. `f.pretty` is a field read; `format(f, line)` is a call.
// UFCS lets us write `f.format(line)` and it desugars to `format(f, line)`.
fn format(f: *JsonFormatter,  line: LogLine) -> []u8 { return line.msg; }
fn format(f: *PlainFormatter, line: LogLine) -> []u8 { return line.msg; }

// `template<F: Formatter>` monomorphizes per concrete type. The contract
// guarantees `format(f, line)` exists; failure is reported at contract site.
template<F: Formatter>
fn format_batch(f: *mut F, lines: []LogLine) -> [][]u8 {
    let mut out = Vec::new();
    for line in lines {
        out.push(f.format(line));     // → format(f, line), checked by contract
    }
    return out.to_slice();            // → to_slice(&out)
}


// =========================================================================
// §7. VTABLES — dynamic polymorphism without trait/dyn/impl
// =========================================================================
//
// A vtable is an ordinary struct of function pointers. A fat pointer is an
// ordinary struct { *void, *vtable }. Dispatch is an ordinary function call.
// UFCS means the call site looks identical to a concrete-type call.

struct FormatterVTable {
    size:    usize,
    align:   usize,
    type_id: usize,
    format:  fn(*const void, LogLine) -> []u8,
}

struct FormatterRef {
    data:   *void,
    vtable: *const FormatterVTable,
}

// Dispatch shim: an ordinary `fn`. This one line is the entire "method".
fn format(r: FormatterRef, line: LogLine) -> []u8 {
    return (r.vtable.format)(r.data, line);
}

// A vtable constant — comptime-generated by hand (or by `#[derive(...)]`,
// which is just a `macro fn` producing this exact code).
comptime const JSON_VTABLE: FormatterVTable = FormatterVTable {
    size:    @sizeOf(JsonFormatter),
    align:   @alignOf(JsonFormatter),
    type_id: @typeId(JsonFormatter),
    format:  |data, line| format(@ptrCast(*JsonFormatter, data), line),
};

// Explicit coercion. `as_formatter(&jf)` is required; `&jf` alone does
// not coerce to `FormatterRef` — that is the whole safety story.
fn as_formatter(f: *JsonFormatter) FormatterRef {
    return FormatterRef { data: @ptrCast(*void, f), vtable: &JSON_VTABLE };
}


// =========================================================================
// §8. PATTERN MACRO — custom patterns, expanded before exhaustiveness
// =========================================================================

// A `pattern macro` returns a `Pattern` AST. It is fully expanded before
// the exhaustiveness checker runs, so `not_error!()` participates in
// reachability the same as an inline or-pattern.
pattern macro fn not_error() -> Pattern {
    return quote! { LogLevel::Trace | LogLevel::Debug | LogLevel::Info };
}


// =========================================================================
// §9. ERROR HANDLING — Result + `?`
// =========================================================================
//
// `?` is the propagation operator. It requires the enclosing function to
// return `Result<T, E>`. `?` runs deferred actions (Drop, etc.) on the
// way out — never "returns early without unwinding".

fn read_line(fd: file_handle_t) -> Result<LogLine, ReadError> {
    let n = unsafe { read_file(fd, buf_ptr(), LOG_MAX_LINE) };
    if n < 0 { return Err(ReadError::Io); }

    // `?` is usable in any `if let` / `let` expression, because it
    // desugars to `match ... { Ok(x) => x, Err(e) => return Err(e) }`.
    let level = parse_level(buf_slice(0, n))?;
    let kind  = parse_kind(buf_slice(0, n))?;

    return Ok(LogLine { level, kind, msg: buf_slice(0, n).to_vec(), elapsed: 0 });
}


// =========================================================================
// §10. EXTERN "C" — callable FROM C, using unsafe only where the ABI requires
// =========================================================================

extern "C" fn log_pipe_version() -> *const u8 {
    return c"1.0";           // `c"..."` is a NUL-terminated literal
}

// C calls this with a raw pointer + length. Inside, we validate before
// touching memory, then drop to `unsafe` only for the pointer walk.
extern "C" fn log_pipe_process(line: *const u8, len: usize) -> i32 {
    // Null + zero-length guards are the C-ABI contract.
    if line.is_null() || len == 0 { return -1; }

    // Unsafe block: reading through a C-provided pointer.
    let mut buf = Vec::with_capacity(len);
    unsafe {
        for i in 0..len {
            buf.push(*line.add(i));
        }
    }

    let ll = LogLine {
        level:   LogLevel::Info,
        kind:    LogKind::Plain,
        msg:     buf.to_slice(),
        elapsed: 0,
    };

    // Dynamic dispatch through the vtable — looks like a normal call.
    let mut jf = JsonFormatter { pretty: false };
    let r = as_formatter(&mut jf);
    let out = format(r, ll);          // → format(r, ll) → (r.vtable.format)(r.data, ll)

    // Unsafe block: writing to C stdio.
    unsafe {
        stdio.fwrite(out.ptr as *const _, 1, out.len, stdio.stdout);
        stdio.fflush(stdio.stdout);
    }
    return 0;
}


// =========================================================================
// §11. MAIN — full control-flow showcase
// =========================================================================

fn main(argc: i32, argv: **u8) -> i32 {

    // ---- if-let-chain: `&&` combines refutable `let` with booleans ----
    // The desugaring is right-nested `if let`; short-circuit is preserved.
    if argc >= 2
        && let Some(path) = arg_at(argv, 1)
        && path.len > 0
    {
        process_file(path);
    } else {
        // No `else if` needed; absence of chain success falls here.
        eprint("usage: logpipe <file>");
        return 1;
    }

    // ---- labeled block: value-producing early exit ----
    // The label lets `break 'blk value` short-circuit the block without
    // returning from the enclosing function. `budget` ends up as `usize`.
    let budget: usize = 'blk: {
        let raw = compute_budget();
        if raw < 10      { break 'blk 10; }
        if raw > 10_000  { break 'blk 10_000; }
        raw
    };

    // ---- comptime if: only the taken branch is compiled ----
    // Here both branches use `format_batch` monomorphized for a different
    // type; only one gets code-generated at build time.
    comptime if FIB_TABLE[10] > 100 {
        let mut jf = JsonFormatter { pretty: true };
        let _ = format_batch(&mut jf, initial_batch());
    } else {
        let mut pf = PlainFormatter { prefix: c"log> " };
        let _ = format_batch(&mut pf, initial_batch());
    }

    // ---- labeled outer loop, `while let` with a boolean condition ----
    // `'outer: while let` — break 'outer exits BOTH loops at once.
    'outer: while let Some(batch) = next_batch() {
        for line in batch {
            // Pattern macro `not_error!()` expands to
            //   LogLevel::Trace | LogLevel::Debug | LogLevel::Info
            // BEFORE exhaustiveness checking — it counts as real coverage.
            match line.level {
                not_error!()                        => {}
                LogLevel::Warn                      => { log_warn(line); }
                LogLevel::Error | LogLevel::Fatal   => {
                    report(line);
                    break 'outer;                    // stop on first error
                }
            }
        }
    }

    // ---- `loop` with a value, labeled block, break-with-value ----
    // Every `break` arm must agree on the type (`i32` here).
    let retries: i32 = loop {
        if try_flush()        { break 0; }
        if budget == 0        { break -1; }
        budget -= 1;
    };

    // ---- `for` over a range, `continue` on a predicate ----
    for i in 0..retries {
        if is_retry_blackout(i) { continue; }
        retry_flush(i);
    }

    // ---- `comptime while` with explicit bound ----
    // Evaluated during compilation; must terminate under the step budget.
    comptime const MAX_TAG_LEN = 'blk: {
        let mut n: usize = 0;
        comptime while FIB_TABLE[n] < 1000 { n += 1; }
        break 'blk n;
    };

    // ---- final: return through every exit path with `?` ----
    return finish(retries);
}


// =========================================================================
// §12. Helper stubs (declarations only; body omitted)
// =========================================================================
//
// These exist so the sections above compile in isolation. Each would be
// implemented in a real program using the same primitives demonstrated.

fn arg_at(argv: **u8, i: usize) -> Option<[]u8>          { return None; }
fn process_file(path: []u8)                             { }
fn compute_budget() -> usize                            { return 0; }
fn initial_batch() -> []LogLine                         { return []; }
fn next_batch() -> Option<[]LogLine>                    { return None; }
fn report(line: LogLine)                                { }
fn log_warn(line: LogLine)                              { }
fn try_flush() -> bool                                  { return true; }
fn retry_flush(i: i32)                                  { }
fn is_retry_blackout(i: i32) -> bool                    { return false; }
fn finish(retries: i32) -> i32                          { return retries; }
fn read_file(fd: file_handle_t, buf: *mut u8, n: usize) -> i64 { return 0; }
fn parse_level(bytes: []u8) -> Result<LogLevel, ReadError> { return Ok(LogLevel::Info); }
fn parse_kind(bytes: []u8) -> Result<LogKind, ReadError>   { return Ok(LogKind::Plain); }
fn buf_ptr() -> *mut u8                                 { return @null; }
fn buf_slice(start: usize, end: usize) -> []u8          { return []; }
fn eprint(s: []u8)                                      { }
fn eprint_bytes(s: []u8)                                { }
What Each Section Demonstrates
§	Feature	Language rule exercised
1	cimport with defines	Typed header import; hermetic include search; all symbols unsafe at call sites
2	c_define, c_macro, c_if/c_else, c { }	Object-like macros are safe; function-like require unsafe; only the taken c_if branch is parsed; # appears only inside c { }
3	comptime, comptime for, comptime const, labeled block	Phase qualifier; unrolled loops; value-producing early exit via break 'blk v
4	macro vec	Hygienic declarative macro; $x:expr specifiers
5	enum, open enum, #[repr(C)] struct	Exhaustive closed enum; .. marks open (catch-all required at every match); C layout
6	contract, template<F: Formatter>, UFCS	Static polymorphism via type-shape check; f.format(line) desugars to format(f, line); failure reported at contract site
7	FormatterVTable, FormatterRef, format(shim), as_formatter	Dynamic polymorphism as an explicit struct of fn pointers + fat pointer; no dyn, no impl, no interface; coercion is explicit
8	pattern macro fn not_error()	Custom patterns expanded before exhaustiveness checking
9	Result, ?	Error propagation runs deferred actions on the way out; usable inside if let / let chains
10	extern "C" fn, c"...", unsafe { }	C ABI in both directions; unsafe confined to the pointer walk and C calls
11	if-let-chain, labeled block, comptime if, while let, for, loop with value, continue, break 'outer, ?	Every control-flow construct, including the two distinct break forms (break in a loop; break 'blk v in a labeled block) and the two distinct while let forms (bare, and as a chain)
—	let, let mut, const, static, nested items	Immutable-by-default bindings; const is comptime-evaluated; items nest at block scope
Things Worth Noticing
No impl, no trait, no dyn, no interface, no abstract anywhere in the file — yet the program has both static (via template<F: Formatter>) and dynamic (via FormatterRef + vtable) polymorphism.

Every unsafe is paired with a C call or a raw-pointer walk. Nothing else in the language forces unsafe.

The preprocessor is confined to §1–§2 and to cimport configuration. The host macro system (macro vec) never sees c_macro names, and # only exists inside c { }.

Every control-flow construct has a value shape where it can: if/else, match, loop with break v, labeled blocks with break 'blk v. while and for are unit-valued by design.

comptime reuses the same grammar — comptime if, comptime for, comptime while, comptime match are the runtime forms with a phase qualifier, not separate syntax.

Two break forms, not one. break in a loop exits with unit; break 'blk v in a labeled block exits with a value. This is what makes labeled blocks a lightweight return from a region without inventing a new keyword.

Server busy. Usage limit reached. Your limit will reset on Nov 21, 2026 at 17:00.
