Error :: enum { Failed }
step :: fn (ok: bool) -> ?void {
    on !ok => return nil
}
work :: fn (first: bool, second: bool, later: bool, log: ^i32) -> ?void {
    defer log^ = log^ * 10 + 9
    step(first)? undo log^ = log^ * 10 + 1
    step(second)?
    undo { log^ = log^ * 10 + 2 }
    step(later)?
}
forward :: fn (value: ?i32, log: ^i32) -> ?i32 {
    undo log^ += 1
    return value
}
result :: fn (ok: bool, log: ^i32) -> void\Error {
    undo log^ = log^ * 10 + 1
    defer log^ = log^ * 10 + 2
    undo log^ = log^ * 10 + 3
    on !ok => return (Error.Failed)!
}
result_chain :: fn (ok: bool, log: ^i32) -> void\Error {
    undo log^ = log^ * 10 + 4
    result(ok, log)?
}
nested :: fn (early: bool, log: ^i32) -> ?void {
    undo log^ = log^ * 10 + 1
    on yes => {
        undo log^ = log^ * 10 + 2
        on early => return nil
    }
    return nil
}
latest :: fn (log: ^i32) -> ?void {
    value := 1
    undo log^ = value
    value = 7
    return nil
}
looped :: fn (log: ^i32) -> ?void {
    for i := 0; i < 3; i += 1 {
        undo log^ = log^ * 10 + 1
        defer log^ = log^ * 10 + 2
        on i == 0 => again
        on i == 1 => break
    }
    return nil
}
main :: fn () -> ?void {
    log: i32 = 0
    on work(no, yes, yes, ^log) => return nil
    on log != 9 => return nil
    log = 0
    on work(yes, no, yes, ^log) => return nil
    on log != 19 => return nil
    log = 0
    on work(yes, yes, no, ^log) => return nil
    on log != 219 => return nil
    log = 0
    work(yes, yes, yes, ^log)?
    on log != 9 => return nil
    log = 0
    on forward(nil, ^log) => return nil
    on log != 1 => return nil
    _value := forward(42, ^log)?
    on log != 1 => return nil
    log = 0
    on result_chain(no, ^log) => return nil
    on log != 3214 => return nil
    log = 0
    on result_chain(yes, ^log) => {} else return nil
    on log != 2 => return nil
    log = 0
    on nested(yes, ^log) => return nil
    on log != 21 => return nil
    log = 0
    on nested(no, ^log) => return nil
    on log != 1 => return nil
    log = 0
    on latest(^log) => return nil
    on log != 7 => return nil
    log = 0
    on looped(^log) => return nil
    on log != 22 => return nil
    prn("Undo registration, failure cleanup and success paths passed")
}
¬
0
¬
Undo registration, failure cleanup and success paths passed

¬
delete
¬
