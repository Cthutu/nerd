Code :: enum {
    None
    Home
    End
}

score :: fn (code: Code) -> i32 {
    on code == Home => return 10
    on code == End => return 20
    return 0
}

main :: fn () -> i32 {
    home: Code = Home
    return score(home) + score(Code.End)
}
¬
30
¬
¬
¬
