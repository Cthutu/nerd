-- test-platform: linux
use std.process

main :: fn () -> i32 {
    string_status := run("echo string=yes; exit 7")
    arguments : [5]string = ["/bin/sh",
                             "-c",
                             "echo array=$1; exit 9",
                             "sh",
                             "argument value"]
    array_status := run(arguments[..])
    empty : []string

    on string_status != 7 => return 1
    on array_status != 9 => return 2
    on run(empty) != -1 => return 3
    return 0
}
¬
0
¬
string=yes
array=argument value

¬
delete
¬
