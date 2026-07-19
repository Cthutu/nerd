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
- A comment line that starts with `--|` is preserved by the formatter. Use this
  for examples, diagrams, or text where line breaks and spacing are meaningful.
- End-of-line comments stay attached to the formatted line they followed.
- Consecutive end-of-line comments on plex fields, union fields, enum variants,
  and multiline plex literal fields align their `--` marker.
- Enum variants with explicit `= <value>` discriminants align the `=` marker
  within the same enum variant group, even when variants without explicit values
  appear between them.
- Enum variant alignment groups are split only by empty rows.
- Long plex field, union field, enum variant, and multiline plex literal
  end-of-line comments wrap onto following comment lines with the `--` marker
  aligned and continuation text indented.
- Existing wrapped plex field, union field, enum variant, and multiline plex
  literal end-of-line comments are reattached before wrapping so repeated
  formatting preserves continuation text.
- Comment-only lines inside code constructs remain standalone.
- Empty rows between enum variant groups, plex or union field groups, and
  multiline plex literal field groups are preserved, with repeated empty rows
  collapsed to a single empty row.

## Code Layout Rules

- Core syntax spacing is normalised rather than preserving arbitrary user spacing.
- Binding and function tokens such as `::`, `:`, `=`, and `=>` use normalised spacing.
- Binary operators use surrounding spaces.
- String continuation fragments that start on separate source lines remain on
  separate lines, indented one level beyond the first string. This preserves
  intentional row and diagram boundaries while same-line fragments may still
  be folded together.
- Long single-branch boolean `on condition => action` statements whose
  condition is a `&&` or `||` chain wrap each chained condition term onto a
  continuation line.
- Explicit grouping parentheses are preserved.
- Vertical spacing remains conservative apart from collapsing repeated blank lines.

## Alignment Rules

- Consecutive one-line local variable declarations in the same source paragraph
  align their symbol, type, and `=` columns.
- Consecutive one-line local constant bindings in the same source paragraph
  align their symbol, optional type, and second `:` columns.
- Renamed entries inside an `ffi "lib" { ... }` block align their `::` marker
  within the same FFI block group, including any `pub ` prefix.
- Multiline plex literals, including literals whose type target is qualified
  through a module, align field `:` markers and values only within consecutive
  single-line field runs. Multiline fields use compact `field: value` spacing.
  Trailing comments still align within the same field group.
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
## Compound Functions

The formatter writes `fn {` and `}` on their own declaration lines and places
each member on one indented line. Qualified names and comments remain attached
to their member.
