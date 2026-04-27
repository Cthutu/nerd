use std.io

Colour :: enum { Red Green Blue }

score :: fn (colour: Colour) -> i32 {
    return on colour {
        Red => 10
        Green => 20
        Blue => 30
    }
}

pick_shadowed :: fn () -> Colour {
    Red: Colour = Green
    picked: Colour = Red
    return picked
}

main :: fn () -> i32 {
    red: Colour = Red
    green: Colour = Green
    blue := Colour.Blue

    prn($"red {score(red)}")
    prn($"green {score(green)}")
    prn($"blue {score(blue)}")
    prn($"shadowed {score(pick_shadowed())}")
    prn($"total {score(red) + score(green) + score(blue)}")

    return 0
}
¬0¬red 10
green 20
blue 30
shadowed 10
total 60

¬¬