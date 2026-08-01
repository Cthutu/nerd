# `std.term`

The `std.term` module supports interactive terminal applications. It manages
raw input, an alternate screen, keyboard and mouse events, timed simulation,
and a retained cell framebuffer.

Import it with:

```nerd
use std.term
```

## A Minimal Application

This program exits when Escape is pressed and draws coloured text:

```nerd
use std.term
use std.time

handle_input :: fn (input: TermInput) {
    on input.key_pressed(Escape) => term_done()
}

present :: fn (_frame: TermPresent) {
    term_fb_clear()
    term_fb_text(1,
                 1,
                 "Hello, World!",
                 rgb(255, 255, 0),
                 rgb(255, 0, 0))
}

main :: fn () {
    term_init()
    term_hook_input(handle_input)
    term_hook_presentation(from_ms(33), present)
    term_run()
}
```

`term_init` enters terminal application mode. `term_run` processes input and
runs registered hooks until `term_done` requests shutdown. Terminal state is
restored by the loop before `term_run` returns.

## Hooks

`std.term` has three hook types:

| Hook | Purpose |
| --- | --- |
| `term_hook_input` | React to newly received input. |
| `term_hook_simulation` | Update application state at a requested interval. |
| `term_hook_presentation` | Draw the current state into the framebuffer. |

Input and simulation hooks receive keyboard and mouse snapshots. Simulation
hooks also receive elapsed time. Each hook may be given an optional `^void`
user-data pointer, which is returned in its callback payload.

## Physical Keys And Text

Keyboard input distinguishes a physical key from the text produced by the
active keyboard layout.

`TermScanCode.Q`, for example, identifies the same physical key whether it
produces `q`, `Q`, or a character from another layout. Shifted characters are
therefore not separate scan codes.

The `events` slice on `TermInput` and `TermSimulate` contains ordered
`TermInputEvent` values received during the current terminal step:

```nerd
read_input :: fn (input: TermInput) {
    for event in input.events {
        on event {
            Key(key) => on key.action {
                Press   => {}
                Repeat  => {}
                Release => {}
            }

            Character(codepoint) => {
                -- Insert `codepoint` into an editor document.
            }

            else => {}
        }
    }
}
```

A text-producing key is recorded as a `Key` event followed by a `Character`
event when the terminal supplies both. Function keys, arrows, and modifiers
produce key events without character events. Pasted or legacy input may produce
a character whose scan code is `Unknown`.

The event slice is borrowed from `std.term` and remains valid only during the
callback.

## Keyboard Snapshots

`TermKeyboard` tracks physical scan codes with `down`, `pressed`, and
`released` state. Prefer the methods on callback payloads:

```nerd
simulate :: fn (sim: TermSimulate) {
    on sim.key_down(W) => move_up()
    on sim.key_pressed(Space) => jump()
    on sim.key_released(Escape) => term_done()
}
```

Equivalent lower-level helpers accept a `TermKeyboard` directly:

```nerd
term_key_down(sim.keyboard, W)
term_key_pressed(sim.keyboard, Space)
term_key_released(sim.keyboard, Escape)
```

The `modifiers` field uses `TERM_MOD_SHIFT`, `TERM_MOD_CONTROL`,
`TERM_MOD_ALT`, `TERM_MOD_SUPER`, `TERM_MOD_META`, and the lock-state flags.

## Terminal Capabilities

Windows console input supplies physical keys, modifiers, text, repeats, and
releases directly. On VT-style terminals, `std.term` negotiates enhanced Kitty
keyboard reporting. Supporting terminal emulators can then provide the same
information through escape sequences.

Traditional terminal input supplies translated UTF-8 bytes and a smaller set
of escape sequences. It cannot reliably report physical positions, held keys,
or releases. In that fallback mode, key presses are exposed as one-step pulses.

Inspect `input.keyboard.capabilities` before requiring game-style key state:

```nerd
caps := input.keyboard.capabilities
on !caps.key_releases => {
    -- Offer pulse-based controls or explain the terminal requirement.
}
```

The capability fields are `physical_keys`, `key_releases`, `modifiers`, and
`associated_text`. They describe what the active input backend can report, not
what the application requested.

## Key Names

`term_scan_code_name(scan_code)` returns a stable canonical key name.
`input.key_name(scan_code)` and `sim.key_name(scan_code)` use a layout label
reported by the terminal when one has been observed, with the canonical name as
a fallback. Dynamically produced labels borrow terminal temporary storage and
should be copied if they must outlive the current callback.

Keyboard layout and process locale are separate concepts. The terminal or
operating system performs text translation; Nerd decodes the resulting text as
Unicode.

## Mouse Input

`TermMouse` contains the last cell position, held buttons, press and release
edges, and wheel movement. Use `term_mouse_down`, `term_mouse_pressed`, and
`term_mouse_released` with `TERM_MOUSE_LEFT`, `TERM_MOUSE_RIGHT`, or another
button flag.

## Framebuffer Drawing

The framebuffer stores terminal cells until they are presented. Common drawing
functions include:

- `term_fb_clear`
- `term_fb_put`
- `term_fb_text`
- `term_fb_fill_rect`
- `term_fb_paint_rect`
- `term_fb_box`

Coordinates are zero-based. A view created with `term_view` clips subsequent
drawing; `term_view_reset` restores the full-terminal view.

Colours are written as ink and paper values. Construct a 24-bit colour with
`rgb(r, g, b)`. `COLOUR_TRANSPARENT` leaves the existing ink or paper unchanged
where an API supports it.

Presentation callbacks should redraw the current application state. The
framebuffer tracks dirty cells so unchanged cells do not need to be written to
the terminal again.

## Terminal Size

`TermInput`, `TermSimulate`, and `TermPresent` contain the current terminal
width and height. `term_init` also accepts minimum dimensions and a
`TermTooSmallPolicy`.

`PauseSimulation` is the default. It pauses simulation and displays the
built-in size warning. `ContinueSimulation` continues state updates while
`std.term` owns presentation until the terminal is large enough again.

## Direct Terminal Helpers

Applications that do not use the retained framebuffer can use helpers such as
`term_print`, `term_cls`, `term_cursor_move`, and `term_cursor_enable`.
Framebuffer applications should normally draw from a presentation hook instead.
