answer
:
:
42

main::fn()=>answer
¬
answer :: 42

main :: fn () => answer
¬
main::fn(){return 42}
¬
main :: fn () => 42
¬
value:i32:120
main:fn()->i32:fn()=>value
¬
value : i32 : 120

main : fn () -> i32 : fn () => value
¬
base:=40
main::fn(){
base=base+1
extra:i32
extra=1
return base+extra
}
¬
base := 40

main :: fn () {
    base = base + 1
    extra : i32
    extra = 1
    return base + extra
}
¬
Price::u16
MainFn::fn()->i32
main:MainFn:fn()=>1000.as(u8)
¬
Price  :: u16
MainFn :: fn () -> i32

main : MainFn : fn () => 1000.as(u8)
¬
greeting:string="Hello"
enabled:bool=yes
ratio:f32=42.as(f32)
weight:f64

main::fn()=>enabled.as(i32)
¬
greeting : string = "Hello"
enabled  : bool   = yes
ratio    : f32    = 42.as(f32)
weight   : f64

main :: fn () => enabled.as(i32)
¬
main::fn(){value:=1{value:=2 return value}return value}
¬
main :: fn () {
    value := 1
    {
        value := 2
        return value
    }
    return value
}
¬
-- This test checks that the formatter aligns variable declarations and
-- assignments properly.

top::1

longer_top::2


typed_top:i32:3

Callable :: fn () {
}

Named :: plex { value i32 }

after_type::4

test :: fn () {
    one_var:=1

    single_const::1

    after_blank: i32 = 2
}

main :: fn () {
    answer::3.14
    typed:f64:2.0
    ratio: f64 = 3.14 + 2.0 * 3.0
    s :string = "hello"
    flags: u32 = 7 | 2 ~ 1 & 3
    same := ratio >= 7.5 && flags != 0
    return on same => "ok" else "bad"
}

bind_values :: fn () {
    first::1

    longer::2

    typed:i32:3
}

single_mixed :: fn () {
    one::1
    two:=2
    three:u32=3
}

inferred_variables :: fn () {
    short:=1
    much_longer_name:=2
}

long_values :: fn () {
    long_const: string: "this const string is deliberately long enough to wrap after the second colon"
    short_const: string: "ok"
    long_name: string = "this string is deliberately long enough to wrap after the equals"
    short: string = "ok"
}
¬
-- This test checks that the formatter aligns variable declarations and
-- assignments properly.

top :: 1

longer_top :: 2

typed_top : i32 : 3

Callable :: fn () {
}

Named :: plex {
    value i32
}

after_type :: 4

test :: fn () {
    one_var := 1

    single_const :: 1

    after_blank : i32 = 2
}

main :: fn () {
    answer          :: 3.14
    typed  : f64    : 2.0
    ratio  : f64    = 3.14 + 2.0 * 3.0
    s      : string = "hello"
    flags  : u32    = 7 | 2 ~ 1 & 3
    same            := ratio >= 7.5 && flags != 0

    return on same => "ok" else "bad"
}

bind_values :: fn () {
    first :: 1

    longer :: 2

    typed : i32 : 3
}

single_mixed :: fn () {
    one         :: 1
    two         := 2
    three : u32 = 3
}

inferred_variables :: fn () {
    short            := 1
    much_longer_name := 2
}

long_values :: fn () {
    long_const  : string :
                           "this const string is deliberately long enough to wrap after the second colon"
    short_const : string : "ok"
    long_name   : string =
                           "this string is deliberately long enough to wrap after the equals"
    short       : string = "ok"
}
¬
main :: fn () => ${ break 7 }
¬
main :: fn () => ${
    break 7
}
¬
main :: fn () => $answer { break $answer 7 }
¬
main :: fn () => $answer {
    break $answer 7
}
¬
main :: fn () {
    node.value += 1
    ptr.value += 2
    ptr^.value += 3
}
¬
main :: fn () {
    node.value += 1
    ptr.value += 2
    ptr^.value += 3
}
¬
g_term_initialised:=no
g_term:Term


other:=1
next:=2

ONE::1
TWO::2

width:i32=80
height:i32=25
title:="Demo"

DEFAULT_WIDTH::80
DEFAULT_TERM:Term:(80,25)

pub PUBLIC_ONE::1
pub PUBLIC_TWO::2
¬
g_term_initialised        := no
g_term             : Term

other := 1
next  := 2

ONE :: 1
TWO :: 2

width  : i32 = 80
height : i32 = 25
title        := "Demo"

DEFAULT_WIDTH        :: 80
DEFAULT_TERM  : Term : (80, 25)

pub PUBLIC_ONE :: 1
pub PUBLIC_TWO :: 2
¬
main :: fn () {
    a=1
    b+=2


    longer=3
    other=4
}
¬
main :: fn () {
    a = 1
    b += 2

    longer = 3
    other  = 4
}

¬
main::fn(){
value:i32=undefined
value=1
}
¬
main :: fn () {
    value : i32 = undefined
    value = 1
}
