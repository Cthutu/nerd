calls: i32 = 0
side :: fn () -> bool { calls += 1
    return yes }
result :: fn (success: bool) -> bool\string {
    on success => return no
    return "failure"!
}
main :: fn () {
    present: ?i32 = 42
    absent: ?i32 = nil
    false_payload: ?bool = no
    assert present && yes
    assert !(absent && side()) && calls == 0
    assert present || side()
    assert calls == 0
    assert absent || side()
    assert calls == 1
    assert false_payload && yes
    on present && false_payload => { calls += 1 }
    assert calls == 2
    success := result(yes)
    failure := result(no)
    assert success && yes && !(failure && side()) && calls == 2
    assert success || side()
    assert calls == 2
    on 7 {
        7 on false_payload => { calls += 1 }
        else => { assert no }
    }
    on { present => { calls += 1 } else => { assert no } }
    assert calls == 4
    for present && calls < 5 { calls += 1 }
    assert calls == 5
    prn("Condition truthiness passed")
}
¬
0
¬
Condition truthiness passed
¬
¬
