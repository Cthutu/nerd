calls : i32
failures : i32
operation :: fn (success: bool) -> i32\string {
    calls += 1
    on !success => return "failed"!
    return 7
}
optional :: fn (present: bool) -> ?i32 {
    on present => return 9
    return nil
}
identity :: fn (value: i32) => value

inspect :: fn (success: bool) -> i32\string {
    value := operation(success) else [error] {
        assert error == "failed"
        failures += 1
    }
    return value + 1
}

main :: fn () -> i32 {
    assert inspect(yes) \ 0 == 8
    assert calls == 1 && failures == 0
    assert inspect(no) \ 23 == 23
    assert calls == 2 && failures == 1

    -- Full on extraction keeps independent value and error branches.
    answer := on operation(yes) => [value] ${ break value + 1 }
              else [error] ${ assert error == "failed" break 42 }
    assert answer == 8
    recovered := on operation(no) => [value] ${ break value }
                 else [error] ${ assert error == "failed" break 42 }
    assert recovered == 42

    -- The enclosing on owns an unbraced branch's else.
    grouped := on yes => (optional(yes) else { return 1 }) else 3
    assert grouped == 9
    called := on yes => identity(optional(yes) else { return 2 }) else 3
    assert called == 9
    nested := on yes => ${
        value := optional(yes) else { return 3 }
        break value
    } else 3
    assert nested == 9
    assigned : i32
    assigned = optional(yes) else { return 4 }
    assert assigned == 9
    assert identity(operation(yes) else { return 5 }) == 7
    return 0
}
¬
0
¬

¬

¬
