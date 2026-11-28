=== case: optional-success-result ===
get :: fn (kind: i32) -> ?string\string {
    on kind == 0 => return nil
    on kind == 1 => return "ready"
    return "failed"!
}
forward :: fn (kind: i32) -> ?string\string {
    value := get(kind)?
    return value
}
number :: fn () -> ?i32\string { return 42 }
show :: fn (value: ?string\string) {
    on value {
        success => on success {
            text => prn($"present={text}")
            else => prn("absent")
        }
        error! => prn($"error={error}")
    }
}
main :: fn () {
    for kind in [0 .. 3] { show(get(kind)) }
    for kind in [0 .. 3] { show(forward(kind)) }
    local: ?string\string = "local"
    show(local)
    on number() {
        success => on success {
            value => prn($"number={value}")
            else => { assert no }
        }
        error! => { assert no, error }
    }
}
¬
0
¬
absent
present=ready
error=failed
absent
present=ready
error=failed
present=local
number=42
¬

¬
