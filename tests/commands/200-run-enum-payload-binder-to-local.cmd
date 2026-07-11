Key :: enum { A B }

key_from_byte :: fn (byte: u8) -> ?Key {
    return on byte {
        'A'   => A
        else  => nil
    }
}

accept :: fn (key: Key) -> i32 {
    return key.as(i32)
}

main :: fn () -> i32 {
    key := key_from_byte('A')
    return on key {
        key_code => {
            code := key_code
            break accept(code) - Key.A.as(i32)
        }
        else => 1
    }
}
¬
0
¬

¬
keep
¬

¬
run
