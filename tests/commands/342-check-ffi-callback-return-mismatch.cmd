Callback :: fn (context: ^void) -> u32
ffi "callback-test" { invoke (callback: Callback) }
wrong :: fn (context: ^void) -> u64 { return 0 }
main :: fn () { invoke(wrong) }
¬
1
¬

¬
delete
¬

¬
check
