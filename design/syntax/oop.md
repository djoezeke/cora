## Struct

A struct contain beha

## interface

A struct is an interface if any of its funcs lacks a body.
If a struct has fields, it cannot be used as a bound (it's a concrete data type).
If it has only body-less funcs, it's a pure interface.

```cora
struct Shape {
    func(s: &Shape) area() -> f64;                         // abstract — no body
    func(s: &Shape) name() -> str { return "shape" }       // default body allowed
}

struct Drawable {
    func(d: &Drawable) draw() -> void;
}
```

Interfaces may carry default method bodies.
Interfaces may not carry fields — that would be data inheritance, which we don't have.

Interfaces may themselves satisfy other interfaces (interface composition, still no data):

```cora
struct ShapeDrawable : Shape, Drawable {
    // no fields, no new abstract funcs required — inherits the contract
}
```
