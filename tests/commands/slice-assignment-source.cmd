-- test-release
main :: fn () {
    xs := [1, 2, 3]
    xs[..2] = xs[1..4]
}
¬
127
¬

¬
delete
¬
-r
¬
run
¬
fatal: slice assignment range out of bounds
