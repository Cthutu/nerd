main :: fn () -> i32 {
    values: [2]i32 = [10, 20]
    first          := ^values[0]
    second         := ^values[1]

    on !(first < second && first <= second && second > first &&
         second >= first) => return 1
    return 0
}
¬
0
¬

¬
delete
¬
