load::fn[T](value:^atomic[T],order::AtomicLoadOrder=SequentiallyConsistent)->T{return value^}
¬
load :: fn [T] (value : ^atomic[T],
                order :: AtomicLoadOrder = SequentiallyConsistent) -> T {
    return value^
}
