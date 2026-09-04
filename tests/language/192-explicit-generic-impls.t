Stack :: plex [U] {
    value U
}

impl [T] Stack[i32] {
    identity :: fn (self: Self, value: T) -> T {
        copy: T = value
        return copy
    }
}

impl [T] []T
where T: Eq {
    contains :: fn (self: Self, value: T) -> bool {
        for item in self {
            on item^ == value => return true
        }
        return false
    }
}

main :: fn () -> i32 {
    stack  : Stack[i32] = { value: 0 }
    number := stack.identity(40)
    word   := stack.identity[string]("ok")
    values := [1, 2, 3]

    on !values.contains(2) => return 1
    on values.contains(4) => return 2
    on [].contains(1) => return 3
    return number + word.count.as(i32) - 42
}
¬
0
¬
¬
¬
