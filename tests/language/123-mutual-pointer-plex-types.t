Left :: plex {
    value i32
    right ^Right
}

Right :: plex {
    value i32
    left  ^Left
}

left: Left = { value: 1, ... }
right: Right = { value: 2, left: ^left }

main :: fn () -> i32 {
    left.right = ^right
    return left.right.value + right.left.value
}
¬
3
¬
¬
¬
