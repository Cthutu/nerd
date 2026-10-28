use std.io

Point::plex{x i32 y i32 name string}

main::fn(){
Local::plex{value i32 label string}
p:Point=Point{x:1,y:2,name:"p"}
prn($"{p.name} {p.x} {p.y}")
}
¬
use std.io

Point :: plex {
    x    i32
    y    i32
    name string
}

main :: fn () {
    Local :: plex {
        value i32
        label string
    }
    p : Point = Point { x: 1 y: 2 name: "p" }
    prn($"{p.name} {p.x} {p.y}")
}
¬
use std.io

Person :: plex { name string age u8 }

matt :: Person {
name: "Matt"
          age: 53
}

main :: fn () {
    prn($"His name is {matt.name} and he is {matt.age} years old")
}
¬
use std.io

Person :: plex {
    name string
    age  u8
}

matt :: Person { name: "Matt" age: 53 }

main :: fn () {
    prn($"His name is {matt.name} and he is {matt.age} years old")
}
¬
use std.io

Point::plex#c{x i32 y i32 name string}
Packed::plex#packed{a u8 b i32}

main::fn(){
p:=Point{x:x,y:y,name:name}
short:=Point{x,y,name}
q:=p with{
y
name:"second"
}
pp:=^q
prn($"{pp.name} {pp.x} {pp.y}")
packed:=Packed{a:1,b:2}
}
¬
use std.io

Point :: plex #c {
    x    i32
    y    i32
    name string
}

Packed :: plex #packed {
    a u8
    b i32
}

main :: fn () {
    p     := Point { x y name }
    short := Point { x y name }
    q     := p with { y name: "second" }
    pp    := ^q

    prn($"{pp.name} {pp.x} {pp.y}")
    packed := Packed { a: 1 b: 2 }
}
¬
use std.io

Value::union{i i32 f f32 label string}

main::fn(){
v:Value=Value{i:42}
prn($"{v.i}")
}
¬
use std.io

Value :: union {
    i     i32
    f     f32
    label string
}

main :: fn () {
    v : Value = Value { i: 42 }
    prn($"{v.i}")
}
¬
use std.io
Point::plex{x i32 y i32}
origin:Point:{x:9,y:8}
points:[]Point:[{x:1,y:2},{y:4,x:3}]
¬
use std.io

Point :: plex {
    x i32
    y i32
}

origin : Point   : { x: 9 y: 8 }
points : []Point : [{ x: 1 y: 2 }, { y: 4 x: 3 }]
¬
Room::plex{name string exits [3]i32}
rooms:[]Room:[
{
name:"Hall"
exits:[1,2,3]
},
{
name:"Cellar"
exits:[4,5,6]
},
]
items:[]i32:[
1,
2,
]
¬
Room :: plex {
    name  string
    exits [3]i32
}

rooms : []Room : [
    { name : "Hall"   exits : [1, 2, 3] },
    { name : "Cellar" exits : [4, 5, 6] },
]

items : []i32 : [
    1,
    2,
]
¬
Room :: plex {
    name        string
    description string
    exits       [4]i32
}

rooms : []Room : [
    {
        name        : "Hall"
        description : "A dusty entrance hall."
        exits       : [0, 1, 2, 3]
    },
    {
        name        : "Kitchen"
        description : "An old kitchen."
        exits       : [0, 0, 0, 1]
    },
]

items : []Room : [
    { name : "key"     description : "A small brass key."      exits : [1, 2, 3, 4] },
    { name : "lantern" description : "A battered oil lantern." exits : [5, 6, 7, 8] },
    { name : "coin"    description : "An old silver coin."     exits : [9, 10, 11, 12] },
]
¬
Room :: plex {
    name        string
    description string
    exits       [4]i32
}

rooms : []Room : [
    { name : "Hall"    description : "A dusty entrance hall." exits : [0, 1, 2, 3] },
    { name : "Kitchen" description : "An old kitchen."        exits : [0, 0, 0, 1] },
]

items : []Room : [
    { name : "key"     description : "A small brass key."      exits : [1, 2, 3, 4] },
    { name : "lantern" description : "A battered oil lantern." exits : [5, 6, 7, 8] },
    { name : "coin"    description : "An old silver coin."     exits : [9, 10, 11, 12] },
]
¬
p :: Point { x : 1,... }

q := { alpha : 1, beta : 2, ... }
¬
p :: Point { x: 1 ... }

q := { alpha: 1 beta: 2 ... }
¬
TermRect :: plex {
    x      i32
    y      i32
    width  u32
    height u32
}

main :: fn () {
    rect := TermRect { x: 10 y: 5 width: 20 height: 10 }
}
¬
TermRect :: plex {
    x      i32
    y      i32
    width  u32
    height u32
}

main :: fn () {
    rect := TermRect { x: 10 y: 5 width: 20 height: 10 }
}
¬
pub Frame::plex {
    -- first field lead
    handle u64 -- native handle
    -- standalone after trailing
    system ^FrameSystem -- owning system comment
                        --     wrapped continuation
    -- standalone before title
    title string -- visible title
    -- final standalone
}
¬
pub Frame :: plex {
    -- first field lead
    handle u64           -- native handle
    -- standalone after trailing
    system ^FrameSystem  -- owning system comment wrapped continuation
    -- standalone before title
    title  string        -- visible title
    -- final standalone
}
¬
Frame::plex {
    handle u64


    width u32
    height u32
    -- comment stays attached to title
    title string

    visible bool
}

Value::union {
    int i32


    text string
}
¬
Frame :: plex {
    handle  u64

    width   u32
    height  u32
    -- comment stays attached to title
    title   string

    visible bool
}

Value :: union {
    int  i32

    text string
}
¬
Frame::plex {
    title string
    x i32
    y i32
    width u32
    height u32
}

frame:Frame:{
    title:"Example" -- title comment
    x:10 -- x comment

    -- size group
    width:640 -- width comment that should align with height and wrap onto the next line if the configured formatter width requires it
    height:480 -- height comment
}
¬
Frame :: plex {
    title  string
    x      i32
    y      i32
    width  u32
    height u32
}

frame : Frame : {
    title : "Example"  -- title comment
    x     : 10         -- x comment

    -- size group
    width  : 640  -- width comment that should align with height and wrap onto
                  --     the next line if the configured formatter width requires
                  --     it
    height : 480  -- height comment
}
¬
gfx :: use std.gfx

main :: fn () {
    fs : gfx.FrameSystem

    frame := gfx.Frame {
        handle: 0
        system: nil
        title: ""
        x: 0
        y: 0
        width: 0
        height: 0
        resizable: no
        fullscreen: no
    }
}
¬
gfx :: use std.gfx

main :: fn () {
    fs : gfx.FrameSystem

    frame := gfx.Frame {
        handle     : 0
        system     : nil
        title      : ""
        x          : 0
        y          : 0
        width      : 0
        height     : 0
        resizable  : no
        fullscreen : no
    }
}
¬
main :: fn () {
    prn_wrap("Now is the time for all good people to come to the aid of party "
    +"and make merry with the likes of us.")
}

State :: plex {
    -- Database
    locations []string

    -- Game state
    player_loc usize
}

init :: fn () -> State {
    return {
        locations:[
            "I am in a Hall.  The Kitchen is to the East, the Bedroom to the West and the Lounge to the South.  Steps lead Down to the Cellar.",
            "I am in the Cellar.  Steps lead Up to the Hall.".
        ]
        player_loc: 0
    }
}
¬
main :: fn () {
    prn_wrap("Now is the time for all good people to come to the aid of party "
        +"and make merry with the likes of us.")
}

State :: plex {
    -- Database
    locations  []string

    -- Game state
    player_loc usize
}

init :: fn () -> State {
    return {
        locations:[
            "I am in a Hall.  The Kitchen is to the East, the Bedroom to the "
                +"West and the Lounge to the South.  Steps lead Down to the "
                +"Cellar.",
            "I am in the Cellar.  Steps lead Up to the Hall.".
        ]
        player_loc: 0
    }
}
¬
use std.io
use std.random

main :: fn () {
    for _ in [0 .. 10] {
        prn($"{random_u64()}")
    }

    test()
}

test :: fn () {
    details: plex {
        age i32
        name string
    }
    = {
        age: 42, name: "Alice"
    }

    prn(on details {
        {
            age: 42, name: _
        }
        => $"Hello {details.name}!"
        else => "Whatever!"
    }
    )
}
¬
use std.io
use std.random

main :: fn () {
    for _ in [0 .. 10] {
        prn($"{random_u64()}")
    }

    test()
}

test :: fn () {
    details: plex {
        age  i32
        name string
    }
    = { age: 42 name: "Alice" }

    prn(on details {
        { age: 42, name: _ }  => $"Hello {details.name}!"
        else                  => "Whatever!"
    })
}
¬
main::fn(){
    p::Point{...}
}
¬
main :: fn () {
    p :: Point { ... }
}
¬
main :: fn () {
    wndclass :: WNDCLASSA {
        hInstance: instance
        lpszClassName: c"HandmadeHeroWindowClass"
        ...
    }

    point := Point { x: 1, y: 2, ... }
}
¬
main :: fn () {
    wndclass :: WNDCLASSA {
        hInstance     : instance
        lpszClassName : c"HandmadeHeroWindowClass"
        ...
    }

    point := Point { x: 1 y: 2 ... }
}
¬
main :: fn () {
state := {
locations:[
"hall",
]
exits:[
[("E",1)],
]
short:1
longer_name:2
verbs:[
["look","l"],
]
}
}
¬
main :: fn () {
    state := {
        locations: [
            "hall",
        ]
        exits: [
            [("E", 1)],
        ]
        short       : 1
        longer_name : 2
        verbs: [
            ["look", "l"],
        ]
    }
}
¬
PixelLayerMode :: enum {
    FixedSizeAutoScale {
        width u16
        height u16
    }
}

PixelLayer :: plex {
    mode PixelLayerMode
    width u16
    height u16
    data [..]u32
}

main :: fn () {
    layer := PixelLayer {
        mode: PixelLayerMode.FixedSizeAutoScale { width: 1 height: 1 }
        width: 1
        height: 1
        data: nil
    }
}
¬
PixelLayerMode :: enum {
    FixedSizeAutoScale {
        width  u16
        height u16
    }
}

PixelLayer :: plex {
    mode   PixelLayerMode
    width  u16
    height u16
    data   [..]u32
}

main :: fn () {
    layer := PixelLayer {
        mode   : PixelLayerMode.FixedSizeAutoScale { width: 1 height: 1 }
        width  : 1
        height : 1
        data   : nil
    }
}
¬
pub Shader :: plex {
    id GLuint
}

impl Shader {

    pub new :: fn (vertex_shader : string,
    fragment_shader : string) -> ?Self {
        return Self {
            id: 1
        }
    }
}
¬
pub Shader :: plex {
    id GLuint
}

impl Shader {

    pub new :: fn (vertex_shader: string, fragment_shader: string) -> ?Self {
        return Self { id: 1 }
    }

}
¬
main::fn(){
pixels:=gfx_system.create_pixel_layer(^main_window,PixelLayerMode.FixedSizeAutoScale{
width:400
height:300
}
)
}
¬
main :: fn () {
    pixels := gfx_system.create_pixel_layer(^main_window,
                                            PixelLayerMode.FixedSizeAutoScale {
        width  : 400
        height : 300
    })
}
¬
LineIterator :: plex {
    text string
    offset usize
}

impl string {
    pub lines :: fn (source: Self) => LineIterator {
    text: source
    offset: 0
}
}
¬
LineIterator :: plex {
    text   string
    offset usize
}

impl string {

    pub lines :: fn (source: Self) => LineIterator { text: source offset: 0 }

}
¬
TokenType :: enum {Eof Other}
Header :: plex {u8 {version:4
kind TokenType: 4}
u16 {
_ : 3
active:1 -- Active bit.
code:4
_:8}
length u16}
¬
TokenType :: enum {
    Eof
    Other
}

Header :: plex {
    u8 {
        version           : 4
        kind    TokenType : 4
    }
    u16 {
        _      : 3
        active : 1  -- Active bit.
        code   : 4
        _      : 8
    }
    length u16
}
¬
lex::fn(input:string){offset:=0
for offset<input.count{on input[offset]{in [0..=9]=>{session.tokens.push({
kind:TokenType.Integer
offset:start
}
)}}}}
¬
lex :: fn (input: string) {
    offset := 0
    for offset < input.count {
        on input[offset] {
            in [0 ..= 9] => {
                session.tokens.push({
                    kind: TokenType.Integer
                    offset: start
                })
            }
        }
    }
}

¬
Point::plex[T]{x T y T} Rect::plex{tag u8 use Point[i32] width i32}
¬
Point :: plex [T] {
    x T
    y T
}

Rect :: plex {
    tag   u8
    use Point[i32]
    width i32
}
¬
Hash::plex{
union{
bytes [20]u8 -- Bytes of the hash.
plex{a u32 -- First word.
b u32 c u32 d u32 e u32}
}
}
Union::union{plex #c{a u8 b u32}other u64}
Named::plex{field plex{x i32 y i32} tail u8}
¬
Hash :: plex {
    union {
        bytes [20]u8  -- Bytes of the hash.
        plex {
            a u32  -- First word.
            b u32
            c u32
            d u32
            e u32
        }
    }
}

Union :: union {
    plex #c {
        a u8
        b u32
    }
    other u64
}

Named :: plex {
    field plex { x i32 y i32 }
    tail  u8
}
¬
main::fn(){
on point{
Point{x:for wanted,y:height,name:_}as matched=>height
{x:0,...}=>point.y
{...,}=>0
}
on nested{
{before:1,point:{x:for (1+1),...},after:last}=>last
}
}
¬
main :: fn () {
    on point {
        Point { x: for wanted, y: height, name: _ } as matched  => height
        { x: 0, ... }                                           => point.y
        { ... }                                                 => 0
    }
    on nested {
        { before: 1, point: { x: for (1 + 1), ... }, after: last } => last
    }
}
