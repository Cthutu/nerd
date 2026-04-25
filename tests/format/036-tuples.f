use std.print

Pair :: (i32,string)
Single :: (i32,)

main :: fn(){
pair :: (7,"seven")
single :: (pair.0+1,)
grouped :: (pair.0)
from_fn: (i32, string): (3, "three")
prn($"{pair.0} {pair.1} {single.0} {grouped} {from_fn.1}")
}
¬
use std.print

Pair :: (i32, string)

Single :: (i32,)

main :: fn () {
    pair    :               : (7, "seven")
    single  :               : (pair.0 + 1,)
    grouped :               : (pair.0)
    from_fn : (i32, string) : (3, "three")

    prn($"{pair.0} {pair.1} {single.0} {grouped} {from_fn.1}")
}
