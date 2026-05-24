-- test-llvm-debug: yes
main :: fn () -> i32 {
    value := 1
    {
        value := 2
        on value == 0 => return value
    }
    return value
}
¬
!10 = !DILocalVariable(name: "value", scope: !6, file: !1, line: 1, type: !9)
!12 = distinct !DILexicalBlock(scope: !6, file: !1, line: 5, column: 1)
!13 = !DILocation(line: 5, column: 1, scope: !12)
!14 = !DILocalVariable(name: "value", scope: !12, file: !1, line: 1, type: !9)
