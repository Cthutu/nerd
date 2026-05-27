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
module module.0(183-for-in-fixed-array.input)
import import.0 prn from module.1(core).decl.13: fn (string) -> void
bind prn = import.0
bind main = fn.0
func fn.0() -> void {
  let names: [3]string = [3]string array(string "Alice", string "Bob", string "Charlie")
  expr void for in name: ^string in [3]string local.0(names) {
    body {
      expr void call bind.0(prn)(string deref(^string local.1(name)))
    }
  }
}
¬
%local.0 = alloca [3 x { ptr, i64 }]
%local.1 = alloca ptr
store [3 x { ptr, i64 }] %t2, ptr %local.0
%t3 = getelementptr inbounds [3 x { ptr, i64 }], ptr %local.0, i64 0, i64 0
%t4 = add i64 0, 3
br label %for.in.cond.0
for.in.cond.0:
%t7 = icmp ult i64 %t6, %t4
br i1 %t7, label %for.in.body.1, label %for.in.end.2
for.in.body.1:
%t8 = getelementptr inbounds { ptr, i64 }, ptr %t3, i64 %t6
store ptr %t8, ptr %local.1
%t10 = load { ptr, i64 }, ptr %t9
call void @$prn({ ptr, i64 } %t10)
br label %for.in.cond.0
for.in.end.2:
