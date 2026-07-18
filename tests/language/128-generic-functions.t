id :: fn [T] (value: T) => value

choose :: fn [T] (left: T, right: T, use_left: bool) => on use_left {
    yes => left
    else => right
}

Box :: plex [T] {
    value T
}

wrap :: fn [T] (value: T) -> Box[T] {
    box: Box[T]
    box.value = value
    return box
}

main :: fn () {
    number := id(7)
    text := id("ok")
    explicit := id[i32](number + 5)

    int_id := id[i32]
    from_value := int_id(explicit + 30)

    box := wrap[string](text)
    chosen := choose[string](box.value, "bad", yes)

    prn($"{number} {text} {explicit} {from_value} {chosen}")
}
¬
0
¬
7 ok 12 42 ok

¬
hir 0
bind Box = type.0
bind main = fn.0
generic type type.0 = <unknown>
func fn.0() -> void {
inst func fn.1(value: i32) -> i32 {
inst func fn.2(value: string) -> string {
¬
; nerd llvm-ir 0
define internal void @fn.0() {
define internal i32 @fn.1(i32 %value) {
define internal { ptr, i64 } @fn.2({ ptr, i64 } %value) {
@$main = alias void (), ptr @fn.0
