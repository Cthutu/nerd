-- test-llvm-debug: yes
main :: fn () {
    for i in [0..1] {
        _ := i
    }
}
¬
  %t4 = load i32, ptr %local.0, !dbg !12
  call void asm sideeffect "nop", ""(), !dbg !13
  br label %for.range.update.2, !dbg !8
for.range.end.3:
  call void asm sideeffect "nop", ""(), !dbg !14
  ret void, !dbg !14
!12 = !DILocation(line: 4, column: 1, scope: !11)
!13 = !DILocation(line: 5, column: 1, scope: !11)
!14 = !DILocation(line: 6, column: 1, scope: !6)
