Item :: plex {
    value i32
}

accept_bool :: fn (value: bool) -> i32 {
    return on value {
        yes => 1
        else => 0
    }
}

main :: fn () -> i32 {
    item: box[Item] = nil

    on item => return 1

    item = box[Item]()
    item.value = 7

    on !item => return 2

    ok: bool = item
    on !ok => return 3

    on item && ok => {
        on accept_bool(item) != 1 => return 4
    }
    else {
        return 5
    }

    assert item

    item.free()
    on item => return 6

    return 42
}
¬
42
¬

¬
delete
¬
