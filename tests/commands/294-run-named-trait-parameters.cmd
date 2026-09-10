Callback :: fn (i32) -> i32
Render :: trait {
    render :: fn (self: ^Self, callback: fn (i32) -> i32) -> i32
}
Thing :: plex { value i32 }
impl Render for Thing {
    render :: fn (_self: ^Self, callback: Callback) -> i32 {
        return callback(7)
    }
}
impl Thing {
    idle :: fn (_self: ^Self) {}
    read_value :: fn (self: ^Self) -> i32 { return self.value }
}
identity :: fn (value: i32) -> i32 { return value }
main :: fn () {
    thing := Thing { value: 3 }
    thing.idle()
    assert thing.render(identity) == 7
    assert thing.read_value() == 3
}
¬
0
¬

¬
delete
