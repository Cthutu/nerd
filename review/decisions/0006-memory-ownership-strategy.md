# 0006: Memory Ownership Strategy

Status: accepted
Date: 2026-05-12

## Context

MS7 measurements added phase-level counters for heap allocations, arena
allocations, arena commits, and dynamic-array growth. The goal is to make the
compiler's memory model explicit enough that future performance work improves
the right lifetime boundary instead of moving churn around.

The compiler has three main allocation mechanisms:

- arenas, for bump allocation with phase or request lifetime
- dynamic arrays, for tables that grow as the source is discovered
- heap allocation, mostly through dynamic arrays and escaped ownership

## Decision

Use arenas for phase-lifetime products and scratch text.

Good arena owners:

- source copies owned by a document, command, or program load
- temporary formatting strings and backend command/text construction
- LLVM text rendering buffers
- per-request LSP response objects
- short-lived grouping/alignment work where all allocations die together

Use dynamic arrays for compiler tables whose final size is not known when the
phase starts.

Good dynamic-array owners:

- lexer token, literal, symbol, and comment tables
- AST/CST node and side tables
- sema declarations, scopes, locals, type tables, and diagnostics
- HIR entity, statement, expression, type, and binding tables
- module lists and import/export lists
- LSP result lists where entries are appended conditionally

Use direct heap allocation only when ownership escapes a phase or when the
allocation is already represented as a dynamic-array backing store.

Good heap owners:

- dynamic-array backing storage
- map/interner tables with explicit teardown
- process or document lifetime objects that cannot be expressed as arena
  products

Avoid direct heap allocation for temporary strings, per-node scratch state, and
layout work.

## Consequences

The default compiler shape remains simple:

- a compile owns long-lived source/module/front-end products until teardown
- each compiler phase can allocate freely into its own tables
- backend and formatter temporary text should stay arena-based
- LSP keeps document-lifetime arenas but should avoid creating fresh scratch
  arenas inside hot incremental paths when an existing request scratch arena
  would do

Measurement interpretation follows this ownership model:

- High dynamic-array growth means table sizing or pre-reservation should be
  reviewed.
- High arena allocation count with small byte totals means scratch arena churn
  should be reviewed.
- High heap allocation count outside array growth needs an ownership audit.

## Follow-Up

1. Pre-size sema and HIR tables from earlier token/AST counts where simple.
2. Reuse formatter scratch arenas instead of creating many one-page arenas for
   small formatting jobs.
3. Reuse LSP request/document scratch arenas during incremental reanalysis
   where lifetimes are already bounded.
4. Keep LLVM text rendering arena-based, but reduce helper dynamic arrays in
   small modules if measurements show it matters.
