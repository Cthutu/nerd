Info :: plex { value i32 other i32 }
CreateInfo :: plex { info ^Info }

read :: fn (info: ^Info) -> i32 { return info.value }

main :: fn () {
    pointer := ^Info { value: 7, ... }
    assert pointer.value == 7
    assert pointer.other == 0
    pointer.value = 9
    assert read(pointer) == 9

    create := CreateInfo { info: ^Info { value: 11, ... } }
    assert create.info.value == 11
    create.info.other = 3
    assert create.info.other == 3

    contextual := CreateInfo { info: ^{ value: 13, ... } }
    assert contextual.info.value == 13
    assert read(^{ value: 17, ... }) == 17
    grouped := ^(Info { value: 19, ... })
    assert grouped.value == 19

    calls := 0
    first := ^Info { value: calls += 1, ... }
    second := ^Info { value: calls += 1, ... }
    assert calls == 2
    assert first.value == 1
    assert second.value == 2
    assert first != second
}
¬
0
¬
