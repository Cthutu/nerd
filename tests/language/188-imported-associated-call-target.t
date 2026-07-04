use test.associated_b
use test.associated_a

main :: fn () -> i32 {
    value := on A.new() {
        Some(value)  => value
        None         => return 1
    }
    return value.id
}
¬
42
¬

¬

¬
