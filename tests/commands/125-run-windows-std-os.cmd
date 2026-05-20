-- test-platform: windows
use std.time
use std.term

main :: fn () {
    start := now()
    sleep_ms(1)
    finish := now()
    _size := term_get_size()

    assert finish >= start
}
¬
0
¬

¬
delete
¬
