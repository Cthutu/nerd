=== case: optional-result-record-fields ===
Record :: plex { number ?u32 text ?string result i32\string }
Event :: enum { Value { number ?u32 result i32\string } }
check :: fn (record: Record, expected: u32) {
    on record.number { value => { assert value == expected } else => { assert no } }
    on record.text { value => { assert value == "text" } else => { assert no } }
    on record.result { value => { assert value == 42 } error! => { assert no, error } }
}
main :: fn () {
    record := Record { number: 127 text: "text" result: 42 }
    check(record, 127)
    updated := record with { number: 8 }
    check(updated, 8)
    event := Event.Value { number: 9 result: 42 }
    on event {
        Value { number, result } => {
            on number { value => { assert value == 9 } else => { assert no } }
            on result { value => { assert value == 42 } error! => { assert no, error } }
        }
    }
}
¬
0
¬

¬

¬
