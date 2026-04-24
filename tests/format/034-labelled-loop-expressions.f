main :: fn () {
for i := 0; i < 5; i += 1 $outer { on i == 2 => continue $outer
on i == 4 => break $outer }
value :: for $value { break $value 7 }
}
¬
main :: fn () {
    for i := 0; i < 5; i += 1 $outer {
        on i == 2 => continue $outer
        on i == 4 => break $outer
    }
    value :: for $value {
        break $value 7
    }
}
