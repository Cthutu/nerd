main::fn(){
ratio:f64=1.5+2.0*3.0
flags:u32=7|2~1&3
same:=ratio>=7.5&&flags!=0
return on same=>"ok" else "bad"
}
¬
main :: fn () {
    ratio : f64 = 1.5 + 2.0 * 3.0
    flags : u32 = 7 | 2 ~ 1 & 3
    same        := ratio >= 7.5 && flags != 0

    return on same => "ok" else "bad"
}
¬
main::fn()->i32{
first:='a'
second:u16='ab'
newline:='\\n'
return on first==97&&second==24930&&newline==10=>0 else 1
}
¬
main :: fn () -> i32 {
    first         := 'a'
    second  : u16 = 'ab'
    newline       := '\\n'

    return on first == 97 && second == 24930 && newline == 10 => 0 else 1
}
¬
main::fn()=>0x10+0b11+0o7+1_000_000
¬
main :: fn () => 0x10 + 0b11 + 0o7 + 1_000_000
¬
main::fn()->i32{return (i32.size+128.size+void.size).as(i32)}
¬
main :: fn () -> i32 {
    return (i32.size + 128.size + void.size).as(i32)
}
¬
main::fn()=>~(0xff & 0x0f)
¬
main :: fn () => ~(0xff & 0x0f)
¬
main::fn(){
while!
window_should_close()
{
draw()
}
}
¬
main :: fn () {
    while !window_should_close() {
        draw()
    }
}
¬
main :: fn () {
    frame := Frame {
        id : NEW_FRAME
        width : 640
        height : 480
        title : "Pixels"
        full_screen : no
        resizable : yes
        ...
    }
    defer frame_system.done();
}
¬
main :: fn () {
    frame := Frame {
        id          : NEW_FRAME
        width       : 640
        height      : 480
        title       : "Pixels"
        full_screen : no
        resizable   : yes
        ...
    }
    defer frame_system.done()
}
¬
use std.term

quit :: fn (input: TermInput) {
    on input.key_pressed(Escape) ||
        -- input.key_pressed(Enter) ||
        input.key_pressed(Q) => term_done()
}
¬
use std.term

quit :: fn (input: TermInput) {
    on input.key_pressed(Escape) ||
        -- input.key_pressed(Enter) ||
        input.key_pressed(Q) => term_done()
}
