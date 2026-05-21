use std.io

main :: fn () {
    details := (42, "matt")
    name := "matt"

    on details {
        (42, for name) => prn($"Hello {name}!")
        else => prn("Whatever!")
    }

    other := "jane"
    on details {
        (42, for other) => prn("wrong")
        (42, matched) => prn($"Matched {matched}.")
    }
}
¬
0
¬
Hello matt!
Matched matt.

¬
delete
¬
