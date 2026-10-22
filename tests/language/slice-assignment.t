use std.io

step :: fn (log: ^i32, digit: i32, value: i32) -> i32 {
    log^ = log^ * 10 + digit
    return value
}
target :: fn (values: []i32, log: ^i32) -> []i32 {
    log^ = log^ * 10 + 1
    return values
}
fill :: fn [T] (values: []T, item: T) {
    values[..] = item
}
copy :: fn [T] (values: []T, source: []T) {
    values[..] = source
}
make :: fn (calls: ^i32) -> [2]i32 {
    calls^ += 1
    return [21, 22]
}
Mode :: enum { A B }
Pair :: plex { small u8 large i64 }
main :: fn () {
    values := [0, 1, 2, 3, 4, 5]
    values[1..5] = values[0..4]
    prn($"right {values}")
    values[0..4] = values[1..5]
    prn($"left {values}")
    values[..3] = values[2]
    prn($"alias {values}")
    values[..] = values[..]
    log: i32
    target(values[..], ^log)[step(^log, 2, 1)..step(^log, 3, 4)] = step(^log, 4, 7)
    assert log == 1234
    prn($"once {log} {values}")
    log = 0
    target(values[..], ^log)[step(^log, 2, 1)..step(^log, 3, 3)] = target(values[..], ^log)[step(^log, 4, 3)..step(^log, 5, 5)]
    assert log == 123145
    prn($"copy once {log} {values}")
    values[2..2] = step(^log, 6, 99)
    assert log == 1231456
    values[6..] = values[..0]
    fill(values[..2], 8)
    copy(values[2..4], values[..2])
    prn($"generic {values}")
    view :: values[..]
    view[4..] = 9
    prn($"borrow {values}")
    dynamic: [..]i32 = [1, 2, 3, 4]
    dynamic[1..] = 5
    dynamic[..2] = dynamic[2..]
    prn($"dynamic {dynamic}")
    dynamic.free()
    n := 3
    runtime: [n]i32
    runtime[..] = 6
    runtime[1..] = runtime[..2]
    prn($"runtime {runtime}")
    row := [7, 8]
    rows: [3][]i32
    rows[..] = row
    row[0] = 9
    assert rows[0].data == row.data && rows[2][0] == 9
    rows[..1] = rows[2..]
    prn($"nested {rows}")
    pairs: [3]Pair
    pairs[..] = { small: 2 large: 42 }
    pairs[1..] = pairs[..2]
    assert pairs[2].small == 2 && pairs[2].large == 42
    prn($"pairs {pairs[0].small} {pairs[2].large}")
    matrices: [2][2]i32
    matrices[..] = [3, 4]
    assert matrices[1][1] == 4
    flags: [3]bool
    flags[..] = yes
    assert flags[0] && flags[2]
    words: [3]string
    words[..] = "hello"
    words[1..] = words[..2]
    prn($"words {words}")
    optional: [2]?i32
    optional[..] = 42
    optional[..1] = nil
    assert optional[0] == nil && optional[1] == 42
    null: []i32
    null[..] = 1
    null[..] = null
    result :: (values[..1] = 17)
    assert result.count == 1 && result[0] == 17 && values[0] == 17
    prn($"result {result}")
    calls: i32
    values[..2] = make(^calls)
    assert calls == 1 && values[0] == 21 && values[1] == 22
    modes: [3]Mode
    modes[..] = Mode.B
    assert modes[0] == Mode.B && modes[2] == Mode.B
    for i in [0..2] {
        values[i..i + 2] = 23
    }
    assert values[0] == 23 && values[2] == 23
    for i in [0..1000000] { values[..] = i }
    assert values[0] == 999999 && values[5] == 999999
}

¬
0
¬
right [0, 0, 1, 2, 3, 5]
left [0, 1, 2, 3, 3, 5]
alias [2, 2, 2, 3, 3, 5]
once 1234 [2, 7, 7, 7, 3, 5]
copy once 123145 [2, 7, 3, 7, 3, 5]
generic [8, 8, 8, 8, 3, 5]
borrow [8, 8, 8, 8, 9, 9]
dynamic [5, 5, 5, 5]
runtime [6, 6, 6]
nested [[9, 8], [9, 8], [9, 8]]
pairs 2 42
words [hello, hello, hello]
result [17]

¬

¬

=== case: release-overlap-and-nested-fills ===
-- test-release
main :: fn () {
    values := [1, 2, 3, 4]
    values[1..] = values[..3]
    assert values[1] == 1 && values[2] == 2 && values[3] == 3
    values[..3] = values[1..]
    assert values[0] == 1 && values[1] == 2 && values[2] == 3
    views: [2][]i32
    views[..] = values
    assert views[0].data == values.data && views[1].data == values.data
    values[..] = values[2]
    assert views[1][0] == 3 && views[1][3] == 3
    values[4..] = values[..0]
    prn("release ranges")
}
¬
0
¬
release ranges
¬

¬
