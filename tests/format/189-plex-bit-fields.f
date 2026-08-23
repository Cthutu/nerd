Header :: plex {u8 {version:4
kind : 4}
u16 {
_ : 3
active:1 -- Active bit.
code:4
_:8}
length u16}
¬
Header :: plex {
    u8 {
        version : 4
        kind    : 4
    }
    u16 {
        _      : 3
        active : 1  -- Active bit.
        code   : 4
        _      : 8
    }
    length u16
}
