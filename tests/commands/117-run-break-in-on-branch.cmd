use std.io

main :: fn () -> i32 {
    count := 0
    for {
        count += 1
        on count == 1 => {
            prn("breaking")
            break
        } else {
            prn("else")
        }
        on count > 3 => return 2
    }
    prn("done")
    return 0
}
¬
0
¬
breaking
done

¬
delete
¬
