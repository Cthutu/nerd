Stack::plex[T]{data [..]T}
impl Stack[T]{pub push::fn(self:^Stack[T],item:T){self.data.push(item)} pub pop::fn(self:^Stack[T])->T{return self.data.pop()}}
¬
Stack :: plex [T] {
    data [..]T
}

impl Stack[T] {

    pub push :: fn (self: ^Stack[T], item: T) {
        self.data.push(item)
    }


    pub pop :: fn (self: ^Stack[T]) -> T {
        return self.data.pop()
    }

}
