-- test-llvm-debug: yes
main :: fn () -> i32 {
    total := 0
    for i := 0; i < 2; i += 1 {
        total += i
    }
    for i := 0; i < 3; i += 1 {
        total += i
    }
    return total
}
¬
!13 = distinct !DILexicalBlock(scope: !6, file: !1, line: 1, column: 1)
!15 = !DILocalVariable(name: "i", scope: !13, file: !1, line: 1, type: !9)
!20 = distinct !DILexicalBlock(scope: !6, file: !1, line: 1, column: 1)
!22 = !DILocalVariable(name: "i", scope: !20, file: !1, line: 1, type: !9)
