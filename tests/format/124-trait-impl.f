Display :: trait{show::fn(Self)->string}
Point :: plex{x i32 y i32}
impl Display for Point{show::fn(self:Self)=>$"Point({self.x}, {self.y})"}
¬
Display :: trait {
    show :: fn (Self) -> string
}

Point :: plex {
    x i32
    y i32
}

impl Display for Point {

    show :: fn (self: Self) => $"Point({self.x}, {self.y})"

}
