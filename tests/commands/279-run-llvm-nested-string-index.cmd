main :: fn (args: []string) -> i32 {
    on args.count != 4 => return 10
    on args[1][0] != 'a' => return 11
    return args[1][0].as(i32) - 'a'
}
¬
0
¬

¬
default-main
¬
-- alpha "two words" -x
