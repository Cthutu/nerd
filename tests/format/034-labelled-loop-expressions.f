main :: fn () {
$outer for i := 0; i < 5; i += 1 { on i == 2 => continue $outer
on i == 4 => break $outer }
value :: $value for { break $value 7 }
}
¬
main :: fn () {
    $outer for i := 0; i < 5; i += 1 {
        on i == 2 => continue $outer
        on i == 4 => break $outer
    }
    value :: $value for {
        break $value 7
    }
}
