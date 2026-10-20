# Chapter 1: Overview of the Nerd Programming Language

Nerd is a systems programming language designed as a replacement for C, with the
aim of reducing the friction between an idea and its implementation. It keeps
direct control over memory and data representation, while providing concise
syntax that lets common programming patterns read naturally. The goal is to make
code easier to write, understand, and maintain, with less ceremony between what
a programmer means and what they express.

Nerd balances expressiveness with a small, consistent set of language
constructs. All loops use `for`, and `on` brings conditional branching and
pattern matching together. Features beyond C—including type inference, generics,
enums with payloads, and built-in optional and result types—help express data
and intent directly. `defer` and failure-only `undo` keep clean-up close to the
operations that need it. These features serve a common purpose: capture
recurring patterns with clear, reusable constructs while keeping the language
approachable and its syntax compact.

## Who this guide is for

This guide assumes you can already program in C and are new to Nerd. You should
be comfortable with variables, functions, structs, pointers, and manual memory
management. The chapters focus on Nerd's syntax and behaviour, using comparisons
with C where they help explain a difference. Each new Nerd concept is introduced
before it is used in larger examples.

Your knowledge of C provides a useful foundation: programs still work with
concrete values, call functions, and access memory through pointers. Nerd gives
you additional ways to express those operations and organise the data around
them. Learning it involves understanding both the new syntax and the rules that
come with its types and control flow.

## A small set of constructs

Nerd's design favours constructs that cover several related patterns. `for`
handles collection iteration, condition-controlled loops, infinite loops, and
C-style loops. `on` handles boolean conditions and matching values against
patterns. Both branching and loops can produce values, so a choice or search
can directly supply the value being assigned.

The balance between features and simplicity is central to the language. A
feature earns its place by making recurring code clearer or reducing repeated
work. Keeping syntax compact also means making related operations consistent,
so learning one form helps you understand others.

## Features beyond C

Several features give common C patterns direct language support:

- **Type inference and generics.** Types can be inferred from initialisers and
  usage, and generic functions and types let you reuse logic across concrete
  types. Explicit type annotations remain available when they clarify intent.
- **Richer data types.** Tuples group values, slices carry a pointer and a count,
  and dynamic arrays provide growable storage. Payload enums associate each
  variant with its own data, expressing relationships that would typically
  require a tag and a union in C.
- **Optional values and results.** `?T` represents a value that may be absent,
  and `T\E` represents either a successful value or an error. Matching and
  failure propagation make these outcomes part of ordinary control flow.
  Alongside booleans, these types have truthiness: presence or success is true,
  while absence or failure is false, regardless of the contained value.
  Chapter 10 explains their use in conditions and current compiler limitations.
- **Scoped cleanup.** `defer` runs cleanup when a scope exits. `undo` runs
  cleanup on failure, helping unwind partially completed resource acquisition.
  Cleanup can stay beside the operation that makes it necessary.
- **Arenas and temporary storage.** The built-in `arena` type groups allocations
  that share a lifetime, so their storage can be reclaimed together. The
  built-in temporary arena provides readily available scratch storage and backs
  runtime string interpolation.
- **Modules and string interpolation.** Modules expose declarations through
  `use` imports. Interpolated strings let you embed values directly in text.

These features will be introduced through working examples, with their syntax,
behaviour, and resource costs explained where they become relevant.

## Memory and interoperability

Nerd preserves direct access to memory through raw pointers and provides
control over data layout, including C-compatible record layouts. Foreign
function declarations let Nerd programs call C libraries, making existing
libraries available alongside Nerd code.

Memory ownership depends on the type. Slices borrow existing storage, dynamic
arrays own growable storage, and boxes transfer ownership and release locally
owned allocations on scope exit. Raw pointers and slices require the same
care with lifetimes that you bring to C: a borrowed view must not outlive its
storage. Scoped cleanup helps manage resources, while the programmer remains
responsible for knowing who owns them and how long they remain valid.

Arenas make it convenient to manage memory for a whole task, request, or frame.
Allocations keep stable addresses as the arena grows. You can restore an arena
to a saved mark, reset it for reuse, or release its storage with `done()`. These
operations invalidate the affected allocations together, removing the need to
free each one individually.

The runtime also provides a temporary arena, available through `temp_arena`,
for short-lived allocations. Runtime interpolated strings use this storage and
can outlive the function that created them. Their lifetime follows the arena:
resetting it invalidates those strings and any other allocations it holds.
In a frame or request loop, reset the temporary arena at a boundary where its
previous allocations are no longer needed. Copy data into longer-lived storage
when it must survive that boundary.

## Learning Nerd

Begin by building and running a small program, then learn declarations,
expressions, and functions. From there, move through branching and loops,
compound data, optional values and errors, and resource management. Modules,
generics, and interoperability build on those foundations.

Run and modify the examples as you go. Pay attention to the compiler's
diagnostics and to differences from C, especially in type inference, value
matching, and ownership. The aim is to become comfortable expressing familiar
programming tasks in Nerd and to recognise where its features simplify them.
