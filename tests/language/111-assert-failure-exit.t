-- A failing assertion prints its message and exits with status 127.
main :: fn () {
    assert no, "stopped"
}
¬
127
¬

¬
hir 0
bind main = fn.0
func fn.0() -> void {
  assert bool no, string "stopped"
}
¬
; nerd llvm-ir 0
; generated from HIR

@.str.m0.0 = private unnamed_addr constant [8 x i8] c"stopped\00"
@.assert.source_path.m0.0 = private unnamed_addr constant [63 x i8] c"tests/language/111-assert-failure-exit.t\00"

declare void @nerd_assert(i1, ptr, i32, { ptr, i64 })

define void @fn.0() {
  call void @nerd_assert(i1 0, ptr @.assert.source_path.m0.0, i32 3, { ptr, i64 } { ptr @.str.m0.0, i64 7 })
  ret void
}

@$main = alias void (), ptr @fn.0
