direct :: fn (value: ?u8) -> ?u8 {
    byte := on value {
        found => found
        else => return nil
    }
    return byte
}

block :: fn (value: ?u8) -> ?u8 {
    byte := on value {
        found => found
        else => {
            return nil
        }
    }
    return byte
}

main :: fn () -> i32 {
    direct_result := direct(nil)
    block_result := block(nil)

    on direct_result => {
        return 1
    } else {
    }

    on block_result => {
        return 2
    } else {
    }

    return 0
}
¬
0
¬

¬
delete
¬
