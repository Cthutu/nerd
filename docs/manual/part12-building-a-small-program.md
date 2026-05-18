# Part 12: Building A Small Program

[Manual Index](README.md) | Previous: [Interoperability With C](part11-interoperability-with-c.md) | Next: [Diagnostics And Debugging](part13-diagnostics-and-debugging.md)

This part combines the earlier concepts into a small command-line text
adventure. The goal is not to build a full game; it is to show how Nerd programs
fit together.

The examples use `input` from `std.io` and `split` from `std.string`. Those are
standard library helpers, not new language constructs.

## Build Pragmas

Pragmas are optional compiler controls:

```nerd
pragma windowed
```

Unknown pragmas are ignored. `pragma windowed` changes Windows executable builds
to use a windowed subsystem and a generated `WinMain` wrapper instead of the
normal console `main` wrapper. It has no effect on non-Windows builds.

## Start With A Loop

```nerd
use std.io

main :: fn () {
    prn("Welcome to Little Cave Adventure!")

    for {  -- main command loop
        command := input("--> ")
        on command {  -- dispatch by command text
            "quit", "q" => break
            "look", "l" => prn("You see nothing but darkness.")
            else => prn("I don't understand.")
        }
    }

    prn("Bye!")
}
```

The program loops until the user enters `quit` or `q`.

## Use Qualified Modules

For a larger file, qualified modules make dependencies clearer:

```nerd
io :: use std.io  -- keep standard I/O names qualified

main :: fn () {
    prn("Welcome!")
}
```

## Split Input

When a command needs words, split the input into a dynamic array:

```nerd
io :: use std.io
str :: use std.string

main :: fn () {
    input := io.input("--> ")
    parts := str.split(input, " ")  -- split returns a dynamic array
    defer parts.free()              -- free it before leaving the scope

    for part in parts {
        prn($"word: {part^}")
    }
}
```

`split` returns a dynamic array, so the caller frees it. `defer` keeps that
cleanup attached to the scope.

## Model State

Use enums for state with distinct cases:

```nerd
Room :: enum {
    Cave    -- one possible room
    Tunnel
}
```

Use plexes for named fields:

```nerd
Game :: plex {
    room Room  -- named field storing current room
    turns i32
}
```

Then dispatch commands with `on`:

```nerd
io :: use std.io

describe :: fn (game: Game) {
    on game.room {  -- branch on the current room
        Cave => prn("It is very dark in here.")
        Tunnel => prn("The tunnel narrows to the north.")
    }
}

handle :: fn (game: ^Game, command: string) {
    on command {
        "look", "l" => describe(game^)
        "north", "n" => game^.room = Room.Tunnel  -- mutate original state
    }
}
```

`handle` receives `^Game`, a pointer to mutable game state, so assigning through
`game^` changes the original `Game` value.

## Grow The Program In Layers

Build larger programs by adding one concept at a time:

1. A main loop.
2. Input and command matching.
3. State stored in a plex.
4. Enums for distinct cases.
5. Helpers for command handling.
6. Dynamic arrays for parsed input.
7. `defer` for cleanup.

This keeps the shape of the program clear while the language features become
more familiar.
