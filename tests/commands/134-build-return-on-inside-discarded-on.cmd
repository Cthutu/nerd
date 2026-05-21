Event :: enum { None KeyDown(u8) }

fallback :: fn (event: Event, byte: u8) -> Event {
    on event {
        None => {
            return on byte == 0 => Event.None else Event.KeyDown(byte)
        }
        else => return event
    }
}

main :: fn () -> i32 {
    event := fallback(Event.None, 65)
    return on event {
        KeyDown(65) => 0
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
build
