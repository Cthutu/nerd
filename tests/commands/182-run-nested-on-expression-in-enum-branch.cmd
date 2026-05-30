Mode :: enum {
    A
}

min_value :: fn (mode: Mode, width: u16, height: u16) -> u32 {
    on mode {
        A => {
            scale_x := width.as(u32)
            scale_y := height.as(u32)
            scale := on scale_x < scale_y => scale_x else scale_y
            return scale
        }
    }

    return 0
}

main :: fn () -> i32 {
    return min_value(Mode.A, 3, 5).as(i32)
}
¬
3
¬

¬
