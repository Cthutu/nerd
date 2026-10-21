# Chapter 13: Methods and Traits

[Previous: Generics](12-Generics.md)

[Next: String Interpolation](14-String-Interpolation.md)

Earlier chapters used calls such as `values.push(42)` and `storage.reset()`.
These are methods: functions called through a value. This chapter explains
how to define methods for your own types, and how traits describe behavior
that different types can provide.

## Methods with `impl`

An `impl Type { ... }` block attaches functions to a type. These are called
**inherent methods**, because they belong directly to the type:

```nerd
Counter :: plex { value i32 }

impl Counter {
    add :: fn (self: ^Self, amount: i32) {
        self.value += amount
    }

    get :: fn (self: Self) => self.value
}

main :: fn () -> i32 {
    counter: Counter
    counter.add(40)
    pointer := ^counter
    pointer.add(2)
    return counter.get()
}
```

This returns `42`. Inside `impl Counter`, `Self` means `Counter`. The first
parameter is the **receiver**: `counter.add(40)` passes `counter` as that
argument, followed by `40` as `amount`.

The receiver's name is an ordinary parameter binding. `self` is a convention,
rather than a required keyword. Its type determines how the value is passed:

| Receiver | Meaning |
| --- | --- |
| `self: Self` | Receive a value, as with an ordinary value parameter. |
| `self: ^Self` | Receive a pointer, allowing mutation of the original value. |

The compiler takes the receiver's address when a pointer receiver needs it,
so `counter.add(40)` does not need an explicit `^counter`. A pointer can also
be the receiver, as `pointer.add(2)` demonstrates. As with other pointer use,
the referenced value must remain alive.

A value receiver does not permit assignment to its fields. Use a pointer
receiver to change the original value. Calling a value-receiver method also
does not make a noncopyable type, such as an arena, copyable.

Both block and expression function syntax work for methods. In the example,
`add` uses a block for mutation, while `get` directly returns an expression.
Implementation blocks can extend primitive types too: `impl i32` can define
methods whose `Self` is `i32`, called through an integer value of that type.

## Associated functions

An implementation can also contain a function without a receiver. Call it
through the type:

```nerd
Counter :: plex { value i32 }

impl Counter {
    init :: fn (start: i32) -> Self {
        return { value: start }
    }
}

main :: fn () -> i32 {
    counter := Counter.init(42)
    return counter.value
}
```

This returns `42`. `Counter.init(42)` constructs a `Counter`; no existing
counter is passed to it. Such functions are called **associated functions**.
An associated function's declared return type must contain `Self`, for example
`Self`, `^Self`, or `?Self`, making these functions useful for constructors and
factories. Write that return type explicitly rather than relying on expression
function inference. `init` is an ordinary binding name, not a special constructor
keyword.

## Methods on generic types

Chapter 12 introduced generic data types. Declare their implementation's type
parameters explicitly after `impl`, then apply them to the target type:

```nerd
Cell :: plex [T] { value T }

impl [T] Cell[T] {
    init :: fn (value: T) -> Self {
        return { value: value }
    }

    set :: fn (self: ^Self, value: T) {
        self.value = value
    }

    get :: fn (self: Self) => self.value
}

IntCell :: Cell[i32]

main :: fn () -> i32 {
    cell := IntCell.init(40)
    cell.set(42)
    return cell.get()
}
```

This returns `42`. `[T]` declares the implementation's parameter;
`Cell[T]` applies it to the target type. For `Cell[i32]`, `Self` means
`Cell[i32]`, and the methods use `i32` wherever they refer to `T`.
The parameter is available in member signatures and bodies.

The example calls `init` through the concrete alias `IntCell`. You can also
write `Cell[i32].init(40)` directly; both select the same concrete type.

A method can also declare its own type parameters, independently of whether
its receiver type is generic:

```nerd
Selector :: plex { use_first bool }

impl Selector {
    choose :: fn [T] (self: Self, first: T, second: T) =>
        on self.use_first => first else second
}

main :: fn () -> i32 {
    selector := Selector { use_first: yes }
    prn(selector.choose("chosen", "other"))
    return selector.choose[i32](42, 0)
}
```

This prints `chosen` and returns `42`. Type arguments can be inferred from
the method's arguments or supplied after its name. The `storage.alloc[i32]()`
calls from Chapter 8 use this explicit method-call syntax too.

## Declaring and implementing a trait

A trait names a set of required functions. A type implements the trait by
providing those functions with compatible signatures:

```nerd
Named :: trait {
    name :: fn (self: Self) -> string
}

Status :: enum { Ready Waiting }

impl Named for Status {
    name :: fn (self: Self) => on self {
        Ready => "ready"
        Waiting => "waiting"
    }
}

main :: fn () {
    status := Status.Ready
    prn(status.name())
    prn(Named.name(status))
}
```

This prints `ready` twice. The trait declaration gives `name` a function
type, with no body. `impl Named for Status` supplies its implementation.
Here, `Self` in the requirement stands for whichever type implements `Named`;
in this implementation, it means `Status`.

Every parameter in a requirement needs a name. Implementation parameter names
may differ from those in the requirement, but their types and the return type
must match after substituting `Self`. Use an underscore-prefixed parameter
name when an implementation intentionally does not read it, as in Chapter 4.

Provide all required members for a trait/type pair in one implementation
block. Separate blocks do not merge into a complete implementation. Missing
members, incompatible signatures, and duplicate implementations are errors.
An inherent method with the same name does not replace a required trait
implementation.

A trait can give its self type another name with `trait for Value`. This is
useful when a domain-specific name reads more clearly than `Self`:

```nerd
Resettable :: trait for Value {
    reset :: fn (value: ^Value)
}

Counter :: plex { value i32 }

impl Resettable for Counter {
    reset :: fn (self: ^Self) {
        self.value = 0
    }
}

main :: fn () -> i32 {
    counter := Counter { value: 42 }
    counter.reset()
    return counter.value
}
```

This returns `0`. `Value` names the implementing type in the trait declaration;
`Self` is still available inside the implementation.

## Selecting a trait method explicitly

Receiver syntax makes both inherent and trait methods convenient to call.
An inherent method takes precedence if it has the same name as a trait method.
Use `Trait.member(value, ...)` to choose the trait implementation explicitly:

```nerd
Named :: trait {
    name :: fn (self: Self) -> string
}

Item :: plex { label string }

impl Item {
    name :: fn (self: Self) => self.label
}

impl Named for Item {
    name :: fn (_self: Self) => "trait name"
}

main :: fn () {
    item := Item { label: "inherent name" }
    prn(item.name())
    prn(Named.name(item))
}
```

This prints `inherent name` followed by `trait name`. If two traits provide
the same receiver method and neither is selected by an inherent method, the
receiver call is ambiguous. Name the trait to resolve that ambiguity.

Trait members are not ordinary top-level functions: write `Named.name(item)`
or `item.name()`, rather than a bare `name(item)`.

## The built-in traits

The standard-library `core` module declares these five traits in `core.n`.
Nerd makes its public declarations available automatically, so you do not
need an import to use them. [Chapter 15](15-Modules.md#the-core-module)
explains the module in more detail:

| Trait | Required function | Language behavior |
| --- | --- | --- |
| `Display` | `show(self: Self) -> string` | Text representation, including errors returned from `main`. |
| `Eq` | `eq(self: Self, other: Self) -> bool` | Equality with `==` and `!=`. |
| `Order` | `compare(self: Self, other: Self) -> i32` | Ordering with `<`, `<=`, `>`, and `>=`. |
| `Default` | `default() -> Self` | Initialization of a typed local variable without an initializer. |
| `Iterator[Item]` | `next(self: ^Self) -> ?Item` | Producing successive items for iteration. |

These are the existing core traits, so do not redeclare them to customize
your own type. Add an implementation of the existing trait instead. A local
trait with the same name would be a different declaration.

### `Display`: converting values to text

Implement `Display` to give a value a textual representation:

```nerd
Status :: enum { Ready Waiting }

impl Display for Status {
    show :: fn (self: Self) => on self {
        Ready => "ready"
        Waiting => "waiting"
    }
}

main :: fn () {
    status := Status.Waiting
    prn(status.show())
    prn(Display.show(status))
}
```

This prints `waiting` twice. `show` returns a string; it does not print that
string itself. `prn` performs the printing here. Strings and numbers already
have built-in display support.

Chapter 10 required `Display` for the error type of a result-returning `main`.
Here is a complete implementation for a custom error:

```nerd
Failure :: enum { NotReady }

impl Display for Failure {
    show :: fn (self: Self) => on self {
        NotReady => "the system is not ready"
    }
}

main :: fn () -> void\Failure {
    return Failure.NotReady!
}
```

This writes `Error: the system is not ready` followed by a newline to standard
error and exits with status `1`. The runtime uses `Display.show` to obtain the
message after cleanup has run.

String interpolation also uses `Display` for custom values. Chapter 14 covers
its syntax and temporary storage; direct `show()` calls need
neither interpolation syntax nor an implicit printing operation. If a `show`
implementation allocates its returned string in an arena, that string follows
the arena's lifetime, as in Chapter 8. The examples above return string literals.

### `Eq`: defining equality

Primitive values already have equality behavior. Plexes and enums also
support structural equality when all their fields or payloads support equality:
matching values compare field by field, with enum variants also taken into
account. An explicit `Eq` implementation takes precedence over that behavior:

```nerd
User :: plex { id i32 label string }

impl Eq for User {
    eq :: fn (self: Self, other: Self) => self.id == other.id
}

main :: fn () -> i32 {
    first := User { id: 7, label: "first label" }
    second := User { id: 7, label: "second label" }
    return on first == second && !(first != second) => 42 else 0
}
```

This returns `42`. The implementation defines equality by `id`, so the
different labels do not make the users unequal. `==` calls the implementation,
and `!=` negates its result. Write the comparison using fields that already
support equality; comparing `self == other` inside `eq` would call it again.

Arrays, slices, and dynamic arrays compare their contents when element
equality is supported. Raw pointers compare addresses. Arenas, raw unions,
and function values do not have ordinary value equality.

### `Order`: defining ordering

`Order.compare` returns a negative integer when the receiver is less than
the other value, zero when they have equal ordering, and a positive integer
when the receiver is greater:

```nerd
Priority :: plex { rank i32 }

impl Order for Priority {
    compare :: fn (self: Self, other: Self) => on {
        self.rank < other.rank => -1
        self.rank > other.rank => 1
        else => 0
    }
}

main :: fn () -> i32 {
    low := Priority { rank: 2 }
    same := Priority { rank: 2 }
    high := Priority { rank: 9 }
    return on low < high && low <= same && high > low && same >= low => 42 else 0
}
```

This returns `42`. Nerd compares the returned integer with zero for each
ordering operator. The result need not be exactly `-1` or `1`; only its sign
matters. Branching on the fields avoids overflow from subtracting arbitrary
integer values.

`Order` does not define `==` or `!=`. Implement `Eq` as well when your type
needs custom equality, and keep equality and ordering consistent.

### `Default`: choosing an initial value

A typed local variable without an initializer uses its type's `Default`
implementation when one exists:

```nerd
Settings :: plex { retries i32 }

impl Default for Settings {
    default :: fn () -> Self {
        return { retries: 42 }
    }
}

main :: fn () -> i32 {
    settings: Settings
    explicit := Default[Settings].default()
    return on settings.retries == explicit.retries => settings.retries else 0
}
```

This returns `42`. `settings: Settings` calls the implementation instead of
using zero initialization. `Default[Settings].default()` explicitly selects
the implementation for `Settings`: the function has no receiver argument
from which to infer that type. The brackets select a type, rather than index
a value.

An explicit initializer takes precedence. The `undefined` initializer from
Chapter 4 skips initialization, including a call to `Default`. Without a
matching `Default` implementation, a typed local uses the zero initialization
rules already described there. This trait does not automatically turn every
zero-initialized field or plex literal into a call to `default()`.

Implicit initialization calls `default()` once for the variable, just as an
explicit call such as `settings := Default[Settings].default()` does.

## Trait requirements in reusable functions

A generic function from Chapter 12 can require its type arguments to implement
a particular trait. Add a `where` constraint:

```nerd
Status :: enum { Ready Waiting }

impl Display for Status {
    show :: fn (self: Self) => on self {
        Ready => "ready"
        Waiting => "waiting"
    }
}

Label :: plex { text string }

impl Display for Label {
    show :: fn (self: Self) => self.text
}

describe :: fn [T] (value: T) -> string
where T: Display {
    return value.show()
}

main :: fn () {
    prn(describe(Status.Ready))
    prn(describe(Label { text: "label" }))
    prn(describe(42))
}
```

This prints `ready`, `label`, and `42` on separate lines. As before, `[T]`
declares the function's type parameter. The new `where T: Display` requires the chosen
type to support `Display`, so the body can call `value.show()`.
The compiler checks the requirement for each concrete call. It can infer `T`
from the argument, or you can supply it explicitly, as in
`describe[Status](Status.Ready)`.

A matching trait constraint is required inside the generic body; a particular
caller passing a displayable type does not replace that declaration. Generic
implementation blocks can use `where` constraints in the same way when their
methods need trait operations on a type parameter.

Built-in formatting types also satisfy the core `Display` trait. For example,
`describe(42)` returns `"42"`. Booleans, integer types, `f32`, `f64`, and strings
support this, as do tuples, arrays, slices, and dynamic arrays containing
built-in displayable values. Their `.show()` uses the same representation as
string interpolation, covered in Chapter 14. Aggregates containing custom
types do not automatically gain `Display` through this rule.

### `Iterator[Item]`: producing optional items

The final core trait takes an item type. `Iterator[i32]` means an iterator
that produces `i32` values; the brackets supply the trait's type argument.
Its `next` method takes a pointer so it can update the iteration state,
and returns `?i32`:

```nerd
Countdown :: plex { remaining i32 }

impl Iterator[i32] for Countdown {
    next :: fn (self: ^Self) -> ?i32 {
        on self.remaining == 0 => return nil
        item := self.remaining
        self.remaining -= 1
        return item
    }
}

main :: fn () -> i32 {
    countdown := Countdown { remaining: 2 }
    total := 0
    for item in countdown {
        total += item
    }
    return total
}
```

This returns `3`. The loop calls `next` to obtain present values `2` and `1`,
then stops on absence. A present zero would still be an item: the optional's
presence, rather than the payload's truthiness, determines whether an item
was produced.
This implementation keeps returning `nil` once `remaining` reaches zero;
start it with a nonnegative count.

The loop syntax from Chapter 11 uses the trait to request successive items.
Unlike built-in collection iteration, a user-defined iterator binds the
returned item directly: here `item` is an `i32`, so it needs no dereference.
You can also call `countdown.next()` directly and handle the optional result
using the forms from Chapter 10.

## Choosing a method or a trait

Use an inherent method for behavior specific to one type. Use a trait when
different types should provide the same named operations, when a generic
function needs to require those operations, or when integrating with language
features such as equality, ordering, default initialization, and error display.

Methods remain functions: their parameters, return values, unused bindings,
and resource lifetimes follow the rules from earlier chapters. A trait
implementation supplies behavior; it does not change who owns the values
passed to those functions.
