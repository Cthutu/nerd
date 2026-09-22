use std.term

main :: fn () {
    events: [..]TermInputEvent
    defer events.free()
    for i: u32 = 0; i < 40; i += 1 {
        events.push(TermInputEvent.Character(i + 65))
        assert (^events[0]).as(usize) % 16 == 0
    }
    events.reserve_to(100)
    assert (^events[0]).as(usize) % 16 == 0
    for i: usize = 0; i < events.count; i += 1 {
        on events[i] {
            Character(value) => { assert value == i.as(u32) + 65 }
            else => { assert no }
        }
    }
    reserved: [4..]TermInputEvent
    defer reserved.free()
    reserved.push(TermInputEvent.Character(99))
    assert (^reserved[0]).as(usize) % 16 == 0
}
¬
0
¬
