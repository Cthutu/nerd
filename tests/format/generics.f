id::fn[T](value:T)=>value
Box::plex[T]{value T}
Either::union[T,E]{ok T err E}
Maybe::enum[T]{None Some(T)}
pair:Pair[i32,string]
¬
id :: fn [T] (value: T) => value

Box :: plex [T] {
    value T
}

Either :: union [T, E] {
    ok  T
    err E
}

Maybe :: enum [T] {
    None
    Some(T)
}

pair: Pair[i32, string]
¬
Iterator :: trait[Item]{next::fn(self: Self)->Item}
Mapper :: trait[Input, Output] for This{map::fn(value: This, other: Input)->Output}
¬
Iterator :: trait [Item] {
    next :: fn (self: Self) -> Item
}

Mapper :: trait [Input, Output] for This {
    map :: fn (value: This, other: Input) -> Output
}
¬
id :: fn[T](value:T)->T where T:Display{return value}
impl[T] Box[T] where T:Display{get::fn(self:Self)->T{return self.value}}
combine :: fn[Short,Longer](left:Short,right:Longer)->string where Short:Display,Longer:Debug{return left.show()}
impl[Short,Longer] Pair[Short,Longer] where Short:Display,Longer:Debug{first::fn(self:Self)->Short{return self.left}}
¬
id :: fn [T] (value: T) -> T
where T: Display {
    return value
}

impl [T] Box[T]
where T: Display {

    get :: fn (self: Self) -> T {
        return self.value
    }

}

combine :: fn [Short, Longer] (left  : Short,
                               right : Longer) -> string
where Short : Display,
      Longer: Debug {
    return left.show()
}

impl [Short, Longer] Pair[Short, Longer]
where Short : Display,
      Longer: Debug {

    first :: fn (self: Self) -> Short {
        return self.left
    }

}
¬
main::fn(){pair:=make_pair[i32,string](42,"answer")
value:f64=1E+3
other:=make_pair[Cell[i32],string](cell,"nested")
pair.0.1+=1}

¬
main :: fn () {
    pair := make_pair[i32, string](42, "answer")
    value: f64 = 1E+3
    other := make_pair[Cell[i32], string](cell, "nested")
    pair.0.1 += 1
}
¬
cells::use test.qualified_generics
main::fn(){cell:=cells.Cell[i32]{value:42}
assert cell.value==42}
¬
cells :: use test.qualified_generics

main :: fn () {
    cell := cells.Cell[i32] { value: 42 }
    assert cell.value == 42
}
