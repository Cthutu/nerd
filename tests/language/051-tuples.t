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
fn make_pair
param i32:a
param string:b
$0 = tuple(i32:a, string:b)
return (i32,string):$0
end
fn main
string.reset
$0 = string.start
string.append string:"pair = "
$2 = tuple(i32:7, string:"seven")
$3 = (i32,string):$2.0
string.append i32:$3
string.append string:", "
$4 = (i32,string):$2.1
string.append string:$4
$1 = string.finish $0
call fn(string)->void:prn, string:$1
string.reset
$5 = string.start
string.append string:"pair tuple = "
string.append (i32,string):$2
$6 = string.finish $5
call fn(string)->void:prn, string:$6
string.reset
$7 = string.start
string.append string:"single = "
$9 = (i32,string):$2.0
$10 = i32:$9 + i32:1
$11 = tuple(i32:$10)
$12 = (i32,):$11.0
string.append i32:$12
$8 = string.finish $7
call fn(string)->void:prn, string:$8
string.reset
$13 = string.start
string.append string:"single tuple = "
string.append (i32,):$11
$14 = string.finish $13
call fn(string)->void:prn, string:$14
string.reset
$15 = string.start
string.append string:"from_fn = "
$17 = call fn(i32,string)->(i32,string):make_pair, i32:3, string:"three"
$18 = (i32,string):$17.0
string.append i32:$18
string.append string:", "
$19 = (i32,string):$17.1
string.append string:$19
$16 = string.finish $15
call fn(string)->void:prn, string:$16
string.reset
$20 = string.start
string.append string:"nested tuple = "
$22 = tuple((i32,string):$2, (i32,):$11, bool:yes)
string.append ((i32,string),(i32,),bool):$22
$21 = string.finish $20
call fn(string)->void:prn, string:$21
string.reset
$23 = (i32,string):$2.0
$24 = (i32,):$11.0
$25 = i32:$23 + i32:$24
$26 = (i32,string):$17.0
$27 = i32:$25 + i32:$26
return i32:$27
end
¬

