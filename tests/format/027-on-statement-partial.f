use std.io

main :: fn () {
    value :: 2

    on value { 1 => prn("one") }
}
¬
use std.io

main :: fn () {
    value :: 2

    on value {
        1 => prn("one")
    }
}
