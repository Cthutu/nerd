# Chapter 12: Generics

[Previous: Loops with `for`](11-Loops.md)

[Next: Methods and Traits](13-Methods-and-Traits.md)

Generics let one function or data type work with different concrete types.
Instead of writing separate versions for integers, strings, and each of your
own types, declare **type parameters** and supply types when the declaration
is used. The compiler builds concrete versions for the combinations you use.

This chapter covers generic functions and data types. It does not require
traits or method definitions; those follow in Chapter 13.

## A function with a type parameter

Declare type parameters in square brackets after `fn`:

```nerd
identity :: fn [T] (value: T) -> T {
    return value
}

main :: fn () -> i32 {
    prn(identity("hello"))
    return identity(42)
}
```

This prints `hello` and returns `42`. `T` names a type chosen at each call.
The first call uses `string`; the second uses `i32`. In each concrete version,
the parameter and return value have the same type.

`T` is a binding for a type, rather than a runtime value. Its name is your
choice; `T` is simply a common convention. Function value parameters still
appear in parentheses, and `::` still binds the function itself to a name.

Expression function syntax works too: the same body can be written
`identity :: fn [T] (value: T) => value`, with its return type inferred.

## Inferred and explicit type arguments

The compiler can usually infer a generic function's type arguments from its
value arguments. To choose them explicitly, put the types between the
function name and the call parentheses:

```nerd
identity :: fn [T] (value: T) -> T {
    return value
}

main :: fn () -> i32 {
    wide := identity[i64](42)
    return wide.as(i32)
}
```

This returns `42`. `[i64]` chooses `T` as `i64`, so the untyped literal `42`
is accepted as an `i64` argument and `wide` has type `i64`.

In `identity(42)`, the otherwise unconstrained integer argument becomes `i32`.
Choosing a generic instantiation is different from converting its result:
`identity[i64](42)` selects the `i64` function, while
`identity(42).as(i64)` converts the result of the `i32` function.

Do not rely on an expected return type to choose the type arguments. Supply
them explicitly when the value arguments do not determine the intended type.
Concrete numeric types still follow Chapter 3's conversion rules; generics
do not introduce implicit conversions between them.

## Types used only inside the function

A type parameter need not appear in a value parameter. Such a call needs an
explicit type argument because there is no argument from which to infer it:

```nerd
storage_size :: fn [T] () -> usize {
    return T.size
}

main :: fn () -> i32 {
    return storage_size[i32]().as(i32)
}
```

This returns `4`, the storage size of an `i32` in bytes. `T.size` uses the
general storage-size property from [Chapter 3](03-Primitive-Types-and-Expressions.md)
on the type supplied as `T`. The function can use
`T` wherever the chosen type is needed, including declarations, pointer and
array element types, return types, and type operations such as `.size`.
Calling `storage_size()` cannot infer `T`; its `usize` return type does not
identify the type whose size you want.

## Using a concrete function as a value

You can select a concrete version without calling it immediately:

```nerd
identity :: fn [T] (value: T) -> T {
    return value
}

main :: fn () -> i32 {
    int_identity := identity[i32]
    return int_identity(42)
}
```

This returns `42`. `identity[i32]` is a function value taking an `i32` and
returning an `i32`. Parentheses perform the call; the type argument list
alone selects the concrete version.

## Inferring an element type

Inference can look inside parameter types, such as `[]T` and `^T`:

```nerd
first_or :: fn [T] (values: []T, fallback: T) -> T {
    return on values.count > 0 => values[0] else fallback
}

main :: fn () -> i32 {
    values := [20, 99]
    first := first_or(values, 0)
    second := first_or([], 22)
    return first + second
}
```

This returns `42`. The first call infers `T` as `i32` from the fixed array's
element type and accepts the array as a slice. In the second call, the fallback
supplies the element type for the empty slice. The function returns its fallback
without indexing when the slice is empty.

Repeated uses of `T` refer to the same type. The slice element and fallback
must therefore be compatible with one concrete `T`; they cannot independently
choose unrelated types.

Pointer parameters preserve access to the caller's storage:

```nerd
replace :: fn [T] (location: ^T, value: T) {
    location^ = value
}

main :: fn () -> i32 {
    answer := 20
    replace(^answer, 42)
    return answer
}
```

This returns `42`. The `^i32` argument determines `T`, and assignment through
the pointer changes `answer`. Generic code follows the same pointer and
lifetime rules as ordinary functions.

## Generic plexes

Put type parameters after `plex` to define a family of record types:

```nerd
Cell :: plex [T] {
    value T
}

IntCell :: Cell[i32]

main :: fn () -> i32 {
    number: IntCell = { value: 42 }
    text: Cell[string] = { value: "text" }
    prn(text.value)
    return number.value
}
```

This prints `text` and returns `42`. `Cell[i32]` and `Cell[string]` are
concrete types with differently typed fields. `IntCell :: Cell[i32]` binds
another name to one concrete type; it does not introduce a separate kind of
cell. The literals omit their type names because the declarations supply them.

Supply all type arguments when naming a concrete generic type. `Cell` alone
is a template, rather than a concrete storage type. A type argument can itself
be a concrete generic type: `NestedCell :: Cell[Cell[i32]]` defines a cell whose
value is an integer cell. `Cell[IntCell]` names the same type; the inner alias
is optional.

The usual plex rules still apply, including field access, initialization,
layout, and copying. Generic type application does not allocate storage by
itself; a variable or allocation provides storage for a concrete value.

## Several type parameters

A declaration can have several type parameters. A generic function can also
construct a generic type:

```nerd
Pair :: plex [A, B] {
    first A
    second B
}

make_pair :: fn [A, B] (first: A, second: B) -> Pair[A, B] {
    return { first, second }
}

main :: fn () -> i32 {
    pair := make_pair(21, "answer")
    prn(pair.second)
    return pair.first * 2
}
```

This prints `answer` and returns `42`. The call infers `A` as `i32` and `B`
as `string`, producing a `Pair[i32, string]`. Type arguments are positional:
`Pair[string, i32]` would give the fields different types.

The return type supplies the literal's type. Since each parameter has the
same name as its field, `{ first, second }` uses Chapter 5's shorthand for
`{ first: first, second: second }`.

Both parameters must be determined for each call. Supplying only
`make_pair[i32](...)` is an error; it does not ask the compiler to infer just
the remaining parameter.

You can supply both arguments explicitly with
`make_pair[i32, string](21, "answer")`. Type arguments can themselves be
generic applications, such as `Cell[i32]`.

The names `A` and `B` belong to their own declarations. The function's type
parameters are passed as arguments to `Pair`; they do not refer to bindings
shared with the plex declaration.

## Generic enums

Enums can use type parameters in their payloads:

```nerd
Choice :: enum [A, B] {
    Left(A)
    Right(B)
}

main :: fn () -> i32 {
    choice: Choice[i32, string] = Left(42)
    return on choice {
        Left(value) => value
        Right(_) => 0
    }
}
```

This returns `42`. The concrete type determines both payload types: `Left`
carries an `i32`, and `Right` carries a `string`. The declaration supplies
context for the unqualified constructor `Left(42)`. Matching and payload
bindings follow the rules from Chapter 9.

This is an ordinary enum with your chosen variants. The built-in optional
and result types from Chapter 10 retain their dedicated syntax and handling.

## Generic unions

Raw unions can be parameterized too:

```nerd
Storage :: union [T] {
    value T
    pointer ^T
}

main :: fn () -> i32 {
    storage: Storage[i32] = { value: 42 }
    return storage.value
}
```

This returns `42`. `Storage[i32]` overlays an `i32` and a pointer to an
`i32`, following the raw union rules from Chapter 5. There is no automatic
tag identifying the active field. Type parameters do not make reading an
inactive union field safe.

## What generics change

Generics describe types chosen at compile time. Nerd currently accepts type
parameters, rather than arbitrary value parameters: `[T]` names a type, not
a runtime count. Square brackets in a call select types, while square brackets
on an array or slice value index its elements.

A generic body must be valid for the concrete types used with it. Passing
values through a parameter, returning them, or putting them in a plex can
work for many types. An operation such as addition still needs operands that
support that operation; using a type parameter does not make every operation
available on every type.

Generics also preserve ordinary ownership and lifetime rules. A value parameter
does not become a reference because its type is generic, and a noncopyable
arena remains noncopyable. Use pointer parameters when the function needs
access to an existing arena or other caller-owned storage.

The next chapter introduces methods and ways to state shared behavior
requirements for generic code. This chapter's examples use only concrete
instantiations and the operations already covered.
