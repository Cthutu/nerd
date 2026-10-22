main :: fn () {
    xs := [1, 2, 3]
    xs[1..1] = xs[..1]
}
¬
127
¬

¬
delete
¬

¬
run
¬
fatal: slice assignment lengths differ
