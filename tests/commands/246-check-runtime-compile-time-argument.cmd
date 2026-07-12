choose :: fn (enabled :: bool) -> i32 {
    return on enabled => 1 else 0
}

main :: fn () -> i32 {
    runtime := yes
    return choose(runtime)
}
¬
1
¬

¬
delete
¬

¬
check
