use std.io

Event :: enum {
    Closed
    Resized {
        width u16
        height u16
    }
}

score :: fn (event: Event) -> i32 {
    return on event {
        Resized({ width }) => width.as(i32)
        Closed => 0
    }
}

main :: fn () -> i32 {
    event := Event.Resized({ width: 80, height: 25 })
    prn($"{score(event)}")
    return 0
}
¬
0
¬
80

¬
delete
¬
