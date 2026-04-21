# Format Rules

This document records the current formatting rules for Nerd source files. The
formatter should be deterministic and idempotent.

## General Rules

- Formatting already-formatted source should reproduce byte-for-byte identical
  output.
- Formatted output should always end with a trailing newline.
- Runs of blank lines should be reduced to a single empty line.
- Existing indentation should be preserved unless a later rule says otherwise.

## Comment Rules

- A comment with text should use `-- ` followed by exactly one space before the
  comment text.
- An empty comment line should format as exactly `--`.
- Consecutive comment-only lines should be treated as comment paragraphs.
- Empty comment lines should separate comment paragraphs.
- Comment paragraphs should be reflowed to a width of 80 characters.
- Comment reflow should preserve the current indentation of the comment block.

## Code Layout Rules

- Core syntax spacing should be normalised rather than preserving arbitrary
  user spacing.
- The formatter should use spaces around binding and function tokens such as
  `::` and `=>`.
- Binary operators should have surrounding spaces.
- Keyword-adjacent spacing should be normalised consistently.
- Explicit grouping parentheses should be preserved.
- Vertical spacing should be conservative for now apart from collapsing runs of
  blank lines to one empty line.

## Current Scope

The current formatter implementation is intentionally narrow:

- preserve source structure where possible
- reflow comment-only paragraphs
- provide a stable `.format` output for formatter tests

More rules can be added here as the formatter grows.

## Planned CLI Behaviour

The current formatter writes to a sibling `.format` file so the workflow stays
non-destructive while the formatting rules are still changing.

Later, once the formatter is more stable, the CLI should change to:

- rewrite the input file in place by default
- support `-o <filename>` to redirect formatted output elsewhere

When that happens, the formatter tests should be updated to match the new CLI
behaviour without losing snapshot-style verification.
