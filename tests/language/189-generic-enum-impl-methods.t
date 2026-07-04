Maybe :: enum [T] {
    None
    Some(T)
}

impl Maybe[T] {
    expect :: fn (self: Self, fallback: T) -> T {
        return on self {
            Some(value) => value
            None        => fallback
        }
    }
}

main :: fn () -> i32 {
    first  : Maybe[i32] = Some(42)
    second : Maybe[i32] = None
    return first.expect(0) + second.expect(1) - 1
}
¬
42
¬

¬

¬
