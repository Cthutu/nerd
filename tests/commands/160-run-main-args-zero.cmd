use std.io

main :: fn (args: []string) -> i32 {
    prn($"{args.count}")
    on args.count != 1 => return 1
    on args[0].count == 0 => return 2
    return 0
}
¬
0
¬
1

¬
default-main
