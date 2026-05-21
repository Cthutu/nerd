Key :: enum { A B }
Event :: enum { None KeyDown(Key) }

main :: fn () -> i32 {
    event := Event.KeyDown(Key.A)
    on event {
        Event.KeyDown(as key_code) => {
            on key_code == A => return 0
        }
        else => {}
    }
    return 1
}
¬
0
¬

¬
keep
¬

¬
build
