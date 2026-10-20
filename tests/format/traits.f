Display :: trait{show::fn(self: Self)->string}
Hasher :: trait {
hash :: fn(self: Self)->u64
finish :: fn()->u64
}
DisplayValue :: trait for Value{show::fn(self: Value)->string}
¬
Display :: trait {
    show :: fn (self: Self) -> string
}

Hasher :: trait {
    hash   :: fn (self: Self) -> u64
    finish :: fn () -> u64
}

DisplayValue :: trait for Value {
    show :: fn (self: Value) -> string
}
¬
Display :: trait{show::fn(self: Self)->string}
Point :: plex{x i32 y i32}
impl Display for Point{show::fn(self:Self)=>$"Point({self.x}, {self.y})"}
¬
Display :: trait {
    show :: fn (self: Self) -> string
}

Point :: plex {
    x i32
    y i32
}

impl Display for Point {

    show :: fn (self: Self) => $"Point({self.x}, {self.y})"

}
¬
main :: fn() {
point:=Point{x:1,y:2}
_:=point.show()
_:=Display.show(point)
_:=Default[Point].default()
_:=point.convert[i32](3)
}
¬
main :: fn () {
    point := Point { x: 1 y: 2 }
    _     := point.show()
    _     := Display.show(point)
    _     := Default[Point].default()
    _     := point.convert[i32](3)
}
¬
GraphicsSystem :: trait {
get_viewport::fn(g:^Graphics,x:^i32,y:^i32,width:^i32,height:^i32)
get_self_viewport::fn(g:^Self,x:^i32,y:^i32,width:^i32,height:^i32)
}
¬
GraphicsSystem :: trait {
    get_viewport      :: fn (g      : ^Graphics,
                             x      : ^i32,
                             y      : ^i32,
                             width  : ^i32,
                             height : ^i32)
    get_self_viewport :: fn (g      : ^Self,
                             x      : ^i32,
                             y      : ^i32,
                             width  : ^i32,
                             height : ^i32)
}
¬
GraphicsSystem :: trait {
get_viewport::fn(g:^Self,
x:^i32,
y:^i32,
width:^i32,
height:^i32)
}

impl GraphicsSystem for Graphics {
pack_colour::fn(_g:^Self,_colour:Colour)->PackedColour{
return(colour.a << 24) |(colour.b << 16) |(colour.g << 8) | colour.r
}

get_ambient_light::fn(_g:^Self)->Colour{
result:Colour
return result
}
}
¬
GraphicsSystem :: trait {
    get_viewport :: fn (g      : ^Self,
                        x      : ^i32,
                        y      : ^i32,
                        width  : ^i32,
                        height : ^i32)
}

impl GraphicsSystem for Graphics {

    pack_colour :: fn (_g: ^Self, _colour: Colour) -> PackedColour {
        return (colour.a << 24) | (colour.b << 16) | (colour.g << 8) | colour.r
    }

    get_ambient_light :: fn (_g: ^Self) -> Colour {
        result : Colour
        return result
    }

}
