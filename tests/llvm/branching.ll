use std.io

-- Block-form `on` materialises untyped local integer constants to i32.
main :: fn () {
    i :: 2

    prn(on i {
        0 => "zero"
        else as x => $"non-zero: {x}"
    })
}
¬
define internal void @fn.0() {
  %t3 = alloca { ptr, i64 }
  %t0 = icmp eq i32 2, 0
  br i1 %t0, label %on.body.1, label %on.next.2
on.body.1:
  br label %on.value.3
on.value.3:
  br label %on.end.0
on.next.2:
  br label %on.body.4
on.body.4:
  %t1 = call i64 @nrt_string_builder_mark()
  %t2 = alloca { ptr, i64 }
  store { ptr, i64 } { ptr @.str.m0.1, i64 10 }, ptr %t3
  call void @nrt_to_string_string(ptr %t2, ptr %t3)
  call void @nrt_string_builder_append_string(ptr %t2)
  %t4 = alloca { ptr, i64 }
  call void @nrt_to_string_i32(ptr %t4, i32 2)
  call void @nrt_string_builder_append_string(ptr %t4)
  %t5 = alloca { ptr, i64 }
  call void @nrt_string_builder_finish(ptr %t5, i64 %t1)
  %t6 = load { ptr, i64 }, ptr %t5
  br label %on.value.6
on.value.6:
  br label %on.end.0
on.end.0:
  %t7 = phi { ptr, i64 } [{ ptr @.str.m0.0, i64 4 }, %on.value.3], [%t6, %on.value.6]
  call void @$prn_text({ ptr, i64 } %t7)
  ret void
}
