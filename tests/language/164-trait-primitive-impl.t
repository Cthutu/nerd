use std.io

Describe :: trait {
    describe :: fn (Self) -> string
}

impl Describe for i32 {
    describe :: fn (self: Self) => $"i32:{self}"
}

main :: fn () -> i32 {
    value: i32 = 42
    prn(value.describe())
    return value
}
¬
42
¬
i32:42

¬
hir 0
¬
