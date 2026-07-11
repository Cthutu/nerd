Key :: enum { A B }
Event :: enum { None KeyPress(Key) }

key_from_byte :: fn (byte: u8) -> ?Key {
    return on byte {
        'A'   => A
        else  => nil
    }
}

event_from_byte :: fn (byte: u8) -> Event {
    key := key_from_byte(byte)
    return on key {
        key_code => {
            event := Event.KeyPress(key_code)
            break event
        }
        else => Event.None
    }
}

main :: fn () -> i32 {
    return on event_from_byte('A') {
        KeyPress(code) => code.as(i32) - Key.A.as(i32)
        else => 1
    }
}
¬
0
¬

¬
keep
¬

¬
run
