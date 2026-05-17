id :: fn[T](value:T)->T where T:Display{return value}
impl Box[T] where T:Display{get::fn(self:Self)->T{return self.value}}
¬
id :: fn [T] (value: T) -> T where T: Display {
    return value
}

impl Box[T] where T: Display {

    get :: fn (self: Self) -> T {
        return self.value
    }

}
