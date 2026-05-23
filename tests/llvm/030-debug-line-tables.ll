-- test-llvm-debug: yes
main :: fn () -> i32 {
    base := 40
    bonus := 2
    total := base + bonus
    return total
}
¬
define internal i32 @fn.0() !dbg !5 {
  %t0 = add i32 40, 2, !dbg !9
  ret i32 %t0, !dbg !10
}

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4}
!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "Nerd", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "030-debug-line-tables.input.n", directory: "__REPO__/tests/llvm")
!5 = distinct !DISubprogram(name: "main", linkageName: "@fn.0", scope: !1, file: !1, line: 3, type: !6, scopeLine: 3, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!9 = !DILocation(line: 5, column: 1, scope: !5)
!10 = !DILocation(line: 6, column: 1, scope: !5)
