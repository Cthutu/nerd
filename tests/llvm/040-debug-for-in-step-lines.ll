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
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [47 x i8] c"tests/llvm/040-debug-for-in-step-lines.input.n\00"

define internal i32 @fn.0() !dbg !6 {
  %local.0 = alloca [2 x i32]
  call void @llvm.dbg.declare(metadata ptr %local.0, metadata !13, metadata !5), !dbg !14
  %local.1 = alloca i32
  call void @llvm.dbg.declare(metadata ptr %local.1, metadata !16, metadata !5), !dbg !14
  %local.2 = alloca ptr
  call void @llvm.dbg.declare(metadata ptr %local.2, metadata !19, metadata !5), !dbg !17
  %t0 = insertvalue [2 x i32] poison, i32 1, 0, !dbg !8
  %t1 = insertvalue [2 x i32] %t0, i32 2, 1, !dbg !8
  store [2 x i32] %t1, ptr %local.0, !dbg !8
  store i32 0, ptr %local.1, !dbg !15
  %t2 = getelementptr inbounds [2 x i32], ptr %local.0, i64 0, i64 0, !dbg !17
  %t3 = add i64 0, 2, !dbg !17
  %t4 = alloca i64, !dbg !17
  store i64 0, ptr %t4, !dbg !17
  br label %for.in.cond.0, !dbg !17
for.in.cond.0:
  %t5 = load i64, ptr %t4, !dbg !17
  %t6 = icmp ult i64 %t5, %t3, !dbg !17
  br i1 %t6, label %for.in.body.1, label %for.in.end.2, !dbg !17
for.in.body.1:
  %t7 = getelementptr inbounds i32, ptr %t2, i64 %t5, !dbg !17
  store ptr %t7, ptr %local.2, !dbg !17
  %t8 = load i32, ptr %local.1, !dbg !21
  %t9 = load ptr, ptr %local.2, !dbg !21
  %t10 = load i32, ptr %t9, !dbg !21
  %t11 = add i32 %t8, %t10, !dbg !21
  store i32 %t11, ptr %local.1, !dbg !21
  call void asm sideeffect "nop", ""(), !dbg !22
  %t12 = load i64, ptr %t4, !dbg !17
  %t13 = add i64 %t12, 1, !dbg !17
  store i64 %t13, ptr %t4, !dbg !17
  br label %for.in.cond.0, !dbg !17
for.in.end.2:
  %t14 = load i32, ptr %local.1, !dbg !23
  ret i32 %t14, !dbg !23
}

@$main = alias i32 (), ptr @fn.0

declare void @llvm.dbg.declare(metadata, metadata, metadata)

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "Nerd", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "040-debug-for-in-step-lines.input.n", directory: "__REPO__/tests/llvm")
!2 = !{}
!5 = !DIExpression()
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 2, !"Dwarf Version", i32 5}
!7 = !DISubroutineType(types: !2)
!6 = distinct !DISubprogram(name: "main", linkageName: "main", scope: !1, file: !1, line: 3, type: !7, scopeLine: 3, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!8 = !DILocation(line: 3, column: 1, scope: !6)
!10 = !DIBasicType(name: "i32", size: 32, encoding: DW_ATE_signed)
!9 = !DICompositeType(tag: DW_TAG_array_type, name: "[2]i32", file: !1, baseType: !10, size: 64, elements: !11)
!11 = !{!12}
!12 = !DISubrange(count: 2, lowerBound: 0)
!13 = !DILocalVariable(name: "values", scope: !6, file: !1, line: 1, type: !9)
!14 = !DILocation(line: 1, column: 1, scope: !6)
!15 = !DILocation(line: 4, column: 1, scope: !6)
!16 = !DILocalVariable(name: "total", scope: !6, file: !1, line: 1, type: !10)
!17 = !DILocation(line: 5, column: 1, scope: !6)
!18 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "ptr", baseType: !10, size: 64)
!19 = !DILocalVariable(name: "value", scope: !6, file: !1, line: 5, type: !18)
!20 = distinct !DILexicalBlock(scope: !6, file: !1, line: 6, column: 1)
!21 = !DILocation(line: 6, column: 1, scope: !20)
!22 = !DILocation(line: 7, column: 1, scope: !20)
!23 = !DILocation(line: 8, column: 1, scope: !6)
