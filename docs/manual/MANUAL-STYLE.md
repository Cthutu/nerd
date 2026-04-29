# Manual Style Guide

This document records the rules for writing and revising the learner-facing
Nerd manual. Use it when adding new manual sections or reviewing existing parts.

## Goal

The manual should teach the language from first principles. A reader should be
able to move through the parts in order without needing to already know Nerd
terminology.

The manual is not a compiler implementation guide and is not the standard
library reference.

## Teaching Order

- Introduce a concept before relying on its name.
- When a term must appear before its full explanation, give a short working
  definition and say where the full explanation appears later.
- Prefer one new concept at a time. If an example needs two new concepts, either
  simplify the example or add a short note for the secondary concept.
- Do not use syntax in an example before explaining what the reader is meant to
  notice in that syntax.
- Keep early examples small, even if that means they are less idiomatic than
  later examples.

Good:

```md
Write `name()` to run a function named `name`. Functions with inputs are
covered in Part 4.
```

Avoid:

```md
Call `name()` here.
```

when `call` has not been explained yet.

## Terms

- Use plain language first, then introduce the technical term.
- Define source-level terms from the reader's point of view.
- Avoid compiler-internal terms unless they affect how source code behaves.
- If a word has a precise Nerd meaning, do not use it loosely.

Examples:

- A function is a piece of code that can be run.
- A binding gives a name to a value, function, type, module, or other
  declaration.
- A module is a file of code that exports names for other files to use.

## Forward References

Forward references are acceptable when they are explicit and brief.

Use this shape:

```md
Pointers are introduced in Part 8; here `^value` means the loop item is a
pointer to an element rather than a copy.
```

Do not stop the current section to fully teach a later topic. Give only enough
context for the current example to make sense.

## Examples

Examples should be either complete runnable files or clearly partial snippets.

Complete examples should include the imports they need:

```nerd
use std.io

main :: fn () {
    prn("hello")
}
```

Snippet examples are fine for local syntax, but the surrounding text should make
clear that they are not complete programs.

Avoid examples that introduce future concepts accidentally. If a section is
about bindings, do not also require the reader to understand slices, pointers,
modules, and FFI unless those are explicitly introduced or deferred.

## Tables

Use tables when a concept is a set of parallel forms:

- syntax forms and meanings
- operators and meanings
- literal forms and resulting types
- binding forms and when to use them

Use prose when explaining behaviour, ordering, ownership, or control flow.

Tables should not replace teaching text. Introduce the idea first, then use the
table as a compact reference.

## Standard Library

The manual may use standard library functions for simple examples, especially
`std.io.pr` and `std.io.prn`.

When first used, explain only what the example needs:

- `std.io` is a standard library module.
- `use std.io` imports its public names.
- `pr` prints without a newline.
- `prn` prints with a newline.

Do not document the full standard library API in the language manual. Use
`docs/stdlib.md` for that.

## Review Comments

During review, comments may be added as paragraphs beginning with `//`.

When addressing a review comment:

1. Read the nearby section and the surrounding teaching order.
2. Revise the manual text so the issue is actually handled.
3. Remove the original `//` comment.
4. Search the edited part for remaining `//` comments before finishing.

Do not leave review comments in user-facing manual parts.

## Part Checklist

Before considering a part ready:

- The first occurrence of each important term is explained.
- Examples do not rely on unexplained syntax without a forward note.
- Any deferred topic points to the later part that teaches it.
- Tables are used for compact sets of forms where they improve scanning.
- Standard library usage is kept incidental and clearly identified.
- No test files are referenced from user-facing manual parts.
- No review comments beginning with `//` remain.
