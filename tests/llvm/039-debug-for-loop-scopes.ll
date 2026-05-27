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
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [45 x i8] c"tests/llvm/039-debug-for-loop-scopes.input.n\00"

define internal i32 @fn.0() !dbg !6 {
  %local.0 = alloca i32
  call void @llvm.dbg.declare(metadata ptr %local.0, metadata !10, metadata !5), !dbg !11
  %local.1 = alloca i32
  call void @llvm.dbg.declare(metadata ptr %local.1, metadata !15, metadata !5), !dbg !16
  %local.2 = alloca i32
  call void @llvm.dbg.declare(metadata ptr %local.2, metadata !23, metadata !5), !dbg !24
  store i32 0, ptr %local.0, !dbg !8
  store i32 0, ptr %local.1, !dbg !14
  br label %for.cond.0, !dbg !14
for.cond.0:
  %t0 = load i32, ptr %local.1, !dbg !14
  %t1 = icmp slt i32 %t0, 2, !dbg !14
  br i1 %t1, label %for.body.1, label %for.end.3, !dbg !14
for.body.1:
  %t2 = load i32, ptr %local.0, !dbg !18
  %t3 = load i32, ptr %local.1, !dbg !18
  %t4 = add i32 %t2, %t3, !dbg !18
  store i32 %t4, ptr %local.0, !dbg !18
  call void asm sideeffect "nop", ""(), !dbg !19
  br label %for.update.2, !dbg !19
for.update.2:
  %t5 = load i32, ptr %local.1, !dbg !14
  %t6 = add i32 %t5, 1, !dbg !14
  store i32 %t6, ptr %local.1, !dbg !14
  br label %for.cond.0, !dbg !14
for.end.3:
  store i32 0, ptr %local.2, !dbg !22
  br label %for.cond.4, !dbg !22
for.cond.4:
  %t7 = load i32, ptr %local.2, !dbg !22
  %t8 = icmp slt i32 %t7, 3, !dbg !22
  br i1 %t8, label %for.body.5, label %for.end.7, !dbg !22
for.body.5:
  %t9 = load i32, ptr %local.0, !dbg !26
  %t10 = load i32, ptr %local.2, !dbg !26
  %t11 = add i32 %t9, %t10, !dbg !26
  store i32 %t11, ptr %local.0, !dbg !26
  call void asm sideeffect "nop", ""(), !dbg !27
  br label %for.update.6, !dbg !27
for.update.6:
  %t12 = load i32, ptr %local.2, !dbg !22
  %t13 = add i32 %t12, 1, !dbg !22
  store i32 %t13, ptr %local.2, !dbg !22
  br label %for.cond.4, !dbg !22
for.end.7:
  %t14 = load i32, ptr %local.0, !dbg !28
  ret i32 %t14, !dbg !28
}

@$main = alias i32 (), ptr @fn.0

declare void @llvm.dbg.declare(metadata, metadata, metadata)

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "Nerd", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "039-debug-for-loop-scopes.input.n", directory: "__REPO__/tests/llvm")
!2 = !{}
!5 = !DIExpression()
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 2, !"Dwarf Version", i32 5}
!7 = !DISubroutineType(types: !2)
!6 = distinct !DISubprogram(name: "main", linkageName: "main", scope: !1, file: !1, line: 3, type: !7, scopeLine: 3, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!8 = !DILocation(line: 3, column: 1, scope: !6)
!9 = !DIBasicType(name: "i32", size: 32, encoding: DW_ATE_signed)
!10 = !DILocalVariable(name: "total", scope: !6, file: !1, line: 1, type: !9)
!11 = !DILocation(line: 1, column: 1, scope: !6)
!12 = !DILocation(line: 4, column: 1, scope: !6)
!13 = distinct !DILexicalBlock(scope: !6, file: !1, line: 1, column: 1)
!14 = !DILocation(line: 4, column: 1, scope: !13)
!15 = !DILocalVariable(name: "i", scope: !13, file: !1, line: 1, type: !9)
!16 = !DILocation(line: 1, column: 1, scope: !13)
!17 = distinct !DILexicalBlock(scope: !13, file: !1, line: 5, column: 1)
!18 = !DILocation(line: 5, column: 1, scope: !17)
!19 = !DILocation(line: 6, column: 1, scope: !17)
!20 = !DILocation(line: 7, column: 1, scope: !6)
!21 = distinct !DILexicalBlock(scope: !6, file: !1, line: 1, column: 1)
!22 = !DILocation(line: 7, column: 1, scope: !21)
!23 = !DILocalVariable(name: "i", scope: !21, file: !1, line: 1, type: !9)
!24 = !DILocation(line: 1, column: 1, scope: !21)
!25 = distinct !DILexicalBlock(scope: !21, file: !1, line: 8, column: 1)
!26 = !DILocation(line: 8, column: 1, scope: !25)
!27 = !DILocation(line: 9, column: 1, scope: !25)
!28 = !DILocation(line: 10, column: 1, scope: !6)
