use std.io

main :: fn () {
    line := input("")
    defer line.free()
    prn(line.as(string))
}
¬
0
¬
hello world

¬
hir 0
module module.0(102-io-input-lines.input)
import module.1(std.io)
import import.0 prn from module.3(core).decl.13: fn (string) -> void
import import.1 input from module.1(std.io).decl.7: fn (string) -> [..]u8
bind prn = import.0
bind input = import.1
bind main = fn.0
func fn.0() -> void {
  let line: [..]u8 = [..]u8 call bind.1(input)(string "")
  defer {
    expr void call fn () -> void field([..]u8 local.0(line), free)()
  }
  expr void call bind.0(prn)(string cast([..]u8 local.0(line) as string))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [36 x i8] c"tests/language/102-io-input-lines.t\00"
@.str.m0.0 = private unnamed_addr constant [1 x i8] c"\00"

declare ptr @nrt_mem_alloc(i64, i64, ptr, i32)
declare ptr @nrt_mem_realloc(ptr, i64, i64, ptr, i32)
declare void @nrt_mem_free(ptr)

declare void @$prn({ ptr, i64 })
declare ptr @$input({ ptr, i64 })

define internal void @fn.0() {
  %local.0 = alloca ptr
  %t0 = call ptr @$input({ ptr, i64 } { ptr @.str.m0.0, i64 0 })
  store ptr %t0, ptr %local.0
  %t1 = load ptr, ptr %local.0
  %t2 = alloca { ptr, i64 }
  %t3 = icmp eq ptr %t1, null
  br i1 %t3, label %dynarray.string.empty.0, label %dynarray.string.load.1
dynarray.string.empty.0:
  store { ptr, i64 } { ptr null, i64 0 }, ptr %t2
  br label %dynarray.string.done.2
dynarray.string.load.1:
  %t4 = getelementptr inbounds i8, ptr %t1, i64 -24
  %t5 = getelementptr inbounds { ptr, i64, i64 }, ptr %t4, i64 0, i32 0
  %t6 = load ptr, ptr %t5
  %t7 = getelementptr inbounds i8, ptr %t1, i64 -24
  %t8 = getelementptr inbounds { ptr, i64, i64 }, ptr %t7, i64 0, i32 1
  %t9 = load i64, ptr %t8
  %t10 = insertvalue { ptr, i64 } poison, ptr %t6, 0
  %t11 = insertvalue { ptr, i64 } %t10, i64 %t9, 1
  store { ptr, i64 } %t11, ptr %t2
  br label %dynarray.string.done.2
dynarray.string.done.2:
  %t12 = load { ptr, i64 }, ptr %t2
  call void @$prn({ ptr, i64 } %t12)
  %t13 = load ptr, ptr %local.0
  %t14 = icmp eq ptr %t13, null
  br i1 %t14, label %dynarray.free.done.4, label %dynarray.free.3
dynarray.free.3:
  %t15 = getelementptr inbounds i8, ptr %t13, i64 -24
  call void @nrt_mem_free(ptr %t15)
  store ptr null, ptr %local.0
  br label %dynarray.free.done.4
dynarray.free.done.4:
  ret void
}

@$main = alias void (), ptr @fn.0
¬
hello world
