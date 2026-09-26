Info :: plex #c { value i32 }
Chain :: plex #c { pNext ^void }
Opaque :: ^void

read :: fn (ptr: ^void) -> i32 { return ptr.as(^Info).value }
erase :: fn (ptr: ^Info) -> ^void { return ptr }
ffi "c" { memcmp(left: ^void, right: ^void, count: usize) -> i32 }

main :: fn () -> i32 {
    info := Info { value: 7 }
    pointer := ^info
    erased: ^void = pointer
    on erased != pointer => return 1
    erased = ^Info { value: 11 }
    on read(erased) != 11 => return 2
    on read(^Info { value: 13 }) != 13 => return 3
    chain := Chain { pNext: ^Info { value: 17 } }
    on read(chain.pNext) != 17 => return 4
    alias: Opaque = ^Info { value: 19 }
    on read(alias) != 19 => return 5
    on erase(pointer) != pointer => return 6
    on read(^info) != 7 => return 7
    on memcmp(^Info { value: 7 }, ^info, Info.size) != 0 => return 8
    prn("implicit void pointers work")
    return 0
}
¬
0
¬
implicit void pointers work

¬
delete
¬
