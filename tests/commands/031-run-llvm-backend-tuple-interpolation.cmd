use std.io

make_pair :: fn(a: i32, b: string) -> (i32, string) {
    return (a, b)
}

main :: fn () {
    pair :: (7, "seven")
    single :: (pair.0 + 1,)
    from_fn :: make_pair(3, "three")
    nested :: (pair, single, yes)

    prn($"pair = {pair.0}, {pair.1}")
    prn($"pair tuple = {pair}")
    prn($"single = {single.0}")
    prn($"single tuple = {single}")
    prn($"from_fn = {from_fn.0}, {from_fn.1}")
    prn($"nested tuple = {nested}")

    return pair.0 + single.0 + from_fn.0
}
¬
18
¬
pair = 7, seven
pair tuple = (7, seven)
single = 8
single tuple = (8,)
from_fn = 3, three
nested tuple = ((7, seven), (8,), yes)

¬
delete
¬
--llvm-backend
