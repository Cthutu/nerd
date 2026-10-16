-- test-llvm-debug: yes
Compact :: plex { first u8 middle u32 last u8 }
COrder :: plex #c { first u8 middle u32 last u8 }
Embedded :: plex { prefix u64 use Compact suffix u8 }
main :: fn () {
    compact: Compact
    c: COrder
    embedded: Embedded
    compact.middle = 1
    c.middle = 2
    embedded.middle = 3
    assert compact.middle + c.middle + embedded.middle == 6
}
¬
!9 = !DICompositeType(tag: DW_TAG_structure_type, name: "Compact", file: !1, size: 64, elements: !10)
!11 = !DIDerivedType(tag: DW_TAG_member, name: "first", scope: !9, file: !1, baseType: !14, size: 8, offset: 32)
!12 = !DIDerivedType(tag: DW_TAG_member, name: "middle", scope: !9, file: !1, baseType: !15, size: 32, offset: 0)
!13 = !DIDerivedType(tag: DW_TAG_member, name: "last", scope: !9, file: !1, baseType: !14, size: 8, offset: 40)
!19 = !DICompositeType(tag: DW_TAG_structure_type, name: "COrder", file: !1, size: 96, elements: !20)
!21 = !DIDerivedType(tag: DW_TAG_member, name: "first", scope: !19, file: !1, baseType: !14, size: 8, offset: 0)
!22 = !DIDerivedType(tag: DW_TAG_member, name: "middle", scope: !19, file: !1, baseType: !15, size: 32, offset: 32)
!23 = !DIDerivedType(tag: DW_TAG_member, name: "last", scope: !19, file: !1, baseType: !14, size: 8, offset: 64)
!26 = !DICompositeType(tag: DW_TAG_structure_type, name: "Embedded", file: !1, size: 192, elements: !27)
!28 = !DIDerivedType(tag: DW_TAG_member, name: "prefix", scope: !26, file: !1, baseType: !33, size: 64, offset: 0)
!29 = !DIDerivedType(tag: DW_TAG_member, name: "first", scope: !26, file: !1, baseType: !14, size: 8, offset: 96)
!30 = !DIDerivedType(tag: DW_TAG_member, name: "middle", scope: !26, file: !1, baseType: !15, size: 32, offset: 64)
!31 = !DIDerivedType(tag: DW_TAG_member, name: "last", scope: !26, file: !1, baseType: !14, size: 8, offset: 104)
!32 = !DIDerivedType(tag: DW_TAG_member, name: "suffix", scope: !26, file: !1, baseType: !14, size: 8, offset: 128)
