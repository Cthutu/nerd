Node :: plex {
    value i32
    next  ^Node
}

nodes: []Node = [
    { value: 1, next: ^nodes[1] },
    { value: 2, ... },
]

main :: fn () -> i32 {
    return nodes[0].next.value
}
¬
2
¬
¬
¬
