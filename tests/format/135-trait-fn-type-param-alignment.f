GraphicsSystem :: trait {
get_viewport::fn(g:^Graphics,x:^i32,y:^i32,width:^i32,height:^i32)
get_self_viewport::fn(g:^Self,x:^i32,y:^i32,width:^i32,height:^i32)
}
¬
GraphicsSystem :: trait {
    get_viewport      ::
                         fn (g      : ^Graphics,
                             x      : ^i32,
                             y      : ^i32,
                             width  : ^i32,
                             height : ^i32)
    get_self_viewport ::
                         fn (g      : ^Self,
                             x      : ^i32,
                             y      : ^i32,
                             width  : ^i32,
                             height : ^i32)
}
