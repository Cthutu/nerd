main :: fn () {
    total := 0
    for i := 0; i < 6; i += 1 {
        on i == 2 => again
        on i == 5 => break
        total += i
    }
    return total
}
¬
8
¬

¬
delete
¬
