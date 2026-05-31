A :: plex {
    x i32
}

B :: plex {
    y i32
}

impl A {

    pub init :: fn () -> Self {
        return { x: 1 }
    }

}

impl B {

    pub init :: fn () -> Self {
        return { y: 2 }
    }

}

main :: fn () -> i32 {
    a := A.init()
    b := B.init()
    return a.x + b.y
}
¬
3
¬

¬
