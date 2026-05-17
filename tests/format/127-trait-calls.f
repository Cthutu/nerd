main :: fn() {
point:=Point{x:1,y:2}
_:=point.show()
_:=Display.show(point)
_:=Default[Point].default()
_:=point.convert[i32](3)
}
¬
main :: fn () {
    point := Point { x: 1 y: 2 }
    _ := point.show()
    _ := Display.show(point)
    _ := Default[Point].default()
    _ := point.convert[i32](3)
}
