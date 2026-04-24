main :: fn () {
    i  :: 2
    ff :: 3.14
    s  :: "Hello, world!"

    prn($"{s}  i = {i} and f = {ff}!")

    prn(on i { 0 => "zero" x @ else => $"non-zero: {x}" })
}
¬
main :: fn () {
    i  :: 2
    ff :: 3.14
    s  :: "Hello, world!"

    prn($"{s}  i = {i} and f = {ff}!")

    prn(on i {
        0        => "zero"
        x @ else => $"non-zero: {x}"
    })
}
