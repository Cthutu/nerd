use std.io

main :: fn () {
    details := (42, "matt")
    name := "matt"

    on details {
        (42, name) => prn($"Hello {name}!")
        else => prn("Whatever!")
    }

    other := "jane"
    on details {
        (42, other) => prn("wrong")
        (42, as matched) => prn($"Matched {matched}.")
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
