Node :: plex {
    value i32
    next  ^Node
}

direct: [2]Node = [
    { value: 1, next: ^direct[1] },
    { value: 2, ... },
]

aliased: [2]Node = [
    { value: 3, next: second },
    { value: 4, next: first },
]

first  :: ^aliased[0]
second :: ^aliased[1]

main :: fn () -> i32 {
    return direct[0].next.value + aliased[0].next.value + aliased[1].next.value
}
¬
9
¬
¬
¬
