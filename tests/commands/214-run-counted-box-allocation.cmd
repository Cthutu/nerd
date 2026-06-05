take :: fn (bytes: box[u8]) -> i32 {
    on bytes.count != 4 => return 1
    view := bytes.data.as([]u8, bytes.count)
    return view[0].as(i32) + view[1].as(i32) + view[2].as(i32) + view[3].as(i32)
}

main :: fn () -> i32 {
    bytes := box[u8](4)
    on bytes.count != 4 => return 2
    view := bytes.data.as([]u8, bytes.count)

    view[0] = 10
    view[1] = 11
    view[2] = 12
    view[3] = 9

    result := take(bytes)
    on bytes => return 3

    return result
}
¬
42
¬

¬
delete
¬
