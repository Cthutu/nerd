set_value :: fn (value: ^u64) -> bool {
    value^ = 64
    return yes
}

conditional_value :: fn (set: bool) -> u64 {
    value : u64 = 0
    on set => {
        on set_value(^value) => { }
    }
    return value
}

main :: fn () -> i32 {
    on conditional_value(yes) != 64 => return 1
    return on conditional_value(no) == 0 => 0 else 2
}
¬
0
¬
¬
delete
