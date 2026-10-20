calls: i32 = 0
Settings :: plex { value i32 }
impl Default for Settings {
    default :: fn () -> Self { calls += 1
        return { value: 42 }
    }
}
Owned :: plex { data box[i32] }
impl Default for Owned {
    default :: fn () -> Self { calls += 1
        return { data: box[i32]() }
    }
}
main :: fn () -> i32 {
    on yes => { nested: Settings
        assert nested.value == 42 && calls == 1
    }
    for _i in [0 .. 3] { scoped: Settings
        assert scoped.value == 42
    }
    assert calls == 4
    owned: Owned
    assert calls == 5 && owned.data != nil
    owned.data.free()
    calls = 0
    settings: Settings
    prn($"{settings.value}")
    return calls
}
¬
1
¬
42
¬
¬
