# Generics Simplification Discussion

This document collects the current discussion about removing general generics
from Nerd while preserving expressive data modelling, clear error handling,
and useful reusable modules. It is a design exploration, not yet a committed
implementation plan or language specification. Once the design settles, turn
the agreed work into roadmap milestones and update the manual and specs as each
feature lands.

The parametrised-module redirection is currently on hold. Nerd retains general
generic functions, types, traits, implementations, and constraints. The active
direction is limited to the integrated `?T` optional and `T\E` result features;
the broader material below remains design context rather than an active removal
plan. `ROADMAP.md` is authoritative for implementation sequencing.

## Goals

- Simplify the language and compiler substantially.
- Prefer explicit, concrete code after module resolution.
- Keep strong data support, pattern matching, ownership, and error handling.
- Keep Nerd a simple low-level C replacement rather than growing it into a
  type-system-focused language.
- Avoid hidden ranking, duck typing, and inference-heavy abstraction.
- Retain useful reuse at an explicit module boundary.
- Keep HIR and backend input concrete rather than carrying generic entities.

## Proposed Generic Boundary

The emerging direction is to remove user-defined general generics from:

- functions and methods
- plexes, enums, unions, and aliases
- traits and trait implementations
- local inference and compound-function candidate resolution

This does not mean every type constructor stops accepting types. Built-in
parametric type constructors remain part of the type system rather than a
user-programming facility:

```nerd
^T       -- non-owning, non-null thin pointer
[]T      -- non-owning slice
[..]T    -- owning dynamic array
[N]T     -- fixed array
box[T]   -- owning heap pointer
atomic[T]
?T       -- optional T
T\E      -- T or error E
```

`box[T]` is analogous to `^T`: it is a built-in pointer-like type constructor,
not evidence that Nerd needs general generic types.

Removing generics is not yet decided. Before removal, audit all current uses
and prove that the proposed replacements cover real programs without creating
more complexity than they remove.

## Parametrised Modules

### Declaration

A parametrised module declares its parameters in a header:

```nerd
module [T]

pub Stack :: plex {
    data [..]T
}

pub impl Stack {
    push :: fn (self: ^Self, value: T) {
        self.data.push(value)
    }
}
```

Rules under consideration:

- `module` must be the first keyword when present. Comments may precede it.
- Ordinary non-parametrised modules need no `module` declaration.
- Initially, module parameters are types only.
- Later, compile-time value parameters may use the proposed `::` notation.
- A folder module declares parameters once in `mod.n`; sibling module parts
  share them and do not repeat the header.
- Explicit child modules do not inherit parent parameters implicitly.

### Instantiation And Module Bindings

Nerd already binds qualified modules with `name :: use path`. Parametrised
modules extend that existing form:

```nerd
integer_stack :: use std.stack[i32]
string_stack  :: use std.stack[string]

numbers : integer_stack.Stack
names   : string_stack.Stack
```

Do not introduce an `as` form for this purpose.

The intended compiler model is:

1. Resolve the module path and arguments.
2. Construct a canonical module-instance identity.
3. Substitute the parameters throughout the instance.
4. Analyse the instance as ordinary non-generic code.
5. Generate ordinary concrete HIR and LLVM.

Once resolved for a set of types, a module instance acts like a normal
non-generic module. No generic entity should survive into HIR.

Two local bindings of the same path and canonical arguments should name the
same module instance and types:

```nerd
a :: use std.stack[i32]
b :: use std.stack[i32]

-- a.Stack and b.Stack are the same type.
```

Different arguments produce different instances and type identities.

### Benefits

- Reuse is explicit at a large, visible boundary.
- Calls inside an instance are ordinary concrete calls.
- Methods on exported instance types are concrete methods.
- Generic call inference and generic compound-function candidates disappear.
- Related data and algorithms remain grouped together.
- LSP can present a concrete substituted module surface.

### Costs And Questions

Parametrised modules still require:

- canonical argument and instance identity
- instance caching and symbol naming
- dependency and recursive-instantiation handling
- substitution across all module parts
- diagnostics referring to template source and the instantiation site
- LSP navigation between concrete instances and template source
- rules for platform-gated and nested module imports

Initially require every argument explicitly. Do not infer module parameters.

## Optional Types

### Type And Representation

Use a prefix optional type constructor:

```nerd
?T
```

Prefix notation composes without the ambiguity of postfix `T?`:

```nerd
?^T      -- optional pointer to T
^?T      -- non-null pointer to optional T storage
?[]T     -- optional slice
[]?T     -- slice of optional elements
?[4]T    -- optional fixed array
[4]?T    -- fixed array of optional elements
```

Type constructors associate to the right. Parentheses may make grouping
explicit.

Change `^T` to mean a non-null thin pointer. `?^T` is its nullable form and has
a guaranteed one-word niche representation:

```text
null pointer     = absent
non-null pointer = present ^T
```

There is no present-but-null state. This is a language layout guarantee for
optional thin pointers, not an optional backend optimisation. Do not initially
generalise niche guarantees to boxes, integers, or arbitrary enums.

The non-null pointer change requires an audit of default initialisation, FFI,
casts, platform bindings, fields, definite assignment, and existing nil use.

### Construction

Do not require `Some` or `None` constructors. Context constructs the optional:

```nerd
find_user :: fn (id: UserId) -> ?User {
    on missing => {
        return nil
    }

    return user
}
```

For a destination of type `?T`:

- `nil` constructs absence.
- A value compatible with `T` constructs presence.
- An existing `?T` copies or moves according to `T`.

## Result Types

Use `\` as the dedicated result-type separator:

```nerd
T\E
```

Backslash is reserved for this role in type syntax. It is not an expression
operator and has no other language meaning. This is the selected spelling, not
a placeholder for `!` or `|`.

```nerd
load_image :: fn (bytes: []u8) -> Image\ImageError
```

The result remains a tagged success/error value even when `T` and `E` are the
same type. It is a dedicated language result construct, not a general type
union. Nerd does not intend to add arbitrary union types such as `A | B`.
Explicit C-style `union` declarations remain a separate low-level storage
feature.

Postfix `!` remains exclusively an expression-level error conversion:

```nerd
load_image :: fn () -> Image\ImageError {
    return make_error()!
}
```

Reasons for this spelling:

- It does not visually suggest an independent `!E` type.
- Type construction and expression injection use separate symbols.
- It is distinctive enough that `T\E` reads as a type form rather than an
  ordinary expression whose operands happen to be types.
- Unlike `|`, it does not reuse punctuation already associated with boolean,
  bitwise, pattern, or potential alternation roles.
- Its dedicated role distinguishes result types from ordinary operators and
  C-style union declarations.
- Compact signatures remain readable without the visually dense `T !E` form.

Trade-offs to document and test:

- Backslash commonly means escaping, Windows paths, or mathematical set
  difference rather than error or alternation.
- Nested optional/result types may be visually noisy.
- Documentation, generated text, and shell contexts may require extra escaping.

Describe `\` as a result or error-channel separator rather than a general
operator. The formatter preserves the compact `T\E` spelling.

### Construction

Do not require `Ok` or `Err` constructors:

```nerd
parse :: fn (text: string) -> i32\ParseError {
    on invalid => {
        return InvalidDigit!
    }

    return value
}
```

- A value compatible with `T` contextually constructs success.
- Postfix `!` contextually converts a value compatible with `E` into the error
  channel of an expected result type.
- A bare `E` is not implicitly an error.
- Prefix `!` remains boolean negation; postfix `!` is error construction.
- Applying postfix `!` to a value that is already a result is invalid.

For example:

```nerd
make_error :: fn () -> ImageError {
    return InvalidData
}

load_image :: fn () -> Image\ImageError {
    return make_error()!
}
```

The return type supplies the success type. Without an expected result type, the
compiler cannot infer it:

```nerd
failure := make_error()!                    -- invalid: no success type
failure : Image\ImageError = make_error()!  -- valid
```

## Early Propagation

Use postfix `?` to unwrap success/presence or return early:

```nerd
find_manager :: fn (employee: Employee) -> ?Manager {
    department := find_department(employee)?
    return find_manager_for_department(department)
}
```

For `?T`, absence returns `nil` from the enclosing optional-returning function.
For `T\E`, failure returns the error from the enclosing result-returning
function and the expression produces `T` on success:

```nerd
load_asset :: fn (bytes: []u8) -> Asset\AssetError {
    image := decode_image(bytes)?
    return Asset { image: image }
}
```

Initially require the propagated error type to match or use an existing
unambiguous implicit conversion. Do not require generic conversion traits.

Diagnostics must cover invalid operands, incompatible enclosing return types,
incompatible errors, and propagation outside a valid early-return context.

## `on` Extraction And Pattern Matching

### Boolean Form

Nerd's short boolean form uses `=>`:

```nerd
on condition => {
} else {
}
```

Extend it with branch-local payload binders.

Optional:

```nerd
on optional => [value] {
    use(value)
} else {
    handle_missing()
}
```

Result:

```nerd
on result => [value] {
    use(value)
} else [error] {
    report(error)
}
```

The brackets introduce new branch-scoped bindings; they do not assign hidden
side effects into existing locals. The success/present binder exists only in
the first branch. The result error binder exists only in the `else` branch.

### Full Pattern Form

Removing named `Some`, `None`, `Ok`, and `Err` constructors also removes their
obvious pattern syntax. Use `on ... else ...` to separate the tagged payload
domains.

Optional payload patterns appear in the first block; `else` handles absence:

```nerd
on optional {
    { name: "root" } => use_root()
    user             => use(user)
} else {
    handle_missing()
}
```

Result success patterns appear in the first block and error patterns in the
`else` block:

```nerd
on result {
    image => display_image(image)
} else {
    InvalidData { message } => report(message)
    UnexpectedEnd           => report_truncated()
}
```

Here `else` is the preferred syntax for optional absence. This avoids `Some`,
`None`, `Ok`, and `Err`, avoids consuming square-bracket array-pattern syntax
for full matching, and makes success versus error structurally visible. It
needs careful grammar and exhaustiveness rules, especially when either block
contains multiple patterns or an expression-valued `on`.

The boolean binder form and full pattern form remain distinct because `=>`
appears immediately after the scrutinee only in the boolean form.

## Arena Type Operands

Replace the small current use of generic arena methods with compiler-known type
operands:

```nerd
value  := scratch.alloc(i32)
values := scratch.alloc_array(i32, 128)
```

The type argument is not a runtime value. These built-ins resolve it in type
context, compute size and alignment, and return `^T` or `[]T`.

LSP support is required:

- type completion in the operand position
- substituted return type in hover and signature help
- go-to-definition and rename for the referenced type
- type semantic highlighting
- targeted `expected a type` diagnostics
- useful inferred-type hints where appropriate

## Display And Interpolation

Keep `Display` as an explicit nominal trait. Do not use a language-known method
name or duck typing.

The proposed allocation-aware shape is:

```nerd
Display :: trait {
    display :: fn (self: Self, output: ^arena) -> string
}
```

A concrete type opts in explicitly:

```nerd
impl Display for Point {
    display :: fn (self: Self, output: ^arena) -> string {
        -- Return text allocated from output.
    }
}
```

String interpolation resolves the nominal `Display` implementation and passes
its active temporary arena. Non-generic traits using `Self` can remain even if
generic traits and implementations are removed.

Pointers and boxes display pointer information such as their address. They do
not implicitly display their pointee. Dereference explicitly to display the
contents:

```nerd
$"pointer={pointer} contents={pointer^}"
```

This avoids compiler-provided conditional `Display` forwarding for `box[T]`
and keeps pointer identity distinct from pointee presentation.

### Confirmed Current Runtime Behaviour

`data/nrt.c` currently uses two thread-local facilities for interpolation:

- a reusable growable scratch string-builder buffer
- `g_temp_arena`, which owns completed runtime interpolation strings

`string_builder_finish()` copies the completed bytes into the temporary arena
and restores the scratch builder cursor to the mark for that interpolation.
Nested interpolation is therefore supported. The scratch buffer grows through
reallocation when required and is reused afterward.

The result arena is not reset automatically after each interpolation.
`temp_arena.reset()` explicitly invalidates accumulated temporary strings, and
programs with frame or request loops should call it at a safe lifetime
boundary. Exposing the active arena to `Display.display` lets implementations
use the same allocation lifetime as the interpolation that requested them.

## Iteration Without Generic Traits

The current `Iterator[Item]` is a generic trait and needs replacement if generic
traits are removed. A possible direction is a language-checked structural
iteration contract based on a concrete `next` method returning `?T`:

```nerd
impl Counter {
    next :: fn (self: ^Self) -> ?i32 {
        -- Return an item or nil.
    }
}
```

Unlike `Display`, iteration is already syntax-directed by `for in`, so a
specific compiler-validated protocol may be acceptable. This needs a separate
discussion; avoid accidentally recreating broad duck typing.

## Traits After General Generics

Non-generic nominal traits such as `Display`, `Eq`, `Order`, and `Default` can
remain. The likely removals are:

- generic trait declarations
- generic trait implementations
- generic `where` constraints
- conditional implementations such as `Display for Box[T] where T: Display`

Audit each language-known trait and its existing implementations before making
this decision. Prefer explicit concrete conformance and narrowly defined
compiler behaviour over hidden structural lookup.

## Expressiveness Trade-Off

The language would lose general reusable algorithms such as `swap[T]`,
`max[T]`, and generic higher-order collection operations. Replacements include:

- built-in operations where the concept is fundamental
- concrete ordinary functions
- explicit compound functions for a closed set of signatures
- parametrised modules for reusable data structures and related algorithms

This direction deliberately favours concrete systems programs over pervasive
parametric abstraction. It also explicitly rejects arbitrary type unions and
other type-system expansion that does not serve Nerd's role as a low-level C
replacement. The final generics decision should follow experiments rather than
aesthetics alone.

## High-Level Implementation Milestones

Treat the whole design as a staged replacement programme rather than removing
generics first and repairing the language afterward. Each language milestone
must cover parser and CST, formatter, sema, HIR and LLVM, diagnostics, LSP,
tests, the learner-facing manual, and the normative specs where applicable.

### 1. Audit And Normative Design

- Inventory every generic declaration, instantiation, constraint,
  implementation, and standard-library dependency.
- Classify each use as a built-in type constructor, optional/result use,
  reusable data structure, generic algorithm, iterator, arena operation, or
  trait implementation.
- Map every current use to a proposed replacement or record it as an
  expressiveness gap.
- Finalise grammar, precedence, construction, movement, cleanup, layout, FFI,
  pattern, and control-flow rules for the new facilities.
- Define measurable go/no-go criteria for eventual generic removal.

No generic facility is removed in this milestone.

### 2. Optional Types And Non-Null Pointers

- Add `?T`, contextual presence construction, and `nil` absence.
- Change `^T` to a non-null thin pointer and use `?^T` for nullable pointers.
- Guarantee the one-word null-niche representation of `?^T`.
- Define definite assignment, casts, equality, movement, cleanup, and FFI
  behaviour.
- Migrate existing nullable pointers across the compiler, runtime declarations,
  OS modules, standard library, examples, and tests.

Keep the existing generic `Option[T]` temporarily while the replacement is
validated.

### 3. Result Types And Integrated Control Flow

- Add the dedicated `T\E` result type.
- Add contextual success construction and contextual postfix `error!`
  injection, with no standalone error-only type.
- Add postfix `?` propagation for both optionals and results.
- Add boolean extraction with `on value => [success] ... else [error] ...`.
- Add full present/success and error payload pattern tables with
  `on value { ... } else { ... }`.
- Define branch-local bindings, guards, comma patterns, expression-valued
  forms, independent exhaustiveness, cleanup, and error compatibility.
- Migrate representative `Option[T]` and `Result[T, E]` code while retaining
  the old forms temporarily for comparison.

This is the first major ergonomics checkpoint: integrated types must prove
clearer in real decoder, I/O, frame, and command code before old result types
are removed.

### 4. Type Operands And Built-In Generic Replacements

- Replace `arena.alloc[T]()` and `arena.alloc_array[T](count)` with
  compiler-known type operands such as `arena.alloc(T)` and
  `arena.alloc_array(T, count)`.
- Add complete type-context completion, hover, signature help, navigation,
  rename, semantic highlighting, and diagnostics.
- Audit other tiny generic functions and replace only fundamental operations
  with similarly narrow built-ins.
- Do not introduce general runtime type values as a side effect.

### 5. Parametrised Module Prototype

- Add first-position `module [T, ...]` headers and explicit bindings such as
  `integer_stack :: use std.stack[i32]`.
- Initially accept explicit type parameters only.
- Build canonical module-instance identity, caching, substitution, diagnostics,
  concrete methods, source navigation, and concrete symbol naming.
- Substitute parameters before ordinary semantic analysis and emit no generic
  HIR.
- Prove two distinct instances, repeated aliases of one instance, and one
  small real data structure such as `Stack` or `Rect`.

Stop here for a complexity review. Continue only if module instances are
materially simpler than the general generic machinery they are intended to
replace.

### 6. Complete Parametrised Modules

- Support folder module parts, parameter visibility across parts,
  parametrised dependencies, platform-gated imports, public exports, and
  re-exports.
- Diagnose recursive or runaway instantiation with useful template and use-site
  references.
- Allow module parameters to feed explicit instantiation of another module.
- Add compile-time value module parameters later through `::` only after that
  feature has a stable canonical-value model.
- Migrate representative generic compound types and their methods to module
  instances.

### 7. Traits, Display, And Iteration

- Keep non-generic nominal traits and remove no trait facility until its current
  uses have concrete replacements.
- Change `Display` to explicit `display(Self, ^arena) -> string` conformance and
  pass the active interpolation arena.
- Make pointers and boxes display nil/address information; require explicit
  dereference to display pointee contents.
- Audit `Eq`, `Order`, and `Default` for concrete non-generic use.
- Design and prove an explicit `for in` contract that supplies a concrete item
  type without retaining `Iterator[Item]` or introducing broad duck typing.
- Remove conditional generic trait implementations only after equivalent
  deliberate behaviour exists where it is genuinely needed.

Iteration is a required design gate because it is the least settled replacement
in this document.

### 8. Repository Migration

- Convert `Option[T]` to `?T` and `Result[T, E]` to `T\E`.
- Convert nullable pointers to `?^T`.
- Convert generic arena calls to type-operand built-ins.
- Convert reusable generic data structures and concrete methods to
  parametrised modules.
- Convert iteration and generic trait uses to their agreed replacements.
- Rewrite generic manual examples and representative real programs.
- Keep migrations in small reviewable slices with command-path regressions.

Use this milestone to identify any remaining generic algorithm that cannot be
expressed acceptably through modules, compound functions, concrete functions,
or narrowly justified built-ins.

### 9. Remove General Generics

Only after the repository migration is complete and accepted:

- Remove generic functions and methods.
- Remove user-defined generic compound types and aliases.
- Remove generic traits, generic implementations, `where` constraints, and
  conditional conformance.
- Remove generic inference, specialisation, candidate handling, semantic
  tables, HIR paths, symbol naming, diagnostics, LSP paths, syntax, tests, and
  documentation that module instantiation supersedes.
- Continue supporting built-in parametric type constructors and explicit
  parametrised modules.

Do not retain a hidden compatibility implementation indefinitely; once the
migration gate passes, delete obsolete machinery coherently.

### 10. Consolidation And Measurement

- Remove transitional `Option`, `Result`, and old generic compatibility paths.
- Finish the manual, normative specs, language-reference appendices, compiler
  internals, LSP, formatter, diagnostics, and migration documentation.
- Run installed-compiler, editor-integration, FFI, platform, and release smoke
  coverage.
- Measure compiler source and table complexity removed, compile time, memory
  use, module-instance duplication, diagnostic quality, and generated code.
- Record the final language boundary: concrete low-level code, built-in
  parametric storage types, explicit parametrised modules, nominal non-generic
  traits, and no arbitrary type unions.

## Open Questions

- Are module parameters limited to types initially, and exactly which values
  become valid after `::` compile-time parameters exist?
- How are recursive module instances diagnosed and limited?
- Can a parametrised module be re-exported without instantiating it?
- What is the precise grammar and precedence of `T\E` alongside `?T`, postfix
  `?`, and postfix `!`?
- Which implicit conversions may construct optional success or result success?
- How do full optional/result `on ... else ...` forms compose with guards,
  comma patterns, expression results, and exhaustiveness?
- Does `nil` remain valid for `box[T]`, and should optional boxes eventually
  receive a niche layout guarantee?
- How are nullable C pointers expressed and checked at FFI boundaries?
- Does `Display.display` accept `Self` only, or may implementations use `^Self`
  for non-copyable and large values?
- Which concrete iterator protocol preserves nominal, explicit language design
  without retaining a generic trait?
- Which existing uses of generic algorithms cannot be expressed acceptably
  through modules, compounds, or built-ins?
