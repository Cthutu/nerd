use std.io

main::fn(){prn("Hello, world!")}
¬
use std.io

main :: fn () {
    prn("Hello, world!")
}
¬
use std.io

main::fn(){prn("Hello, " +"world!")}
¬
use std.io

main :: fn () {
    prn("Hello, world!")
}
¬
-- Empty line inserted between fn and non-fn binding.
message::$"Hello,{name}! {value.as(u8)}"
main::fn()=>message
¬
-- Empty line inserted between fn and non-fn binding.
message :: $"Hello,{name}! {value.as(u8)}"

main :: fn () => message
¬
short::"Hello, " +"world!"
long_message :: "This formatter test contains a deliberately long plain string literal that should be split at spaces into continuation string literals when it is formatted."
main::fn(){text := "alpha" +" beta" +" gamma"}
¬
short        :: "Hello, world!"
long_message ::
                "This formatter test contains a deliberately long plain string literal that "
    +"should be split at spaces into continuation string literals when it is "
    +"formatted."

main :: fn () {
    text := "alpha beta gamma"
}
¬
message::"\x1b[31mred\x1b[0m"
¬
message :: "\x1b[31mred\x1b[0m"
¬
main :: fn () {
    prn_wrap($"{
        state.locations[state.player_loc]
    }")
    broken := [
}
¬
main :: fn () {
    prn_wrap($"{
        state.locations[state.player_loc]
    }")
    broken := [
    }

¬
main :: fn(){
value:=42
text:=$"literal \{value\} actual {value}"
}
¬
main :: fn () {
    value := 42
    text  := $"literal \{value\} actual {value}"
}
¬
main :: fn () {
text := """alpha
beta"""
escaped := "alpha\n  beta\n"
}

plain_shader :: ""
vertex_shader ::
"""
#version 330 core
void main() {
    indented()
}
"""

typed_shader: string: """alpha
beta"""
¬
main :: fn () {
    text :=
        """
        alpha
        beta
        """
    escaped :=
        """
        alpha
          beta
        """
}

plain_shader  :: ""
vertex_shader ::
    """
    #version 330 core
    void main() {
        indented()
    }
    """

typed_shader : string :
    """
    alpha
    beta
    """
¬
rows::[
    "first"
        +"second"
        +"third",
]
¬
rows :: [
    "first"
        +"second"
        +"third",
]
