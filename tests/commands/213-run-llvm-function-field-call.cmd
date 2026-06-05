MeasureFn :: fn (i32) -> i32

double :: fn (value: i32) -> i32 {
    return value * 2
}

Box :: plex {
    measure MeasureFn
}

main :: fn () -> i32 {
    box := Box { measure: double }
    return box.measure(21)
}
¬
42
¬

¬
delete
¬
