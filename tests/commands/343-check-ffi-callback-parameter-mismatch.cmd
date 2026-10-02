Callback :: fn (context: ^void) -> u32
ffi "callback-test" { invoke (callback: Callback) }
wrong :: fn (context: i64) -> u32 { return 0 }
main :: fn () { invoke(wrong) }
¬
1
¬

¬
delete
¬

¬
check
