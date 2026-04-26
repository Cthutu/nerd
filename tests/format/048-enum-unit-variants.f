Colour::enum{Red Green Blue}

score::fn(colour:Colour)->i32{
return on colour{
Red=>10
Green=>20
Blue=>30
}
}

main::fn(){
Red:Colour=Green
picked:Colour=Red
explicit:Colour=Colour.Red
score(picked)+score(explicit)
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
    Red      : Colour = Green
    picked   : Colour = Red
    explicit : Colour = Colour.Red

    score(picked) + score(explicit)
}
