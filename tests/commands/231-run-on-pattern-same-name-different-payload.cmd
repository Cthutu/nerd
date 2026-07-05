Payload :: plex {
    code i32
}

Thing :: enum {
    Number(u16)
    Record(Payload)
}

main :: fn () -> i32 {
    value : Thing = Thing.Number(0x1234)
    on value {
        Thing.Number(value) => {
            on value == 0x1234 => return 0
            return 1
        }
        Thing.Record(value) => {
            _ := value
            return 2
        }
    }
}
¬
0
¬

¬
delete
