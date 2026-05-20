# Coding Standards

These standards apply to Nerd Standard Library (NSL) source files in this
repository.

## Names

- Use `snake_case` for variables.
- Use `snake_case` for functions.
- Use `PascalCase` for types.
- Prefer names that describe library behaviour directly. Avoid abbreviations
  unless they are already established in Nerd or the surrounding module.

## Spelling

Use British English spelling in all documentation, comments, diagnostics,
examples, and user-facing text.

## Platforms

Put all platform-specific functionality inside a platform guard such as
`on "linux" { ... }` or `on "windows" { ... }`. This includes platform FFI
declarations, constants, types, helper functions, and imports. Public APIs
should stay outside platform guards when they are intended to be portable.

## File Header

Each Nerd source file must begin with comments headed `Nerd Standard Library
(NSL)`, followed by a copyright message belonging to Matt Davies and a short
paragraph describing the file's purpose and the main concepts it provides.

Example:

```nerd
-- Nerd Standard Library (NSL)
--
-- Copyright (c) 2026 Matt Davies
--
-- Provides string searching helpers used by higher-level text utilities.
```

## Comments

Every type and function defined by the library must have a comment immediately
before it. The comment should describe what the item does, including ownership,
lifetimes, mutation, or failure behaviour when relevant.

Example:

```nerd
-- Returns true when the slice contains no elements.
is_empty :: fn (items: []i32) -> bool {
    return items.len == 0
}
```

Every field in a `plex` must have a comment after it on the same line. Plex
field definitions use `field Type` with no colon.

Example:

```nerd
-- Tracks the input text and current read position for a parser.
Reader :: plex {
    source string -- Text being read.
    offset usize  -- Current byte offset in source.
}
```

## Formatting

Format each changed Nerd source file with `nerd format` before checking it.
After formatting, run:

```sh
just check
```

Run `just test` when source-level tests exist or when the change affects
observable behaviour.
