ready: bool
step :: fn () {
    on ready => { ready = no } else { ready = yes }
}
flip :: fn [T] (initial: T) -> T {
    value := initial
    on value => { value = no } else { value = yes }
    return value
}
main :: fn () {
    step()
    assert ready
    step()
    assert !ready
    local := no
    on local => { assert no } else { local = yes }
    assert local
    on local => { local = no }
    assert !local
    assert flip(no)
    assert !flip(yes)
    absent: ?i32 = nil
    on absent => { assert no } else { absent = 7 }
    on absent => { assert absent == 7 } else { assert no }
    result: i32\string = 42
    on result => { assert result == 42 } else { assert no }
}
¬
0
¬

¬
delete
