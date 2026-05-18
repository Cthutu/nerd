use std.io

main :: fn () -> i32 {
    ran_else := 0
    for i := 0; i < 0; i += 1 {
        ran_else = 1
        break
    } else {
        ran_else = 2
    }
    on ran_else != 2 => return 1

    break_else := 0
    for i := 0; i < 0; i += 1 {
        break_else = 1
        break
    } else {
        break_else = 2
        break
    }
    on break_else != 2 => return 2

    skipped_else := 0
    for i := 0; i < 2; i += 1 {
        skipped_else = 3
        break
    } else {
        skipped_else = 4
    }
    on skipped_else != 3 => return 3

    prn("ok")
    return 0
}
¬
0
¬
ok

¬
delete
¬
