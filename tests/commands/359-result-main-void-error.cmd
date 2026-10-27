Problem :: plex { code i32 }
impl Display for Problem {
    show :: fn (self: ^Self) -> string {
        prn("display")
        return $"problem {self.code}"
    }
}
main :: fn () -> void\Problem {
    defer prn("defer")
    undo prn("undo")
    return Problem { code: 42 }!
}

¬
1

¬
undo
defer
display

¬
delete

¬

¬
run

¬
Error: problem 42
