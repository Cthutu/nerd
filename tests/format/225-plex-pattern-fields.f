main::fn(){
on point{
Point{x:for wanted,y:height,name:_}as matched=>height
{x:0,...}=>point.y
{...,}=>0
}
on nested{
{before:1,point:{x:for (1+1),...},after:last}=>last
}
}
¬
main :: fn () {
    on point {
        Point { x: for wanted, y: height, name: _ } as matched  => height
        { x: 0, ... }                                           => point.y
        { ... }                                                 => 0
    }
    on nested {
        { before: 1, point: { x: for (1 + 1), ... }, after: last } => last
    }
}
