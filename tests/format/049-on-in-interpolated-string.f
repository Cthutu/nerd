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
            RED => "red"
            GREEN => "green"
            BLUE => "blue"
        }
    }")
}
