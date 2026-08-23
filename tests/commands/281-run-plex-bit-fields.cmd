VERSION_BITS :: 4
Octet        :: u8

Header :: plex {
    Octet {
        version : VERSION_BITS
        kind TokenType : 4
    }

    u16 {
        status TokenType : 3
        active           : 1
        code             : 4
        _                : 8
    }

    length u16
}

TokenType :: enum {
    Eof
    Integer
    Float
    String
    Symbol
}

main :: fn () -> i32 {
    header := Header {
        version : 2
        kind    : TokenType.String
        status  : TokenType.Integer
        active  : 1
        code    : 10
        length  : 513
    }

    on header.version != 2 => return 1
    on header.kind != TokenType.String => return 2
    on header.status != TokenType.Integer => return 23
    on header.active != 1 => return 3
    on header.code != 10 => return 4
    on header.length != 513 => return 5
    on Header.size != 6 => return 6

    header.version = 15
    header.kind = TokenType.Symbol
    on header.version != 15 => return 7
    on header.kind != TokenType.Symbol => return 8
    on header.active != 1 => return 9

    pointer := ^header
    pointer.code = 3
    on pointer.code != 3 => return 10
    on pointer.active != 1 => return 11
    pointer.status = TokenType.Symbol
    on header.status != TokenType.Symbol => return 24

    header.kind = TokenType.Float
    on header.kind != TokenType.Float => return 12
    on header.version != 15 => return 13

    updated := header with { version: 6 }
    on updated.version != 6 => return 14
    on updated.kind != TokenType.Float => return 15
    on updated.code != 3 => return 16
    on updated.length != 513 => return 17

    defaulted := Header { length: 7 ... }
    on defaulted.version != 0 => return 18
    on defaulted.kind != TokenType.Eof => return 19
    on defaulted.status != TokenType.Eof => return 25
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
