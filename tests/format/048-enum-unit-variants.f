Colour::enum{Red Green Blue}

score::fn(colour:Colour)->i32{
return on colour{
Red=>10
Green=>20
Blue=>30
}
}

main::fn(){
red:Colour=Red
score(red)
}
¬
Colour :: enum { Red Green Blue }

score :: fn (colour: Colour) -> i32 {
    return on colour {
        Red => 10
        Green => 20
        Blue => 30
    }
}

main :: fn () {
    red : Colour = Red
    score(red)
}
