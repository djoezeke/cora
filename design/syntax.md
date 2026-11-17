## Statements

### Labeled statements

A labeled statement labels a statement for control flow purposes.

#### Labels

```
1 - attr (optional) identifier :
2 - attr (optional) case constexpr :
3 - attr (optional) default:
```

### Expression statements

An expression statement is an expression followed by a semicolon.

### Compound statements

A compound statement or block groups a sequence of statements into a single statement.

### Selection statements

A selection statement chooses between one of several control flows.

### Iteration statements

An iteration statement repeatedly executes some code.

### Jump statements

A jump statement unconditionally transfers control flow.

### Declaration statements

A declaration statement introduces one or more identifiers into a block.

## Control Flow

### If statement

```
attr ﻿(optional) if constexpr(optional)
( init-statement ﻿(optional) condition ) statement-true

attr ﻿(optional) if constexpr(optional)
( init-statement ﻿(optional) condition ) statement-true else statement-false


```

### While Loop

```
attr(optional) while ( condition ) statement

```

### For Loop

```
1 - attr(optional) for ( init-statement condition(optional) expression(optional) ) statement

2 - attr(optional) for ( init-statement (optional) item-declaration : range-initializer ) statement
```

### Jump Statements

```
attr (optional) break;
attr (optional) continue;
attr (optional) return expression (optional) ;
attr (optional) return braced-init-list ;
attr (optional) goto identifier ;
```

### Functions

```
attr(optional) decl-specifier-seq(optional) declarator
virt-specifier-seq ﻿(optional) function-body
```

## Examples

```cora
// Line Comment
//! Line Doc Comment
/** Block Comment **/
/*! Block Doc Comment **/
/** Nested /** Block Comment **/ **/

struct Loc {
    x: u32,
    y: u32,
}

//! @brief This function adds two numbers together and returns the result.
//! @param a The first number to add.
//! @param b The second number to add.
//! @return The sum of a and b.
func add(a: u32, b: u32) -> u32 {
    return a + b;
}

func main() -> u32 {

    mut a: Loc = Loc{x: 10, y: 20};
    const c: u32 = add(a.x, a.y);


    mut arr: Loc[5] = [{x: 1, y: 2},
                      {x: 3, y: 4},
                      {x: 5, y: 6},
                      {x: 7, y: 8},
                      {x: 9, y: 10}];

    loc1 := arr[0];
    loc2 := arr[1];

    mut sum: u32 = 0;

    for (i: u32 = 0; i < 5; i++) {
        sum += add(arr[i].x, arr[i].y);
    }

    if (c > 20) {
        return 1;
    } else {
        return 0;
    }

    return;
}
```
