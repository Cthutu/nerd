Counter :: plex {
    value i32
}

impl Counter {
    set :: fn (self: ^Self, amount: i32 = 1, scale: i32 = 2) {
        self.value = amount * scale
    }

    add :: fn (self: ^Self, amount: i32 = 1) {
        self.value += amount
    }

    get :: fn (self: Self) -> i32 {
        return self.value
    }
}

add :: fn (a: i32, b: i32 = 10, c: i32 = a + b) => a + b + c

main :: fn () {
    counter: Counter
    counter.set(amount = 7, scale = 3)
    counter.add(amount = 5)
    return add(a = 1, b = 2, c = 3) + counter.get()
}
¬
32
¬

¬

¬
