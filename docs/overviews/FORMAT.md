# Format Rules

This document records the current formatter rules for Nerd source files.

The formatter should be deterministic and idempotent.

## General Rules

- Formatting already-formatted source should reproduce the same bytes.
- Formatted output should end with one trailing newline.
- Runs of blank lines should collapse to a single empty line.
- Existing indentation should be preserved unless a specific rule changes it.

## Comment Rules

- A comment with text formats as `-- ` followed by exactly one space.
- An empty comment line formats as `--`.
- Consecutive comment-only lines are treated as comment paragraphs.
- Empty comment lines separate comment paragraphs.
- Comment paragraphs are reflowed to a width of 80 characters.
- Reflow keeps the current indentation of the comment block.

## Code Layout Rules

- Core syntax spacing is normalised rather than preserving arbitrary user spacing.
- Binding and function tokens such as `::`, `:`, `=`, and `=>` use normalised spacing.
- Binary operators use surrounding spaces.
- Explicit grouping parentheses are preserved.
- Vertical spacing remains conservative apart from collapsing repeated blank lines.

## Current Scope

The current formatter is intentionally narrow:

- preserve source structure where possible
- reflow comment-only paragraphs
- produce stable `.format` snapshots for formatter tests

The formatter implementation lives in
[src/compiler/format/format.c](/home/matt/nerd/src/compiler/format/format.c)
and uses the CST when structure-sensitive formatting is required.
