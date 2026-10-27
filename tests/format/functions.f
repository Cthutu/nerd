add::fn(a:i32,b:i32)=>a+b
main::fn()=>add(20,22)
¬
add :: fn (a: i32, b: i32) => a + b

main :: fn () => add(20, 22)
¬
adder:=fn(a:i32,b:i32)=>a+b
main::fn(){
add::fn(a:i32,b:i32)=>a+b
return add(20,22)
}
¬
adder := fn (a: i32, b: i32) => a + b

main :: fn () {
    add :: fn (a: i32, b: i32) => a + b

    return add(20, 22)
}
¬
-- Blank lines around function bindings
main::fn(){result:=add(20,22)add::fn(a:i32,b:i32)=>a+b return result}
¬
-- Blank lines around function bindings
main :: fn () {
    result := add(20, 22)

    add :: fn (a: i32, b: i32) => a + b

    return result
}
¬
use std.io

main :: fn () -> i32 {
count := 1


next := count + 1



prn($"next: {next}")

return next
}
¬
use std.io

main :: fn () -> i32 {
    count := 1

    next := count + 1

    prn($"next: {next}")

    return next
}
¬
add::fn(a:i32,b:i32=1,c:i32=a+b)=>a+b+c
blocky::fn(
first:i32,
second:i32=first+1
)->i32{
return first+second
}
main::fn(){
add(a=1,c=3)
}
¬
add :: fn (a: i32, b: i32 = 1, c: i32 = a + b) => a + b + c

blocky :: fn (first: i32, second: i32 = first + 1) -> i32 {
    return first + second
}

main :: fn () {
    add(a = 1, c = 3)
}
¬
long_default::fn(value:usize)=>value
init::fn(arena:Self,reserved_size:usize=long_default(4),grow_rate:usize=16,alloc_granularity:usize=4096){
}
¬
long_default :: fn (value: usize) => value

init :: fn (arena             : Self,
            reserved_size     : usize = long_default(4),
            grow_rate         : usize = 16,
            alloc_granularity : usize = 4096) {
}
¬
main::fn(){
    window_handle:=CreateWindowExA(0,CLASS_NAME,c"Handmade Hero",WS_OVERLAPPEDWINDOW | WS_VISIBLE,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,0,0,instance,0)
}
¬
main :: fn () {
    window_handle := CreateWindowExA(0,
                                     CLASS_NAME,
                                     c"Handmade Hero",
                                     WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     0,
                                     0,
                                     instance,
                                     0)
}
¬
Sink :: fn (value: i32)

App :: plex {
    init fn (app:^App)
    kill fn (app:^App)
    logic fn (app:^App, delta_time: f32)
    render fn (app:^App)
}

main :: fn () {
    _app: App = {
        quit: no
        init: app_init
        kill: nil
        logic: app_logic
        render: app_render
    }
}
¬
Sink :: fn (value: i32)

App :: plex {
    init   fn (app: ^App)
    kill   fn (app: ^App)
    logic  fn (app: ^App, delta_time: f32)
    render fn (app: ^App)
}

main :: fn () {
    _app : App =
                 {
        quit   : no
        init   : app_init
        kill   : nil
        logic  : app_logic
        render : app_render
    }
}
¬
pixels_draw :: fn (layer: ^PixelLayer, frame: u32) {
    pixels := layer.paint({
                        x: 0
                        y: 0
                        width: layer.pixel_width()
                        height: layer.pixel_height()
    }, fn (pixels: []u32, width: u32, height: u32, stride: u32) {
        for y in [0 .. height] {
            row := ^pixels[y * stride]
            for x in [0 .. width] {
                row[x] = pixel_colour(x, y, frame)
            }
        }
    })

    bar_width := (frame % layer.pixel_width().as(u32)) + 1
    layer.fill(PixelRect {
        x: 0 y: 0 width: bar_width height: 3
    }
    , 0xffffffff)
}
¬
pixels_draw :: fn (layer: ^PixelLayer, frame: u32) {
    pixels := layer.paint({
        x      : 0
        y      : 0
        width  : layer.pixel_width()
        height : layer.pixel_height()
    }, fn (pixels: []u32, width: u32, height: u32, stride: u32) {
        for y in [0 .. height] {
            row := ^pixels[y * stride]
            for x in [0 .. width] {
                row[x] = pixel_colour(x, y, frame)
            }
        }
    })

    bar_width := (frame % layer.pixel_width().as(u32)) + 1
    layer.fill(PixelRect { x: 0 y: 0 width: bar_width height: 3 }, 0xffffffff)
}
¬
main :: fn () {
    glVertexAttribPointer(0, -- attribute location
                          3, -- number of components (x, y, z)
                          GL_FLOAT, -- data type
                          GL_FALSE, -- normalised?
                          6 * f32.size.as(i32), -- stride (size of a single vertex in bytes)
                          0.as(^void)) -- offset (position starts at the beginning of the vertex)
}
¬
main :: fn () {
    glVertexAttribPointer(0,                     -- attribute location
                          3,                     -- number of components (x, y,
                                                 --     z)
                          GL_FLOAT,              -- data type
                          GL_FALSE,              -- normalised?
                          6 * f32.size.as(i32),  -- stride (size of a single
                                                 --     vertex in bytes)
                          0.as(^void))           -- offset (position starts at
                                                 --     the beginning of the
                                                 --     vertex)
}
¬
main :: fn () {
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
    indicies.count.as(GLsizeiptr),
    ^indicies,
    GL_STATIC_DRAW)
    on ok => {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indicies.count.as(GLsizeiptr),
        ^indicies,
        GL_STATIC_DRAW)
    }
}
¬
main :: fn () {
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indicies.count.as(GLsizeiptr),
                 ^indicies,
                 GL_STATIC_DRAW)
    on ok => {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     indicies.count.as(GLsizeiptr),
                     ^indicies,
                     GL_STATIC_DRAW)
    }
}
¬
write::fn{write_i32
-- keep this member
io.write_string}
¬
write :: fn {
    write_i32
    -- keep this member
    io.write_string
}
¬
pub net_socket :: fn () => NetSocket { next_id: 1 generation: 1 send_ms: net_wait_infinite receive_ms: net_wait_infinite connect_ms: 1000 reconnect_ms: 25 ... }
short :: fn () => Point { x: 1 y: 2 }
long_constructor_name_that_consumes_most_of_the_available_width :: fn () => Item { value: 42 ... }
from_block :: fn () { return NetSocket { next_id: 1 generation: 1 send_ms: net_wait_infinite receive_ms: net_wait_infinite connect_ms: 1000 reconnect_ms: 25 ... } }
impl Config {
    pub create :: fn () => Self { first_value: default_value second_value: default_value third_value: default_value ... }
}
wrap :: fn () {
    factory := fn () => Config { first_value: default_value second_value: default_value third_value: default_value ... }
    _ := factory
}
¬
pub net_socket :: fn () => NetSocket {
    next_id      : 1
    generation   : 1
    send_ms      : net_wait_infinite
    receive_ms   : net_wait_infinite
    connect_ms   : 1000
    reconnect_ms : 25
    ...
}

short :: fn () => Point { x: 1 y: 2 }

long_constructor_name_that_consumes_most_of_the_available_width :: fn () => Item {
    value: 42
    ...
}

from_block :: fn () => NetSocket {
    next_id      : 1
    generation   : 1
    send_ms      : net_wait_infinite
    receive_ms   : net_wait_infinite
    connect_ms   : 1000
    reconnect_ms : 25
    ...
}

impl Config {

    pub create :: fn () => Self {
        first_value  : default_value
        second_value : default_value
        third_value  : default_value
        ...
    }

}

wrap :: fn () {
    factory := fn () => Config {
        first_value  : default_value
        second_value : default_value
        third_value  : default_value
        ...
    }
    _ := factory
}
