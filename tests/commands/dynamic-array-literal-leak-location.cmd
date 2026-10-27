Location :: plex { mobs [..]i32 }
World :: plex { locations [..]Location }
make :: fn () -> World {
    locations := [
        Location {
            mobs: [1, 2, 3, 4, 5, 6, 7]
        },
    ]
    return { locations }
}
main :: fn () {
    world := make()
    _direct: [..]i32 = [8, 9]
    _empty: [..]i32 = []
    world.locations.free()
}
¬
0
¬
¬
delete
¬
--llvm
¬
run
¬
nrt: memory leaks detected
  type  index bytes  location
  heap      4    32  dynamic-array-literal-leak-location.input.n:14
  heap      3    40  dynamic-array-literal-leak-location.input.n:13
  heap      1    60  dynamic-array-literal-leak-location.input.n:6
nrt: total 3 heap leaks, 132 bytes; 0 arena leaks, 0 bytes committed

