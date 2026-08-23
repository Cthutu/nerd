TokenType :: enum {
    Identifier
}

impl Display for TokenType {
    show :: fn (self: TokenType) -> string {
        return on self {
            TokenType.Identifier => "Identifier"
        }
    }
}

Token :: plex {
    u32 {
        type TokenType : 8
        offset : 24
    }
}

main :: fn () -> i32 {
    token := Token {
        type : TokenType.Identifier
        offset : 0
    }
    text := $"{token.type}"
    on text != "Identifier" => return 1
    return 0
}
¬
0
¬

¬
delete
¬
