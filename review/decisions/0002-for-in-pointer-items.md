# 0002: For-In Pointer Items

Status: accepted
Date: 2026-05-09

## Context

`for item in collection` previously copied each element, while
`for ^item in collection` bound a pointer. That split added syntax and compiler
state for a choice that usually needs to be visible in the item type anyway.

## Decision

`for item in collection` always binds `item` as `^Element`.

The `for ^item in collection` spelling is removed. Indexed iteration keeps the
same shape:

```text
for index, item in collection
```

where `index` is `usize` and `item` is `^Element`.

Member access keeps the existing auto-deref behavior, so `item.field` is the
normal way to access a field through the loop item pointer. Explicit dereference
is still required when using the element value itself or assigning the whole
element:

```text
value := item^
item^ = replacement
item.field = new_value
```

## Consequences

- `for in` has one item binding mode instead of value and pointer variants.
- The parser, AST, CST, formatter, sema, IR, and HIR no longer need an
  `item_is_pointer` loop flag.
- Read-only scalar iteration is more explicit because scalar values require
  `item^`.
- Mutable iteration is the default and does not need special loop syntax.

## Follow-up

- Keep LSP hover/completion tests around `for in` items, because every item
  should now complete and hover as a pointer to the element type.
