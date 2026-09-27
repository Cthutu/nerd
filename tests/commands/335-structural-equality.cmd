use std.slice
use structural_eq
Point :: plex { x i32 name string }
Event :: enum { None Value(Point) Pair(i32, string) }
Custom :: plex { key i32 ignored i32 }
comparisons := 0
impl Eq for Custom {
 eq :: fn (lhs:Self, rhs:Self) -> bool {
  comparisons += 1
  return lhs.key == rhs.key
 }
}
Wrapped :: plex { first i32 custom Custom }
CustomEnum :: enum { A(i32) B }
impl Eq for CustomEnum {
 eq :: fn (_lhs:Self, _rhs:Self) -> bool { return yes }
}
Packed :: plex #packed { number u8 value i64 }
Empty :: plex {}
Kind :: enum { First Second }
Bits :: plex { u32 { kind Kind : 8 offset : 24 } }
same :: fn [T] (a:T, b:T) -> bool where T: Eq { return a == b }
main :: fn () {
 a := Point { x:1, name:"a" }
 b := Point { x:1, name:"a" }
 assert same(a,b)
 b.x = 2
 assert a != b
 x: Event = Value(a)
 y: Event = Value(b)
 assert x != y
 y = Value(a)
 assert same(x,y)
 y = None
 assert x != y
 p: Event = Pair(1,"x")
 q: Event = Pair(1,"y")
 assert p != q
 items := [a,b]
 assert items.contains(a)
 c := Custom { key:1, ignored:2 }
 d := Custom { key:1, ignored:9 }
 w := Wrapped { first:1, custom:c }
 v := Wrapped { first:1, custom:d }
 comparisons = 0
 assert same(w,v)
 assert comparisons == 1
 v.first = 2
 comparisons = 0
 assert w != v
 assert comparisons == 0
 ce: CustomEnum = A(1)
 de: CustomEnum = B
 assert same(ce,de)
 assert [ce] == [de]
 left := Packed { number:1, value:42 }
 right := Packed { number:1, value:43 }
 assert left != right
 assert same(Empty {}, Empty {})
 optional: ?Point = a
 other: ?Point = b
 assert optional != other
 assert optional != nil
 absent: ?arena
 assert absent == nil
 imported_a: Holder = { item: { value:1, ignored:2 }, choice: First(1) }
 imported_b: Holder = { item: { value:1, ignored:3 }, choice: Second }
 assert imported_a == imported_b
 bits_a := Bits { kind: Kind.First, offset: 3 }
 bits_b := Bits { kind: Kind.First, offset: 3 }
 assert same(bits_a,bits_b)
 bits_b.kind = Kind.Second
 assert bits_a != bits_b
 prn("Structural equality passed")
}

¬
0
¬
Structural equality passed

¬
delete
¬
-r
