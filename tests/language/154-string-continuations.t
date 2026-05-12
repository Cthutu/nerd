use std.io

main :: fn () -> i32 {
    name := "Matt"
    plain := "Hello, " +"world!"

    on plain != "Hello, world!" => return 1
    prn($"Hello {name}, " +"again {name}!")
    return 0
}
¬
0
¬
Hello Matt, again Matt!
¬

¬
