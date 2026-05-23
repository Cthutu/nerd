-- test-llvm-debug: yes
main :: fn () {
    numbers: [..]i32
    on numbers == nil => {}
}
¬
; nerd llvm-ir 0
; generated from HIR

@.macro.file.m0 = private unnamed_addr constant [49 x i8] c"tests/llvm/035-debug-dynamic-array-types.input.n\00"

define internal void @fn.0() !dbg !6 {
  %local.0 = alloca ptr
  call void @llvm.dbg.declare(metadata ptr %local.0, metadata !11, metadata !5), !dbg !12
  store ptr null, ptr %local.0, !dbg !8
  %t0 = load ptr, ptr %local.0, !dbg !13
  %t1 = icmp eq ptr %t0, null, !dbg !13
  %t2 = icmp eq i1 %t1, 1, !dbg !13
  br i1 %t2, label %on.body.1, label %on.end.0, !dbg !13
on.body.1:
  br label %on.end.0, !dbg !13
on.end.0:
  ret void, !dbg !13
}

@$main = alias void (), ptr @fn.0

declare void @llvm.dbg.declare(metadata, metadata, metadata)

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "Nerd", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "035-debug-dynamic-array-types.input.n", directory: "__REPO__/tests/llvm")
!2 = !{}
!5 = !DIExpression()
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 2, !"Dwarf Version", i32 5}
!7 = !DISubroutineType(types: !2)
!6 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 3, type: !7, scopeLine: 3, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!8 = !DILocation(line: 3, column: 1, scope: !6)
!10 = !DIBasicType(name: "i32", size: 32, encoding: DW_ATE_signed)
!9 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "[..]i32", baseType: !10, size: 64)
!11 = !DILocalVariable(name: "numbers", scope: !6, file: !1, line: 1, type: !9)
!12 = !DILocation(line: 1, column: 1, scope: !6)
!13 = !DILocation(line: 4, column: 1, scope: !6)
