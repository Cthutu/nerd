use std.io

Event :: enum {
    Closed
    Resized {
        width u16
        height u16
    }
}

main :: fn () -> i32 {
    event := Event.Resized { width: 80, height: 25 }
    result := on event {
        Resized { width: observed_width, height: observed_height } =>
            observed_width.as(i32) + observed_height.as(i32)
        Closed => 0
    }
    prn($"{result}")
    return 0
}
¬
0
¬
105

¬
delete
¬
