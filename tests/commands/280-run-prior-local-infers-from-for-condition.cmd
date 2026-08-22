main :: fn (args: []string) -> i32 {
    i := 1

    for i < args.count {
        return args[i].count.as(i32) - 5
    }
    return 1
}
¬
0
¬

¬
default-main
¬
-- alpha
