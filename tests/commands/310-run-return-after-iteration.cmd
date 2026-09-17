Failure :: enum { Missing }
lookup :: fn (values: []i32, wanted: i32) -> i32\Failure {
    for value in values {
        on value^ == wanted => return value^
    }
    return (Failure.Missing)!
}
main :: fn () {
    values: []i32 = [2, 4, 6]
    found := lookup(values, 4)
    on found => { assert found == 4 } else { assert no }
    missing := lookup(values, 5)
    on missing => { assert no } else { assert missing == Failure.Missing }
    empty: []i32
    absent := lookup(empty, 4)
    on absent => { assert no } else { assert absent == Failure.Missing }
}
¬
0
¬

¬
delete
