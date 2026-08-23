TokenType :: enum {Eof Other}
Header :: plex {u8 {version:4
kind TokenType: 4}
u16 {
_ : 3
active:1 -- Active bit.
code:4
_:8}
length u16}
¬
TokenType :: enum {
    Eof
    Other
}

Header :: plex {
    u8 {
        version           : 4
        kind    TokenType : 4
    }
    u16 {
        _      : 3
        active : 1  -- Active bit.
        code   : 4
        _      : 8
    }
    length u16
}
