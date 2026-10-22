use std.io

Numbers :: [3]i32

main :: fn(){
xs::[1,2,3]
ys:[2]i32=[4,5]
prn($"{xs[0]} {ys[1]}")
}
¬
use std.io

Numbers :: [3]i32

main :: fn () {
    xs          :: [1, 2, 3]
    ys : [2]i32 = [4, 5]

    prn($"{xs[0]} {ys[1]}")
}
¬
use std.io

main :: fn(){
values:[5]i32=[10,20,30,40,50]
all:[]i32=values[..]
middle:[]i32=values[1..4]
from_start:[]i32=values[..3]
to_end:[]i32=values[2..]
literal:[]i32=[1,2,3][..]
prn($"{all} {middle.count} {middle.data[0]}")
}
¬
use std.io

main :: fn () {
    values     : [5]i32 = [10, 20, 30, 40, 50]
    all        : []i32  = values[..]
    middle     : []i32  = values[1..4]
    from_start : []i32  = values[..3]
    to_end     : []i32  = values[2..]
    literal    : []i32  = [1, 2, 3][..]

    prn($"{all} {middle.count} {middle.data[0]}")
}
¬
use std.io

main::fn(){text::"hello" whole:string=text[..] ell:string=text[1..4] prn($"{ell.count} {ell.data[0]}")}
¬
use std.io

main :: fn () {
    text           :: "hello"
    whole : string = text[..]
    ell   : string = text[1..4]

    prn($"{ell.count} {ell.data[0]}")
}
¬
main::fn()->i32{
text:[4]u8=['a','b','c','d']
ptr:=text[..].data
view:=ptr.as([]u8,2)
empty:[]u8=nil
return view.count.as(i32)*100+view[0].as(i32)+empty.count.as(i32)
}
¬
main :: fn () -> i32 {
    text  : [4]u8 = ['a', 'b', 'c', 'd']
    ptr           := text[..].data
    view          := ptr.as([]u8, 2)
    empty : []u8  = nil

    return view.count.as(i32) * 100 + view[0].as(i32) + empty.count.as(i32)
}
¬
main :: fn () {
names:[4..]string=["north","south"]
names.push("east")
names.reserve(10)
view:=names[..]
names.append(view)
names.clear()
names.free()
}
¬
main :: fn () {
    names : [4..]string = ["north", "south"]
    names.push("east")
    names.reserve(10)
    view := names[..]
    names.append(view)
    names.clear()
    names.free()
}
¬
main :: fn () {
    vertices : [15]GLfloat =
                             [
    0.0,
    0.6,
    1.0,
    0.2,
    0.2,
    -0.6,
    -0.5,
    0.2,
    1.0,
    0.3,
    0.6,
    -0.5,
    0.2,
    0.4,
    1.0,
]
}
¬
main :: fn () {
    vertices: [15]GLfloat = [
        0.0,
        0.6,
        1.0,
        0.2,
        0.2,
        -0.6,
        -0.5,
        0.2,
        1.0,
        0.3,
        0.6,
        -0.5,
        0.2,
        0.4,
        1.0,
    ]
}
¬
main::fn(){
--[
-- outside array
--]
values:[]i32:[
--[
1,      2, 3,
4, 5,      6,
--]
]
}
¬
main :: fn () {
    --[
    -- outside array
    --]
    values : []i32 : [
--[
1,      2, 3,
4, 5,      6,
--]
]
}
¬
ZXColours :: [
    0xff000000, -- Black
    0xff0000cd, -- Blue
    0xffcd0000, -- Red
    0xffcdcd00, -- Magenta
    0xff00cd00, -- Green
    0xff00cdcd, -- Cyan
    0xffcdcd00, -- Yellow
    0xffcdcdcd, -- White
    0xff000000, -- Bright Black
    0xff0000ff, -- Bright Blue
    0xffff0000, -- Bright Red
    0xffff00ff, -- Bright Magenta
    0xff00ff00, -- Bright Green
    0xff00ffff, -- Bright Cyan
    0xffffff00, -- Bright Yellow
    0xffffffff  -- Bright White
]

Points :: [
    { x: 1 y: 20 }, -- First
    { x: 300 y: 4 } -- Second
]
¬
ZXColours :: [
    0xff000000,  -- Black
    0xff0000cd,  -- Blue
    0xffcd0000,  -- Red
    0xffcdcd00,  -- Magenta
    0xff00cd00,  -- Green
    0xff00cdcd,  -- Cyan
    0xffcdcd00,  -- Yellow
    0xffcdcdcd,  -- White
    0xff000000,  -- Bright Black
    0xff0000ff,  -- Bright Blue
    0xffff0000,  -- Bright Red
    0xffff00ff,  -- Bright Magenta
    0xff00ff00,  -- Bright Green
    0xff00ffff,  -- Bright Cyan
    0xffffff00,  -- Bright Yellow
    0xffffffff,  -- Bright White
]

Points :: [
    { x : 1   y : 20 },  -- First
    { x : 300 y : 4 },  -- Second
]
¬
ZXColours :: [
    0xff000000, -- Black
    0xff0000cd, -- Blue
    0xffffffff  -- Bright White
]

screen :: @embed "AticAtac.scr"
¬
ZXColours :: [
    0xff000000,  -- Black
    0xff0000cd,  -- Blue
    0xffffffff,  -- Bright White
]

screen :: @embed "AticAtac.scr"
¬
use std.files
use std.process

INPUT_FILE :: "return_2.c"

main :: fn () {
    preprocess_file(INPUT_FILE)
}

preprocess_file :: fn (input: string) {
    _ := run([
        "clang",
        "-E",
        input,
    ])
}

compile_file :: fn (input: string) {
    -- Compile the preprocessed file to assembly
    on read_text(input) => [file]
    {
        prn(file)
    }
}
¬
use std.files
use std.process

INPUT_FILE :: "return_2.c"

main :: fn () {
    preprocess_file(INPUT_FILE)
}

preprocess_file :: fn (input: string) {
    _ := run([
        "clang",
        "-E",
        input,
    ])
}

compile_file :: fn (input: string) {
    -- Compile the preprocessed file to assembly
    on read_text(input) => [file] {
        prn(file)
    }
}
¬
main::fn(){
digest:=[20]u8
}
¬
main :: fn () {
    digest := [20]u8
}
¬
main::fn(){
view:=[]u8
}
¬
main :: fn () {
    view := []u8
}
¬
main::fn(){
growing:=[..]u8
}
¬
main :: fn () {
    growing := [..]u8
}
¬
main::fn(){
nested:=[2][20]u8
}
¬
main :: fn () {
    nested := [2][20]u8
}
¬
main::fn(){
values:=[20]
consume(values)
}
¬
main :: fn () {
    values := [20]
    consume(values)
}
