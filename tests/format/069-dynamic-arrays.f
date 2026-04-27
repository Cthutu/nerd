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
