VERSION_BITS :: 4
Octet        :: u8

Header :: plex {
    Octet {
        version : VERSION_BITS
        kind    : 4
    }

    u16 {
        _      : 3
        active : 1
        code   : 4
        _      : 8
    }

    length u16
}

main :: fn () -> i32 {
    header := Header {
        version : 2
        kind    : 9
        active  : 1
        code    : 10
        length  : 513
    }

    on header.version != 2 => return 1
    on header.kind != 9 => return 2
    on header.active != 1 => return 3
    on header.code != 10 => return 4
    on header.length != 513 => return 5
    on Header.size != 6 => return 6

    header.version = 15
    too_large : u8 = 31
    header.kind    = too_large
    on header.version != 15 => return 7
    on header.kind != 15 => return 8
    on header.active != 1 => return 9

    pointer := ^header
    pointer.code = 3
    on pointer.code != 3 => return 10
    on pointer.active != 1 => return 11

    increment : u8 = 4
    header.kind += increment
    on header.kind != 3 => return 12
    on header.version != 15 => return 13

    updated := header with { version: 6 }
    on updated.version != 6 => return 14
    on updated.kind != 3 => return 15
    on updated.code != 3 => return 16
    on updated.length != 513 => return 17

    defaulted := Header { length: 7 ... }
    on defaulted.version != 0 => return 18
    on defaulted.kind != 0 => return 19
    on defaulted.active != 0 => return 20
    on defaulted.code != 0 => return 21
    on defaulted.length != 7 => return 22
    return 0
}
¬
0
¬

¬
default-main
¬
