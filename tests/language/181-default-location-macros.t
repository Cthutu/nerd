capture_file :: fn (file: string = @file) => file
capture_line :: fn (line: u32 = @line) => line

main :: fn () -> i32 {
    on capture_file() != @file => return 1
    line := capture_line()
    on line != 6 => return line.as(i32)
    return 0
}
¬
0
¬

¬

¬

