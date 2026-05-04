use std.io

Direction :: enum {
    NORTH,
    EAST,
    SOUTH = 10,
    WEST,
    COUNT
}

Token :: enum {
    EOF = 0,
    IDENT,
    NUMBER = 10,
    STRING,
}

labels : [Direction.COUNT]string = [
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11"
]

describe_direction :: fn (direction: Direction) -> string {
    return on direction {
        NORTH => "north"
        EAST => "east"
        SOUTH => "south"
        WEST => "west"
        COUNT => "count"
    }
}

describe_token :: fn (token: Token) -> string {
    return on token {
        EOF => "eof"
        IDENT => "ident"
        NUMBER => "number"
        STRING => "string"
    }
}

main :: fn () -> i32 {
    south : Direction = Direction.SOUTH
    string_token : Token = Token.STRING

    prn(labels[0])
    prn(labels[11])
    prn(describe_direction(south))
    prn(describe_token(string_token))

    ok := on south {
        SOUTH => yes
        else => no
    } && on string_token {
        STRING => yes
        else => no
    }

    return on ok => 0 else 1
}
¬0¬0
11
south
string

¬¬
