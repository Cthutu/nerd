-- Blank lines around function bindings
main::fn(){result:=add(20,22)add::fn(a:i32,b:i32)=>a+b return result}
¬
-- Blank lines around function bindings
main :: fn () {
    result := add(20, 22)

    add :: fn (a: i32, b: i32) => a + b

    return result
}
