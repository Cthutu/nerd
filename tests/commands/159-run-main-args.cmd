use std.io

main :: fn (args: []string) -> i32 {
    on args.count != 4 => return 10
    on args[0].count == 0 => return 11
    on args[1] != "alpha" => return 12
    on args[2] != "two words" => return 13
    on args[3] != "-x" => return 14

    prn(args[1])
    prn(args[2])
    prn(args[3])
    return 0
}
¬
0
¬
alpha
two words
-x

¬
default-main
¬
-- alpha "two words" -x
