use std.io
pub use test.folder_pub_use

main :: fn () -> i32 {
    text := $"module says {"ok"}"
    prn(text)
    on "same" != "same" => return 1
    return child_answer() + local_answer() - 42
}
¬
0
¬
module says ok

¬
clean-llvm
¬
