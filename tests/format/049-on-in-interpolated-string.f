use mod std.print

Colour::enum{RED GREEN BLUE}

main::fn(){
colour:Colour=RED

prn($"Colour = {on colour { RED => "red" GREEN => "green" BLUE => "blue" }}")
}
¬
use mod std.print

Colour :: enum { RED GREEN BLUE }

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
