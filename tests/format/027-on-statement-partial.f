use mod std.print

main :: fn () {
    value :: 2

    on value { 1 => prn("one") }
}
¬
use mod std.print

main :: fn () {
    value :: 2

    on value {
        1 => prn("one")
    }
}
