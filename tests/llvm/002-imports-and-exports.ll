pub use test.folder_pub_use

pub local :: fn() -> i32 {
    return child_answer()
}

main :: fn() -> i32 {
    return local()
}
¬
declare i32 @$local_answer()
declare i32 @$child_answer()
define i32 @$local() {
; export local
; export local_answer
; export child_answer
