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
- End-of-line comments stay attached to the formatted line they followed.
- Consecutive end-of-line comments on plex fields, union fields, and enum
  variants align their `--` marker.
- Consecutive enum variants with explicit `= <value>` discriminants align the
  `=` marker within the same enum variant group.
- Long plex field, union field, and enum variant end-of-line comments wrap onto
  following comment lines with the `--` marker aligned and continuation text
  indented.
- Existing wrapped plex field, union field, and enum variant end-of-line
  comments are reattached before wrapping so repeated formatting preserves
  continuation text.
- Comment-only lines inside code constructs remain standalone and are not
  crossed by alignment or use-sorting groups.
- Empty rows between enum variant groups are preserved, with repeated empty
  rows collapsed to a single empty row.

## Code Layout Rules

- Core syntax spacing is normalised rather than preserving arbitrary user spacing.
- Binding and function tokens such as `::`, `:`, `=`, and `=>` use normalised spacing.
- Binary operators use surrounding spaces.
- Explicit grouping parentheses are preserved.
- Vertical spacing remains conservative apart from collapsing repeated blank lines.

## Alignment Rules

- Consecutive one-line local variable declarations in the same source paragraph
  align their symbol, type, and `=` columns.
- Consecutive one-line local constant bindings in the same source paragraph
  align their symbol, optional type, and second `:` columns.
- A source paragraph that mixes local constant bindings and local variable
  declarations is formatted as separate sub-paragraphs, split by a blank line.
- Alignment does not cross blank lines.
- Single-line paragraphs use the normal compact spacing unless the declaration
  form needs explicit type spacing.
- If an aligned right-hand value does not fit within the formatter width, the
  operator stays on the declaration line and the value moves to the next line,
  indented by one extra level.

## Current Scope

The current formatter is intentionally narrow:

- preserve source structure where possible
- reflow comment-only paragraphs
- produce stable `.format` snapshots for formatter tests

The formatter implementation lives in
[src/compiler/format/format.c](/home/matt/nerd/src/compiler/format/format.c)
and uses the CST when structure-sensitive formatting is required.
