Key :: enum { A B }
Event :: enum { None KeyPress(Key) }

key_from_byte :: fn (byte: u8) -> Option[Key] {
    return on byte {
        'A'   => Some(A)
        else  => None
    }
}

event_from_byte :: fn (byte: u8) -> Event {
    key := key_from_byte(byte)
    return on key {
        Some(key_code) => {
            event := Event.KeyPress(key_code)
            break event
        }
        None => Event.None
    }
}

main :: fn () -> i32 {
    return on event_from_byte('A') {
        KeyPress(code) => code.as(i32) - Key.A.as(i32)
        None => 1
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
