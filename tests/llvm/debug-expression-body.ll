-- test-llvm-debug: yes
answer :: fn () => 42
forward :: fn () => answer()
main :: fn () -> i32 {
    return forward() - 42
}
¬
define internal i32 @fn.0() !dbg !6 {
  ret i32 42, !dbg !8
}
define internal i32 @fn.1() !dbg !9 {
  %t0 = call i32 @fn.0(), !dbg !11
  ret i32 %t0, !dbg !11
}
!8 = !DILocation(line: 2, column: 1, scope: !6)
!11 = !DILocation(line: 3, column: 1, scope: !9)
