Point :: plex { x i32 y i32 name string }
main :: fn () => on Point { x: 3, y: 4, name: "origin" } {
{ name: "origin", x } => x
{ x: 3, y } => y * 10
{ name } => 0
else => 1
}
¬
Point :: plex {
    x    i32
    y    i32
    name string
}

main :: fn () =>
    on Point { x: 3, y: 4, name: "origin" } {
        { name: "origin", x } => x
        { x: 3, y } => y * 10
        { name } => 0
        else => 1
    }
