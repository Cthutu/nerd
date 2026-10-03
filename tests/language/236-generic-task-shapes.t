Box :: plex [T] {
    callback fn (context: ^void) -> T
    context ^void
    result ?T
}
impl [T] Box[T] {
    run :: fn (self: ^Self) -> ?T {
        self.result = self.callback(self.context)
        return self.result
    }
}
pointer :: fn [T] (value: ^T) -> ?^T {
    return value
}
Optional :: plex [T] { value ?T }
impl [T] Optional[T] {
    get :: fn (self: ^Self) -> ?T { return self.value }
}
Error :: enum { Failed }
Outcome :: plex [T] { value T\Error }
impl [T] Outcome[T] {
    get :: fn (self: ^Self) -> T\Error { return self.value }
}
Link :: plex { value ^i32 }
callback :: fn (_context: ^void) -> i32 { return -1 }
read :: fn (context: ^void) -> i32 { return context.as(^i32)^ }
main :: fn () -> i32 {
    assert callback(nil) == -1
    number : i32 = 42
    task : Box[i32]
    task.callback = read
    task.context = (^number).as(^void)
    on task.run() { result => { assert result == 42 } else => { assert no } }
    on pointer(^number) { p => { assert p^ == 42 } else => { assert no } }
    optional : Optional[i32]
    optional.value = number
    on optional.get() { n => { assert n == 42 } else => { assert no } }
    outcome : Outcome[i32]
    outcome.value = number
    on outcome.get() { n => { assert n == 42 } else => { assert no } }
    -- Address publication through a constructor must not reinitialise its
    -- target on each iteration in release LLVM builds.
    count : i32
    links : [4]Link
    for i in [0 .. 4] {
        links[i] = Link { value: ^count }
        links[i].value^ += 1
    }
    assert count == 4
    prn("Generic task shapes passed")
    return 0
}
¬
0
¬
Generic task shapes passed

¬
hir 0
¬
; nerd llvm-ir 0
