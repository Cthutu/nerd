Counter :: plex {
    current i32
    end i32
}

impl Iterator[i32] for Counter {
    next :: fn (self: ^Self) -> ?i32 {
        result: ?i32 = nil
        on self.current < self.end {
            yes => {
                value := self.current
                self.current += 1
                result = value
            }
            else => {}
        }
        return result
    }
}

PtrIter :: plex {
    data []i32
    index usize
}

impl Iterator[^i32] for PtrIter {
    next :: fn (self: ^Self) -> ?^i32 {
        result: ?^i32 = nil
        on self.index < self.data.count {
            yes => {
                ptr := ^self.data[self.index]
                self.index += 1
                result = ptr
            }
            else => {}
        }
        return result
    }
}

main :: fn () -> i32 {
    counter := Counter { current: 0, end: 4 }
    total := 0
    for value in counter {
        total += value
    }

    values: [3]i32 = [2, 3, 5]
    for builtin in values[..] {
        total += builtin^
    }

    iter := PtrIter { data: values[..], index: 0 }
    for value in iter {
        total += value^
    }

    prn($"total={total}")
    return total
}
¬
26
¬
total=26

¬
hir 0
¬
