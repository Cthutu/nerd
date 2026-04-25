use mod std.print

main :: fn () {
    i  :: 2
    ff :: 3.14
    s  :: "Hello, world!"

    prn($"{s}  i = {i} and f = {ff}!")

    prn(on i { 0 => "zero" else as x => $"non-zero: {x}" })
}
¬
use mod std.print

main :: fn () {
    i  :: 2
    ff :: 3.14
    s  :: "Hello, world!"

    prn($"{s}  i = {i} and f = {ff}!")

    prn(on i {
        0         => "zero"
        else as x => $"non-zero: {x}"
    })
}
