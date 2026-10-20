enabled:bool=yes
main::fn()=>on enabled=>42 else 7

blocky::fn(){
on enabled=>{ return } else { return }
}
¬
enabled: bool = yes

main :: fn () => on enabled => 42 else 7

blocky :: fn () {
    on enabled => {
        return
    } else {
        return
    }
}
¬
-- Branches need to be on separate lines

-- Block-type statements (on/for) should be on separate line in expression functions

main::fn()=>on 2{0=>10 1=>10 else=>30}
¬
-- Branches need to be on separate lines

-- Block-type statements (on/for) should be on separate line in expression
-- functions

main :: fn () =>
    on 2 {
        0     => 10
        1     => 10
        else  => 30
    }
¬
main::fn()=>on 2{0,1=>10 else=>30}
¬
main :: fn () =>
    on 2 {
        0, 1  => 10
        else  => 30
    }
¬
main::fn()=>on 2{0..2=>10 2..=4=>20 else=>30}
¬
main :: fn () =>
    on 2 {
        0..2   => 10
        2..=4  => 20
        else   => 30
    }
¬
main::fn()=>on 2{0,1 as matched=>matched 2..=4 as ranged=>ranged else as other=>other}
¬
main :: fn () =>
    on 2 {
        0, 1 as matched  => matched
        2..=4 as ranged  => ranged
        else as other    => other
    }
¬
use std.io

main :: fn () {
    i  :: 2
    ff :: 3.14
    s  :: "Hello, world!"

    prn($"{s}  i = {i} and f = {ff}!")

    prn(on i { 0 => "zero" else as x => $"non-zero: {x}" })
}
¬
use std.io

main :: fn () {
    i  :: 2
    ff :: 3.14
    s  :: "Hello, world!"

    prn($"{s}  i = {i} and f = {ff}!")

    prn(on i {
        0          => "zero"
        else as x  => $"non-zero: {x}"
    })
}
¬
use std.io

main :: fn () {
    value :: 2

    on value { 1 => prn("one") }
}
¬
use std.io

main :: fn () {
    value :: 2

    on value {
        1 => prn("one")
    }
}
¬
main::fn()=>on 2{0..3 as matched on matched==2=>matched+20 0..3=>10 10 as size on size==10=>size*10 else as size=>size+100}
¬
main :: fn () =>
    on 2 {
        0..3 as matched on matched == 2  => matched + 20
        0..3                             => 10
        10 as size on size == 10         => size * 10
        else as size                     => size + 100
    }
¬
Point :: plex { x i32 y i32 name string }
main :: fn () => on Point { x: 3, y: 4, name: "origin" } {
{ name: "origin", x: x } => x
{ x: 3, y: y } => y * 10
{ name: name } => 0
else => 1
}
¬
Point :: plex {
    x    i32
    y    i32
    name string
}

main :: fn () =>
    on Point { x: 3 y: 4 name: "origin" } {
        { name: "origin", x: x }  => x
        { x: 3, y: y }            => y * 10
        { name: name }            => 0
        else                      => 1
    }
¬
use std.io

Colour::enum{RED GREEN BLUE}

main::fn(){
colour:Colour=RED

prn($"Colour = {on colour { RED => "red" GREEN => "green" BLUE => "blue" }}")
}
¬
use std.io

Colour :: enum {
    RED
    GREEN
    BLUE
}

main :: fn () {
    colour : Colour = RED

    prn($"Colour = {
        on colour {
            RED    => "red"
            GREEN  => "green"
            BLUE   => "blue"
        }
    }")
}
¬
main::fn()->i32{
return on{1<0=>0 1==1=>1 else=>2}
}

score::fn(value:i32)->i32{
return on value{<0=>-1 ==0=>0 >=10=>100 else=>20}
}
¬
main :: fn () -> i32 {
    return on {
        1 < 0   => 0
        1 == 1  => 1
        else    => 2
    }
}

score :: fn (value: i32) -> i32 {
    return on value {
        < 0    => -1
        == 0   => 0
        >= 10  => 100
        else   => 20
    }
}
¬
on   "debug"{
answer::7

on !"feature"{
use  std.io
fallback::9
}
}
¬
on "debug" {

    answer :: 7

    on !"feature" {

        use std.io
        fallback :: 9

    }

}
¬
split::fn(s:string,sep:string)->[..]string{parts:[..]string on sep.count==0=>{parts.push(s)return parts} on sep.count==1=>${break s.data[0]} return parts}
¬
split :: fn (s: string, sep: string) -> [..]string {
    parts : [..]string
    on sep.count == 0 => {
        parts.push(s)
        return parts
    }
    on sep.count == 1 => ${
        break s.data[0]
    }
    return parts
}
¬
main::fn(){on value==2=>{on{value==1=>hit=1 else=>hit=2}}}

objs: []Object = [
    { description: "a silver coin", tag: "silver", location: ^locs[0] },
    { description: "a gold coint", tag: "gold", location: ^locs[1] },
    { description: "a burly guard", tag: "guard", location: ^locs[0] },
]
¬
main :: fn () {
    on value == 2 => {
        on {
            value == 1  => hit = 1
            else        => hit = 2
        }
    }
}

objs: []Object = [
    { description : "a silver coin" tag : "silver" location : ^locs[0] },
    { description : "a gold coint"  tag : "gold"   location : ^locs[1] },
    { description : "a burly guard" tag : "guard"  location : ^locs[0] },
]
¬
Event::enum{KeyDown(KeyEvent)}
KeyEvent::plex{keycode Key modifiers i32 char u32}
Key::enum{Escape=41 Other=42}
main::fn()=>on event{
KeyDown(gfx.FrameKeyEvent{
keycode:Escape
})=>{}
KeyDown(KeyEvent{keycode:Other})=>{}
}
¬
Event :: enum {
    KeyDown(KeyEvent)
}

KeyEvent :: plex {
    keycode   Key
    modifiers i32
    char      u32
}

Key :: enum {
    Escape = 41
    Other  = 42
}

main :: fn () =>
    on event {
        KeyDown(gfx.FrameKeyEvent { keycode: Escape }) => {
        }

        KeyDown(KeyEvent { keycode: Other }) => {
        }
    }
¬
find :: fn (values: [..]i32, needle: i32) -> i32 {
return for value in values {
break on value^ == needle => value^
} else {
break -1
}
}

labelled :: fn () -> i32 {
return $pick {
break $pick on 1 == 1 => 5
break $pick 9
}
}

full_else :: fn () -> i32 {
return ${
break on 1 < 2 => ${
break 3
}
else ${
break 4
}
}
}
¬
find :: fn (values: [..]i32, needle: i32) -> i32 {
    return for value in values {
        break on value^ == needle => value^
    } else {
        break -1
    }
}

labelled :: fn () -> i32 {
    return $pick {
        break $pick on 1 == 1 => 5
        break $pick 9
    }
}

full_else :: fn () -> i32 {
    return ${
        break on 1 < 2 => ${
            break 3
        } else ${
            break 4
        }
    }
}
¬
exit_strings::fn(exit:string)->string{return on exit{"N"=>"North"
"S"=>"South"
"E"=>"East"
else=>exit}}
¬
exit_strings :: fn (exit: string) -> string {
    return on exit {
        "N"   => "North"
        "S"   => "South"
        "E"   => "East"
        else  => exit
    }
}
¬
main :: fn () {
    -- Normalise verb
    for i, verb_group in state.verbs {
        for verb_synonym in verb_group^ {
            on verb == verb_synonym^ => verb = state.verbs[i][0]
        }
    }

    on verb {
        "q", "quit" => {
            prn("Goodbye!")
            break
        }
        "n", "s", "e", "w", "u", "d" => move(state, verb)
        else => {
            prn("I don't understand that command.")
        }
    }
}

exit_strings :: fn (exit: string) -> string {
    return on exit {
        "N" => "North"
            "S" => "South"
            "E" => "East"
            "W" => "West"
            "U" => "Up"
            "D" => "Down"
        else => exit
    }
}
¬
main :: fn () {
    -- Normalise verb
    for i, verb_group in state.verbs {
        for verb_synonym in verb_group^ {
            on verb == verb_synonym^ => verb = state.verbs[i][0]
        }
    }

    on verb {
        "q", "quit" => {
            prn("Goodbye!")
            break
        }

        "n", "s", "e", "w", "u", "d" => move(state, verb)

        else => {
            prn("I don't understand that command.")
        }
    }
}

exit_strings :: fn (exit: string) -> string {
    return on exit {
        "N"   => "North"
        "S"   => "South"
        "E"   => "East"
        "W"   => "West"
        "U"   => "Up"
        "D"   => "Down"
        else  => exit
    }
}
¬
on "linux"{test "platform case"{assert yes}}
¬
on "linux" {

    test "platform case" {
        assert yes
    }

}
¬
use std.term

simulate :: fn (simulation: TermSimulate) {
on simulation.key_down(Escape)||simulation.key_down(Enter)||simulation.key_down(Q)=>term_done()
}
¬
use std.term

simulate :: fn (simulation: TermSimulate) {
    on simulation.key_down(Escape) ||
        simulation.key_down(Enter) ||
        simulation.key_down(Q) => term_done()
}
¬
Map :: plex {
    width  u32
    height u32
}

Size :: plex {
    width  u32
    height u32
}

score :: fn (map: ^Map, size: Size) -> i32 {
return on map^ {{width: for size.width,height: for size.height}=>1 else=>2}
}
¬
Map :: plex {
    width  u32
    height u32
}

Size :: plex {
    width  u32
    height u32
}

score :: fn (map: ^Map, size: Size) -> i32 {
    return on map^ {
        { width: for size.width, height: for size.height }  => 1
        else                                                => 2
    }
}
¬
main :: fn () {
    (char, ink, paper) := on tile { 0x00 => (' ', 0x000000, 0x000000) 0x01 => (' ', 0xffffff, 0xffffff) else => ('?', 0xff00ff, 0x000000) }
}
¬
main :: fn () {
    (char, ink, paper) := on tile {
        0x00  => (' ', 0x000000, 0x000000)
        0x01  => (' ', 0xffffff, 0xffffff)
        else  => ('?', 0xff00ff, 0x000000)
    }
}
¬
main :: fn () {
on
x > 0
{
y := 1
}
}
¬
main :: fn () {
    on x > 0 {
        y := 1
    }
}
¬
main::fn(){a:=on value{Short=>1 MuchLonger=>2 else=>3}
b:=on value{One=>1 Two=>{work()} Three=>3 FourLong=>4}
c:=on {x>10=>1 x>2&&x<8=>2 else=>3}}
¬
main :: fn () {
    a := on value {
        Short       => 1
        MuchLonger  => 2
        else        => 3
    }
    b := on value {
        One => 1

        Two => {
            work()
        }

        Three     => 3
        FourLong  => 4
    }
    c := on {
        x > 10          => 1
        x > 2 && x < 8  => 2
        else            => 3
    }
}
¬
main::fn(){return on keysym{A=>ScanCode.A VeryLongPattern=>ScanCode.B else=>on (keysym>='0'&&keysym<='9')=>keysym.as(ScanCode) else on (keysym>='A'&&keysym<='Z')=>keysym.as(ScanCode) else on (keysym>='a'&&keysym<='z')=>(keysym-32).as(ScanCode) else ScanCode.Unknown}}
¬
main :: fn () {
    return on keysym {
        A                => ScanCode.A
        VeryLongPattern  => ScanCode.B

        else => on (keysym >= '0' && keysym <= '9')
            =>   keysym.as(ScanCode)
            else on (keysym >= 'A' && keysym <= 'Z')
            =>   keysym.as(ScanCode)
            else on (keysym >= 'a' && keysym <= 'z')
            =>   (keysym - 32).as(ScanCode)
            else ScanCode.Unknown
    }
}
¬
main :: fn () {
    frame := Frame {
        id : NEW_FRAME
        width : 800
        height : 600
        title : "OpenGL Triangle"
        full_screen : no
        resizable : yes
        ...
    }

    on scan_code {
        Escape, Q => {
            close()
        }
    }
}
¬
main :: fn () {
    frame := Frame {
        id          : NEW_FRAME
        width       : 800
        height      : 600
        title       : "OpenGL Triangle"
        full_screen : no
        resizable   : yes
        ...
    }

    on scan_code {
        Escape, Q => {
            close()
        }
    }
}
¬
main::fn(){on x{0=>return on y{0=>a() 1=>b() else=>c()} 1=>{on z{0=>return on q{0=>d() else=>e()} 1=>f()}}}}
¬
main :: fn () {
    on x {
        0 => return on y {
            0     => a()
            1     => b()
            else  => c()
        }

        1 => {
            on z {
                0 => return on q {
                    0     => d()
                    else  => e()
                }

                1 => f()
            }
        }
    }
}
¬
pub VAO::plex{id GLuint}

impl VAO {
pub new::fn()->?Self{
vao:GLuint
return on vao{0=>None else=>Some(Self{id:vao})}
}
}
¬
pub VAO :: plex {
    id GLuint
}

impl VAO {

    pub new :: fn () -> ?Self {
        vao : GLuint
        return on vao { 0 => None else => Some(Self { id: vao }) }
    }

}
¬
main :: fn () {
vao = on VAO.new().expect("Failed to create VAO")
ebo = EBO.new(indicies).expect("Failed to create EBO")
}
¬
main :: fn () {
    vao = on VAO.new().expect("Failed to create VAO")
    ebo = EBO.new(indicies).expect("Failed to create EBO")
}
¬
main::fn(event:Event){
on event{
None=>{
}
Closed=>{
close()
}
Resized{width:_}=>{
}
}
}
¬
main :: fn (event: Event) {
    on event {
        None => {
        }

        Closed => {
            close()
        }

        Resized { width: _ } => {
        }
    }
}
¬
main::fn(event:Event){on event{None=>{} Escape=>close() Enter=>{toggle_fullscreen()} Other=>ignore() Unknown=>ignore() Closed=>{shutdown()}}}
¬
main :: fn (event: Event) {
    on event {
        None => {
        }

        Escape => close()

        Enter => {
            toggle_fullscreen()
        }

        Other    => ignore()
        Unknown  => ignore()

        Closed => {
            shutdown()
        }
    }
}
¬
main::fn(){colour:=on(pixel&(1<<(7-bit.as(u8))))!=0=>ZXColours[attr&0x07]else ZXColours[(attr>>3)&0x07]}
¬
main :: fn () {
    colour := on (pixel & (1 << (7 - bit.as(u8)))) != 0
        =>   ZXColours[attr & 0x07]
        else ZXColours[(attr >> 3) & 0x07]
}
¬
main :: fn (result: string\Failure) {
    on result => [file]
    {
        prn(file)
    } else [error]
    {
        prn(error)
    }
}
¬
main :: fn (result: string\Failure) {
    on result => [file] {
        prn(file)
    } else [error] {
        prn(error)
    }
}
¬
main :: fn (result: string\Failure) {
    on result => {
        prn(result)
    } else {
        prn(result)
    }
}
¬
main :: fn (result: string\Failure) {
    on result => {
        prn(result)
    } else {
        prn(result)
    }
}
¬
Key :: enum { Q Escape }
Event :: enum { Press { key Key } }
main :: fn () {
    event := Event.Press { key: Q }
    on event {
        Press { key: Q }, Press { key: ESC } => {}
    }
}
¬
Key :: enum {
    Q
    Escape
}

Event :: enum {
    Press {
        key Key
    }
}

main :: fn () {
    event := Event.Press { key: Q }
    on event {
        Press { key: Q },
        Press { key: ESC } => {
        }
    }
}
¬
main::fn(){
first:=open() else [error] {
report(error)
return
}
second:=try_open() else [error]{
report(error)
}
third:=optional() else {}
fourth:=optional() else {again}
fifth:=a_very_long_function_name(first_argument, second_argument, third_argument, fourth_argument) else [error] {
report(error)
}
sixth:=first_operation() else { return second_operation() else {return} }
}
¬
main :: fn () {
    first := open() else [error] {
        report(error)
        return
    }
    second := try_open() else [error] {
        report(error)
    }
    third := optional() else {
    }
    fourth := optional() else {
        again
    }
    fifth := a_very_long_function_name(first_argument,
                                       second_argument,
                                       third_argument,
                                       fourth_argument) else [error] {
        report(error)
    }
    sixth := first_operation() else {
        return second_operation() else {
            return
        }
    }
}
