-- test-platform: linux
use std.process

main :: fn () -> i32 {
    return run("echo process=yes; exit 7")
}
¬
7
¬
process=yes

¬
delete
¬
