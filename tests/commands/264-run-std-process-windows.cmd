-- test-platform: windows
use std.process

main :: fn () -> i32 {
    string_status := run("exit /B 7")
    arguments : [3]string = ["cmd.exe", "/C", "exit /B 9"]
    array_status := run(arguments[..])
    empty : []string

    on string_status != 7 => return 1
    on array_status != 9 => return 2
    on run(empty) != -1 => return 3
    return 0
}
¬
0
¬

¬
delete
¬
