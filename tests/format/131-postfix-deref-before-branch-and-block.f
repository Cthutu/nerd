main :: fn () {
items := [["quit", "q"]]
for group in items {
for item in group^{
on item^=>"quit"
}
}
}
¬
main :: fn () {
    items := [["quit", "q"]]
    for group in items {
        for item in group^ {
            on item^ => "quit"
        }
    }
}
