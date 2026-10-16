Dispatch :: plex { calls i32 }
impl Dispatch {
    apply :: fn [A, R] (self: ^Self, callback: fn (argument: A) -> R, argument: A) -> R {
        self.calls += 1
        return callback(argument)
    }
}
square :: fn (number: ^i32) -> i32 { return number^ * number^ }
length :: fn (text: string) -> usize { return text.count }
erased :: fn (pointer: ^void) -> i32 { return pointer.as(^i32)^ }
main :: fn () -> i32 {
    dispatch : Dispatch
    number : i32 = 7
    assert dispatch.apply(square, ^number) == 49
    assert dispatch.apply(length, "typed") == 5
    assert dispatch.apply(erased, ^number) == 7
    assert dispatch.apply[(^i32, i32)](square, ^number) == 49
    assert dispatch.calls == 4
    prn("Generic callback method inference passed")
    return 0
}
¬
0
¬
Generic callback method inference passed

¬
hir 0
¬
; nerd llvm-ir 0
