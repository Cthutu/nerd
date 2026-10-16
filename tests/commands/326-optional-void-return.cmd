noop :: fn (calls: ^i32) { calls^ += 1 }
expression_success :: fn (calls: ^i32) -> ?void { return noop(calls) }
implicit_success :: fn () -> ?void {}
explicit_success :: fn () -> ?void { return }
fail :: fn () -> ?void { return nil }
chain :: fn (ok: bool, count: ^i32) -> ?void {
    defer count^ += 10
    implicit_success()?
    explicit_success()?
    on !ok => { fail()? }
    count^ += 1
}
main :: fn () -> i32 {
    calls: i32 = 0
    on expression_success(^calls) => {} else { return 5 }
    on calls != 1 => return 8
    missing: ?void
    on missing => { return 7 }
    count: i32 = 0
    on chain(yes, ^count) => {} else { return 1 }
    on count != 11 => return 2
    on chain(no, ^count) => { return 3 }
    on count != 21 => return 4
    prn("optional void works")
    return 0
}
¬
0
¬
optional void works

¬
delete
¬
