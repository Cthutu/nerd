impl string {
    pub lines :: fn (text: Self) => StringLinesIterator {
        text   : text
        offset : 0
    }
}

pub StringLinesIterator :: plex {
    text   string
    offset usize
}

impl Iterator[string] for StringLinesIterator {
    pub next :: fn (iterator: ^Self) -> ?string {
        start := iterator.offset
        for i in [iterator.offset .. iterator.text.bytes] {
            on i == iterator.offset => {
                line := iterator.text[start..i]
                iterator.offset = i
                return line
            }
        }
        return nil
    }
}

main :: fn () -> i32 {
    iterator := "hello".lines()
    return iterator.offset.as(i32) + 7
}
¬
7
¬

¬
delete
