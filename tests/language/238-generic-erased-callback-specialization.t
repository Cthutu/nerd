Invocation :: plex [A] {
    callback fn (argument: A) -> i32
    argument A
}
run :: fn [A] (context: ^void) -> i32 {
    invocation := context.as(^Invocation[A])
    return invocation.callback(invocation.argument)
}
from_pointer :: fn (number: ^i32) -> i32 { return number^ }
from_value :: fn (number: i32) -> i32 { return number * number }
from_float :: fn (number: f64) -> i32 { return (number + 1.0).as(i32) }
main :: fn () -> i32 {
    number : i32 = 7
    pointer : Invocation[^i32]
    pointer.callback = from_pointer
    pointer.argument = ^number
    integer : Invocation[i32]
    integer.callback = from_value
    integer.argument = 9
    decimal : Invocation[f64]
    decimal.callback = from_float
    decimal.argument = 10.0
    first: fn (context: ^void) -> i32 = run[^i32]
    second: fn (context: ^void) -> i32 = run[i32]
    third: fn (context: ^void) -> i32 = run[f64]
    assert first(^pointer) == 7
    assert second(^integer) == 81
    assert third(^decimal) == 11
    prn("Explicit erased callback specializations passed")
    return 0
}
¬
0
¬
Explicit erased callback specializations passed

¬
hir 0
¬
; nerd llvm-ir 0
