Node :: plex {
    value i32
    next  ^Node
}

nodes: []Node = [
    { value: 1, next: second },
    { value: 2, next: first },
]

first  :: ^nodes[0]
second :: ^nodes[1]

main :: fn () -> i32 {
    return nodes[0].next.value + nodes[1].next.value
}
¬
3
¬
¬
¬
