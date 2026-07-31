Point :: plex {
    x i32
    y i32
}

Rect :: plex {
    colour u8
    use Point
    width  i32
    height i32
}

Pair :: plex [T] {
    first  T
    second T
}

IntPair :: plex {
    use Pair[i32]
    label u8
}

Shape :: plex {
    tag u8
    use Rect
}

Aligned :: plex {
    small u8
    wide  u64
}

Container :: plex {
    prefix u8
    use Aligned
    suffix u8
}

initial_container :: Container { prefix: 1 small: 2 wide: 3 suffix: 4 }

move_point :: fn (point: ^Point) {
    point.x += 1
    point.y += 2
}

set_wide :: fn (aligned: ^Aligned) {
    aligned.wide = 10
}

read_wide :: fn (container: Container) -> i32 {
    return on container {
        { wide: wide } => wide.as(i32)
        else => 0
    }
}

main :: fn () -> i32 {
    rect := Rect {
        colour: 1
        x     : 10
        y     : 20
        width : 30
        height: 40
    }
    pair := IntPair { first: 2 second: 3 label: 4 }
    shape := Shape {
        tag   : 0
        colour: 1
        x     : 10
        y     : 20
        width : 30
        height: 40
    }
    move_point((^shape).as(^Point))
    container := Container { prefix: 1 small: 2 wide: 3 suffix: 4 }
    set_wide((^container).as(^Aligned))
    updated := container with { wide: 11 }
    return rect.x + rect.y + rect.width + rect.height + rect.colour.as(i32) +
           pair.first + pair.second + pair.label.as(i32) + shape.x + shape.y +
           container.wide.as(i32) + read_wide(updated) +
           initial_container.suffix.as(i32)
}
¬
168
¬

¬

¬
