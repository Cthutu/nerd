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
