file_name :: @file
line_number :: @line

main :: fn () -> i32 {
    on file_name.count == 0 => return 1
    return (line_number - 2).as(i32)
}
¬
0
¬

¬

¬

