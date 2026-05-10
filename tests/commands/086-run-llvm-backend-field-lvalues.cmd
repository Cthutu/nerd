Node :: plex {
    value i32
}

main :: fn () -> i32 {
    node: Node = Node { value: 1 }
    node.value = 3
    node.value += 2
    node.value *= 2
    node.value /= 2
    node.value -= 1

    ptr: ^Node = ^node
    ptr.value = 5
    ptr.value += 3
    ptr^.value = 7
    ptr^.value += 4
    ptr^.value %= 6

    return node.value + ptr^.value - 10
}
¬
0
¬
¬
delete
¬
--llvm-backend
