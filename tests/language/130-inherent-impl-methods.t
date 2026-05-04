io :: use std.io
use std.collections

Counter :: plex {
    value i32
}

impl Counter {
    inc :: fn (self: ^Self, amount: i32) {
        self.value += amount
    }

    get :: fn (self: Self) -> i32 {
        return self.value
    }
}

main :: fn () {
    counter: Counter
    counter.inc(7)
    counter.inc(5)

    stack: Stack[i32]
    stack.push(counter.get())
    stack.push(30)

    io.prn($"{stack.pop()} {stack.pop()} {counter.get()}")
}
¬
0
¬
30 12 12

¬

¬
