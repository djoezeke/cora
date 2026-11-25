## Concepts

```cora
comptime func is_shape<T>() -> bool {
    return has_func(T, "area" , func(&T) -> f64);
}

comptime func is_drawable<T>() -> bool {
    return has_func(T, "draw" , func(&T) -> void);
}

comptime func is_drawableShape<T>() -> bool {
    return (is_shape<T>() && is_drawable<T>());
}

using Drawable<T> = is_shape<T> && is_drawable<T> ;

```

## Constraints

A generic bound can be either an interface name or a concept function.

Both use the same <T: ...> slot:

```cora
// bound by interface
func area<T: Shape>(s: T) -> f64 {
    return s.area();
}

// bound by concept function
func total<T: is_shape>(s: T) -> f64 {
    return s.area();
}
```

Multiple bounds :

```cora
func render<T: Shape && Drawable>(x: T) -> str {
    // T is guaranteed to provide area(), name(), and draw()
    x.draw();
    return x.name() + ":" + str(x.area())
}

func render<T: Shape || Drawable>(x: T) -> str {
    // T is guaranteed to provide area(), name(), and draw()
    x.draw();
    return x.name() + ":" + str(x.area())
}
```

### NTTP - NON-TYPE TEMPLATE PARAMETERS

```cora

comptime int get_point_x<P: Point>() {
    return P.x;
}
```
