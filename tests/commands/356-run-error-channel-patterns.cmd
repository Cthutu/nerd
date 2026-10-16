Failure :: enum { Bad WithCode { code i32 } }
plain :: fn (value: Failure) -> i32 {
    on value {
        Failure.Bad => { return 1 }
        Failure.WithCode { code } => { return code }
    }
}
result :: fn (value: i32\Failure) -> i32 {
    on value {
        Failure.Bad! => { return 1 }
        Failure.WithCode { code }! => { return code }
        success => { return success }
    }
}
table :: fn (value: i32\Failure) -> i32 {
    on value { success => { return success } } else { error => { return plain(error) } }
}
main :: fn () {
    assert plain(Failure.Bad) == 1
    assert plain(Failure.WithCode { code: 42 }) == 42
    assert result(Failure.Bad!) == 1
    assert result(Failure.WithCode { code: 42 }!) == 42
    assert result(7) == 7
    assert table(Failure.Bad!) == 1
    assert table(Failure.WithCode { code: 42 }!) == 42
    assert table(7) == 7
}
¬
0
¬
¬
delete
