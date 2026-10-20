=== case: 102-io-input-lines ===
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
module module.library-apis.input(library-apis.input)
import module.std.io(std.io)
import import.0 prn from module.core(core).decl.N: <unknown>
import import.1 input from module.std.io(std.io).decl.N: fn (string) -> [..]u8
import import.2 prn_text from module.core(core).decl.N: fn (string) -> void
import import.3 prn_empty from module.core(core).decl.N: fn () -> void
bind prn = import.0
bind input = import.1
bind prn_text = import.2
bind prn_empty = import.3
bind main = fn.0
func fn.0() -> void {
  let line: [..]u8 = [..]u8 call bind.1(input)(string "")
  defer {
    expr void call fn () -> void field([..]u8 local.0(line), free)()
  }
  expr void call bind.2(prn_text)(string cast([..]u8 local.0(line) as string))
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [36 x i8] c"tests/language/library-apis.t\00"
@.str.m0.0 = private unnamed_addr constant [1 x i8] c"\00"

declare ptr @nrt_mem_alloc(i64, i64, ptr, i32)
declare ptr @nrt_mem_realloc(ptr, i64, i64, ptr, i32)
declare void @nrt_mem_free(ptr)
declare i64 @nrt_mem_size(ptr)

declare ptr @$input({ ptr, i64 })
declare void @$prn_text({ ptr, i64 })
declare void @$prn_empty()

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
  %t4 = getelementptr inbounds i8, ptr %t1, i64 -32
  %t5 = getelementptr inbounds { ptr, i64, i64 }, ptr %t4, i64 0, i32 0
  %t6 = load ptr, ptr %t5
  %t7 = getelementptr inbounds i8, ptr %t1, i64 -32
  %t8 = getelementptr inbounds { ptr, i64, i64 }, ptr %t7, i64 0, i32 1
  %t9 = load i64, ptr %t8
  %t10 = insertvalue { ptr, i64 } poison, ptr %t6, 0
  %t11 = insertvalue { ptr, i64 } %t10, i64 %t9, 1
  store { ptr, i64 } %t11, ptr %t2
  br label %dynarray.string.done.2
dynarray.string.done.2:
  %t12 = load { ptr, i64 }, ptr %t2
  call void @$prn_text({ ptr, i64 } %t12)
  %t13 = load ptr, ptr %local.0
  %t14 = icmp eq ptr %t13, null
  br i1 %t14, label %dynarray.free.done.4, label %dynarray.free.3
dynarray.free.3:
  %t15 = getelementptr inbounds i8, ptr %t13, i64 -32
  call void @nrt_mem_free(ptr %t15)
  store ptr null, ptr %local.0
  br label %dynarray.free.done.4
dynarray.free.done.4:
  ret void
}

@$main = hidden alias void (), ptr @fn.0

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)
¬
hello world

=== case: 133-std-io-error-output ===
use std.io

main :: fn () {
    epr("error")
    eprn(" line")
    prn("done")
}
¬
0
¬
done

¬
hir 0
module module.library-apis.input(library-apis.input)
import module.std.io(std.io)
import import.0 epr from module.core(core).decl.N: fn (string) -> void
import import.1 prn from module.core(core).decl.N: <unknown>
import import.2 eprn from module.core(core).decl.N: <unknown>
import import.3 input from module.std.io(std.io).decl.N: fn (string) -> [..]u8
import import.4 eprn_text from module.core(core).decl.N: fn (string) -> void
import import.5 eprn_empty from module.core(core).decl.N: fn () -> void
import import.6 prn_text from module.core(core).decl.N: fn (string) -> void
import import.7 prn_empty from module.core(core).decl.N: fn () -> void
bind epr = import.0
bind prn = import.1
bind eprn = import.2
bind input = import.3
bind eprn_text = import.4
bind eprn_empty = import.5
bind prn_text = import.6
bind prn_empty = import.7
bind main = fn.0
func fn.0() -> void {
  expr void call bind.0(epr)(string "error")
  expr void call bind.4(eprn_text)(string " line")
  expr void call bind.6(prn_text)(string "done")
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [41 x i8] c"tests/language/library-apis.t\00"
@.str.m0.0 = private unnamed_addr constant [6 x i8] c"error\00"
@.str.m0.1 = private unnamed_addr constant [6 x i8] c" line\00"
@.str.m0.2 = private unnamed_addr constant [5 x i8] c"done\00"

declare void @$epr({ ptr, i64 })
declare ptr @$input({ ptr, i64 })
declare void @$eprn_text({ ptr, i64 })
declare void @$eprn_empty()
declare void @$prn_text({ ptr, i64 })
declare void @$prn_empty()

define internal void @fn.0() {
  call void @$epr({ ptr, i64 } { ptr @.str.m0.0, i64 5 })
  call void @$eprn_text({ ptr, i64 } { ptr @.str.m0.1, i64 5 })
  call void @$prn_text({ ptr, i64 } { ptr @.str.m0.2, i64 4 })
  ret void
}

@$main = hidden alias void (), ptr @fn.0

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)

=== case: 097-run-llvm-std-random ===
use std.memory

main :: fn () -> i32 {
    bytes := alloc(8)
    defer free(bytes)
    on alloc_size(bytes) != 8 => return 1
    return 0
}
¬
0
¬

¬

¬
