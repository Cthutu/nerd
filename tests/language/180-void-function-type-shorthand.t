-- Function types without an explicit return type default to void.
Sink :: fn (value: i32)

sink :: fn (_value: i32) {
}

main :: fn () {
    f: Sink = nil
    f = sink
    f(42)
}
¬
0
¬

¬
hir 0
bind Sink = type.0
bind sink = fn.0
bind main = fn.1
type type.0 = fn (i32) -> void
func fn.0(_value: i32) -> void {
}
func fn.1() -> void {
  let f: fn (i32) -> void = fn (i32) -> void nil
  assign fn (i32) -> void local.1(f) = fn (i32) -> void bind.1(sink)
  expr void call local.1(f)(i32 42)
}
¬
; nerd llvm-ir 0
; generated from HIR

define internal void @fn.0(i32 %_value) {
  ret void
}

define internal void @fn.1() {
  %local.1 = alloca ptr
  store ptr null, ptr %local.1
  store ptr @fn.0, ptr %local.1
  %t0 = load ptr, ptr %local.1
  call void %t0(i32 42)
  ret void
}

@$sink = internal alias void (i32), ptr @fn.0
@$main = alias void (), ptr @fn.1
