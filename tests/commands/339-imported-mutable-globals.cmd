use mutable_globals
use std.atomics

other :: use mutable_globals_export

separate :: use mutable_globals_other

main :: fn () {
    assert number == 7
    assert other.number == 7
    number = 37
    assert other.number == 37
    number += 2
    pointer := ^number
    pointer^ += 4
    assert other.number == 43
    assert other.values[0] == 11
    values[1] = 47
    assert other.values[1] == 47
    record.second = 53
    assert record.first == 17 && record.second == 53
    callback = replacement
    assert other.callback() == 31
    signal = 59
    assert other.signal.load() == 59
    assert separate.number == 101
    different := separate.number_address()
    assert different != pointer
    different^ = 103
    assert separate.number == 103 && number == 43
    verify()
    prn("imported globals share storage")
}
¬
0
¬
imported globals share storage
¬
delete
¬
