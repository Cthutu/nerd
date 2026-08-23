-- test-llvm-debug: yes
TokenType :: enum { Eof Request Response }

Header :: plex {
    u8 {
        version : 4
        kind TokenType : 4
    }
    length u16
}

main :: fn () {
    header: Header
    header = Header { version: 1 kind: TokenType.Response length: 3 }
    on header.kind == TokenType.Response => prn("ok")
}
¬
  %local.0 = alloca { i8, i16 }
!9 = !DICompositeType(tag: DW_TAG_structure_type, name: "Header", file: !1, size: 32, elements: !10)
!11 = !DIDerivedType(tag: DW_TAG_member, name: "version", scope: !9, file: !1, baseType: !14, size: 4, offset: 0)
!15 = !DICompositeType(tag: DW_TAG_structure_type, name: "TokenType", file: !1, size: 128, elements: !16)
!12 = !DIDerivedType(tag: DW_TAG_member, name: "kind", scope: !9, file: !1, baseType: !15, size: 4, offset: 4)
!13 = !DIDerivedType(tag: DW_TAG_member, name: "length", scope: !9, file: !1, baseType: !21, size: 16, offset: 16)
