main :: fn () {
for i := 0; i < 6; i += 1 { on i == 2 => continue
on i == 5 => break
total += i }
}
¬
main :: fn () {
    for i := 0; i < 6; i += 1 {
        on i == 2 => continue
        on i == 5 => break
        total += i
    }
}
