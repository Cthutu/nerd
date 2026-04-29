main::fn(){total:=0 defer total+=1 defer{total+=2 total+=3}return total}
¬
main :: fn () {
    total := 0
    defer total += 1
    defer {
        total += 2
        total += 3
    }
    return total
}
