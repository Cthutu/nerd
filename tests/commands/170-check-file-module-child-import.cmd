-- test-platform: linux
use os.linux.x11

main :: fn () -> i32 {
    display : XDisplay = nil
    on display == nil => return 0
    return 1
}
¬
0
¬

¬
delete
¬

¬
check
