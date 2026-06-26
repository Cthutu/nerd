direct :: fn (value: Option[u8]) -> Option[u8] {
    byte := on value {
        Some(found) => found
        None => return None
    }
    return Some(byte)
}

block :: fn (value: Option[u8]) -> Option[u8] {
    byte := on value {
        Some(found) => found
        None => {
            return None
        }
    }
    return Some(byte)
}

main :: fn () -> i32 {
    direct_result := direct(None)
    block_result := block(None)

    on direct_result {
        Some(_) => return 1
        None => {}
    }

    on block_result {
        Some(_) => return 2
        None => {}
    }

    return 0
}
¬
0
¬

¬
delete
¬
