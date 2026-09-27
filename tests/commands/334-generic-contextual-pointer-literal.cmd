use inferred_fill

Info :: plex { value i32 other i32 }
Other :: plex { enabled bool }
local_fill :: fn [T] (items: []T, value: ^T) {
    for item in items { item^ = value^ }
}
main :: fn () {
    count := 3
    items: [count]Info
    calls := 0
    fill(items, ^{ value: calls += 1, ... })
    assert calls == 1
    for item in items {
        assert item.value == 1
        assert item.other == 0
    }
    local_fill(items, ^{ value: 7, ... })
    assert items[2].value == 7
    fixed: [2]Other
    fill(fixed, ^{ enabled: yes })
    assert fixed[0].enabled
    local_fill(fixed, ^{ enabled: no })
    assert !fixed[1].enabled
    view := items[..]
    fill(view, ^{ value: 9, ... })
    assert items[0].value == 9
    items.fill_context(^{ value: 11, ... })
    assert items[1].value == 11
    fixed.fill_context(^{ enabled: yes })
    assert fixed[1].enabled
    prn("Generic argument context passed")
}

¬
0
¬
Generic argument context passed

¬
delete
¬
-r
