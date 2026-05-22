left :: use test.ffi_name_collision
right :: use test.ffi_name_collision_peer

main :: fn () {
    _ := left.local_os_value()
    _ := right.peer_os_value()
}
¬
0
¬

¬
keep
¬

¬
build
