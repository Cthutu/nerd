main :: fn () {
    names := ["Alice", "Bob", "Charlie"]
    for name in names {
        prn(name^)
    }
}
¬
0
¬
Alice
Bob
Charlie

¬
hir 0
module module.183-for-in-fixed-array.input(183-for-in-fixed-array.input)
import import.0 prn from module.core(core).decl.N: <unknown>
import import.1 prn_text from module.core(core).decl.N: fn (string) -> void
import import.2 prn_empty from module.core(core).decl.N: fn () -> void
bind prn = import.0
bind prn_text = import.1
bind prn_empty = import.2
bind main = fn.0
func fn.0() -> void {
  let names: [3]string = [3]string array(string "Alice", string "Bob", string "Charlie")
  expr void for in name: ^string in [3]string local.0(names) {
    body {
      expr void call bind.1(prn_text)(string deref(^string local.1(name)))
    }
  }
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [40 x i8] c"tests/language/183-for-in-fixed-array.t\00"
@.str.m0.0 = private unnamed_addr constant [6 x i8] c"Alice\00"
@.str.m0.1 = private unnamed_addr constant [4 x i8] c"Bob\00"
@.str.m0.2 = private unnamed_addr constant [8 x i8] c"Charlie\00"
@.slice.const.m0.3 = private unnamed_addr constant [3 x { ptr, i64 }] [{ ptr, i64 } { ptr @.str.m0.0, i64 5 }, { ptr, i64 } { ptr @.str.m0.1, i64 3 }, { ptr, i64 } { ptr @.str.m0.2, i64 7 }]

declare void @$prn_text({ ptr, i64 })
declare void @$prn_empty()

define internal void @fn.0() {
  %local.0 = alloca [3 x { ptr, i64 }]
  %local.1 = alloca ptr
  %t0 = insertvalue [3 x { ptr, i64 }] poison, { ptr, i64 } { ptr @.str.m0.0, i64 5 }, 0
  %t1 = insertvalue [3 x { ptr, i64 }] %t0, { ptr, i64 } { ptr @.str.m0.1, i64 3 }, 1
  %t2 = insertvalue [3 x { ptr, i64 }] %t1, { ptr, i64 } { ptr @.str.m0.2, i64 7 }, 2
  store [3 x { ptr, i64 }] %t2, ptr %local.0
  %t3 = getelementptr inbounds [3 x { ptr, i64 }], ptr %local.0, i64 0, i64 0
  %t4 = add i64 0, 3
  %t5 = alloca i64
  store i64 0, ptr %t5
  br label %for.in.cond.0
for.in.cond.0:
  %t6 = load i64, ptr %t5
  %t7 = icmp ult i64 %t6, %t4
  br i1 %t7, label %for.in.body.1, label %for.in.end.3
for.in.body.1:
  %t8 = getelementptr inbounds { ptr, i64 }, ptr %t3, i64 %t6
  store ptr %t8, ptr %local.1
  %t9 = load ptr, ptr %local.1
  %t10 = load { ptr, i64 }, ptr %t9
  call void @$prn_text({ ptr, i64 } %t10)
  br label %for.in.update.2
for.in.update.2:
  %t11 = load i64, ptr %t5
  %t12 = add i64 %t11, 1
  store i64 %t12, ptr %t5
  br label %for.in.cond.0
for.in.end.3:
  ret void
}

@$main = hidden alias void (), ptr @fn.0

declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)