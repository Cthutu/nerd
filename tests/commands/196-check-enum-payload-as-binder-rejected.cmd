Maybe :: enum {
    None
    Some(i32)
}

main :: fn () -> i32 {
    value: Maybe = Some(1)
    return on value {
        Some(as code) => code
        None => 0
    }
}
¬
1
¬

¬
delete
¬

¬
check
