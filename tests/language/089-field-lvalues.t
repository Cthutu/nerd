Node :: plex {
    value i32
}

main :: fn () -> i32 {
    node: Node = Node { value: 1 }
    node.value = 3

    ptr: ^Node = ^node
    ptr.value = 5
    ptr^.value = 7

    return node.value - 7
}
¬
0
¬

¬

¬
