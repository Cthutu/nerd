use std.io

Node :: plex {
    value i32
}

main :: fn () -> i32 {
    missing: ^Node = nil
    value := Node {
        value: 7
    }
    present := ^value

    on missing == nil || missing.value == 0 => {
        prn("or short")
    }
    on missing != nil && missing.value == 0 => {
        prn("bad and")
        return 1
    }
    on present != nil && present.value == 7 => {
        prn("and rhs")
    }
    return 0
}
¬
0
¬
or short
and rhs

¬
delete
¬
--llvm
