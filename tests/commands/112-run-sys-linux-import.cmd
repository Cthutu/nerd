-- test-platform: linux
use std.io
use sys.linux

main :: fn () -> i32 {
    page := sysconf(_SC_PAGESIZE)
    prn($"page={page > 0}")
    return on page > 0 {
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
