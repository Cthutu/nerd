main :: fn () -> i32 {
    up: f32 = 42.as(f32)
    down: f32 = 3.9
    below: f32 = -3.9

    whole := down.as(i32)
    negative := below.as(i32)
    return up.as(i32) + whole + negative
}
¬
42
¬
¬
delete
¬
--llvm-backend
