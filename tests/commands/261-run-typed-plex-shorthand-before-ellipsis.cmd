Item :: plex {
    output i32
    extra  i32
}

main :: fn () -> i32 {
    output := 42
    item := Item { output ... }
    return on item.output == 42 && item.extra == 0 => 0 else 1
}
¬
0
¬
¬
delete
