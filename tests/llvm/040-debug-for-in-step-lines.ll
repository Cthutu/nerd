-- test-llvm-debug: yes
main :: fn () -> i32 {
    values := [1, 2]
    total := 0
    for value in values {
        total += value^
    }
    return total
}
¬
  %t8 = load i32, ptr %local.1, !dbg !21
  %t9 = load ptr, ptr %local.2, !dbg !21
  %t10 = load i32, ptr %t9, !dbg !21
  %t11 = add i32 %t8, %t10, !dbg !21
  store i32 %t11, ptr %local.1, !dbg !21
  %t12 = load i64, ptr %t4, !dbg !17
  %t13 = add i64 %t12, 1, !dbg !17
  store i64 %t13, ptr %t4, !dbg !17
  br label %for.in.cond.0, !dbg !17
for.in.end.2:
  %t14 = load i32, ptr %local.1, !dbg !22
!17 = !DILocation(line: 5, column: 1, scope: !6)
!20 = distinct !DILexicalBlock(scope: !6, file: !1, line: 6, column: 1)
!21 = !DILocation(line: 6, column: 1, scope: !20)
!22 = !DILocation(line: 8, column: 1, scope: !6)
