on "release" {
    answer :: 7
}

on !"debug" {
    fallback :: 9
}

main :: fn () -> i32 {
    return answer + fallback
}
¬
16
¬

¬
delete
¬
-r
