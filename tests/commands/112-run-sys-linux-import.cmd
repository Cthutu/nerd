-- test-platform: linux
use std.io
use os.linux

main :: fn () -> i32 {
    message := "page=yes\n"
    written := sys_write(STDOUT_FILENO, message.data.as(^void), message.count)
    return on written == message.count.as(isize) {
        yes  => 0
        else => 1
    }
}
¬
0
¬
page=yes

¬
delete
¬
--llvm
