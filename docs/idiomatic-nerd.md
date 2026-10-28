# Idiomatic Nerd

This guide records the project's preferences for clear, natural Nerd code.
Grow it from concrete examples encountered during development. The
[learning guide](learn/README.md) teaches the language; this guide helps choose between
valid ways to express a program. Early teaching examples may introduce one
concept at a time before adopting these patterns.

## Express one decision with one `on`

When several actions are alternatives selected by the same value, put them in
one `on value { ... }`. This makes the alternatives and fallback visible
together, and evaluates the value once.

For example, avoid splitting command dispatch between separate checks and a
later match:

```nerd
action_name :: fn (command: string) -> string {
    on command == "browser" => return "Open a browser page"
    on command == "explorer" => return "Open a folder"
    return on command {
        "file" => "Choose a file"
        else => "Unknown command"
    }
}
```

Group the alternatives instead:

```nerd
action_name :: fn (command: string) -> string {
    return on command {
        "browser" => "Open a browser page"
        "explorer" => "Open a folder"
        "file" => "Choose a file"
        else => "Unknown command"
    }
}
```

The runnable
[system example](../examples/system/system.n) applies this to real desktop
operations. Its `show_selection` helper prints successful dialog selections and
preserves their errors, giving every command the same `void\SystemError`
result. The dispatch can then share one error handler.

Use `on { ... }` when one decision has different conditions rather than one
value to match:

```nerd
status :: fn (connected: bool, pending: usize) -> string {
    return on {
        !connected => "offline"
        pending > 0 => "busy"
        else => "ready"
    }
}
```

Separate `on` statements are appropriate for independent actions that may all
run. An early argument-count guard is also a separate decision: it ensures an
argument exists before dispatch indexes it. The preference is one construct
per decision, rather than a limit on the number of `on` expressions in a function.

See [Branching with `on`](learn/09-Branching-and-Pattern-Matching.md) for the available
forms and their semantics.

## Let branches produce their result

When a branch chooses a value, bind or return the `on` expression directly:

```nerd
label :: fn (enabled: bool) -> string {
    return on enabled { yes => "enabled" else => "disabled" }
}
```

This keeps the choice and its result together. Prefer it to declaring a mutable
local and assigning that local in each branch. Keep mutation when the value
really changes over time.

When branches have the same failure policy, handle the combined result once.
Propagate errors with `?` through small helpers and translate them into an exit
status or user-facing message at the application boundary, as the system example
does. Keep separate handlers where recovery or reporting differs.
