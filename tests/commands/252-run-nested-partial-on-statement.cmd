main :: fn () -> i32 {
    result := 0

    on yes {
        yes => {
            on yes => {
                result = 42
            }
        }
    }

    return result - 42
}
¬
0
¬

¬
delete
