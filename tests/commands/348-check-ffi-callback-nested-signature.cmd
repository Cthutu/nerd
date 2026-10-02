Callback :: fn (context: ^void) -> u64
Factory :: fn (callback: Callback) -> Callback
ffi "callback-test" {
    get_callback () -> Callback
    get_factory () -> Factory
}
main :: fn () {
    callback := get_callback()
    factory := get_factory()
    result := factory(callback)
    _ := result(nil)
}
¬
0
¬

¬
delete
¬

¬
check
