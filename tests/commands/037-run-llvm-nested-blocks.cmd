use std.io

main :: fn () {
    value := 10
    {
        value := 32
        value = value + 1
        prn($"inner={value}")
    }
    prn($"outer={value}")
    return value
}
¬
10
¬
inner=33
outer=10

¬
delete
¬
