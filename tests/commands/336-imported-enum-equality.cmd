use structural_eq
main :: fn () {
 a: Choice = First(1)
 b: Choice = Second
 assert a == b
 assert [a] == [b]
 prn("Imported enum equality passed")
}
¬
0
¬
Imported enum equality passed
¬
delete
¬
-r
