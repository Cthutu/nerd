-- test-llvm-debug: yes
result : i32

main :: fn () -> i32 {
    result = on yes {
        yes  => 1
        else => 2
    }
    return result
}
¬
define internal i32 @fn.0() !dbg !10 {
  %t0 = icmp eq i1 1, 1, !dbg !12
  br i1 %t0, label %on.body.1, label %on.next.2, !dbg !12
on.body.1:
  call void asm sideeffect "nop", ""(), !dbg !13
  br label %on.value.3, !dbg !13
on.value.3:
  br label %on.end.0, !dbg !13
on.next.2:
  br label %on.body.4, !dbg !13
on.body.4:
  call void asm sideeffect "nop", ""(), !dbg !14
  br label %on.value.6, !dbg !14
on.value.6:
  br label %on.end.0, !dbg !14
on.end.0:
  %t1 = phi i32 [1, %on.value.3], [2, %on.value.6], !dbg !14
  store i32 %t1, ptr @$result, !dbg !12
  %t2 = load i32, ptr @$result, !dbg !15
  ret i32 %t2, !dbg !15
}

!12 = !DILocation(line: 5, column: 1, scope: !10)
!13 = !DILocation(line: 6, column: 1, scope: !10)
!14 = !DILocation(line: 7, column: 1, scope: !10)
!15 = !DILocation(line: 9, column: 1, scope: !10)
