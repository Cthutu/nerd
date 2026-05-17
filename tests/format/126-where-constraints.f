id :: fn[T](value:T)->T where T:Display{return value}
impl Box[T] where T:Display{get::fn(self:Self)->T{return self.value}}
combine :: fn[Short,Longer](left:Short,right:Longer)->string where Short:Display,Longer:Debug{return left.show()}
impl Pair[Short,Longer] where Short:Display,Longer:Debug{first::fn(self:Self)->Short{return self.left}}
¬
id :: fn [T] (value: T) -> T
where T: Display {
    return value
}

impl Box[T]
where T: Display {

    get :: fn (self: Self) -> T {
        return self.value
    }

}

combine :: fn [Short, Longer] (left  : Short,
                               right : Longer) -> string
where Short : Display,
      Longer: Debug {
    return left.show()
}

impl Pair[Short, Longer]
where Short : Display,
      Longer: Debug {

    first :: fn (self: Self) -> Short {
        return self.left
    }

}
