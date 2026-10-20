Stack::plex[T]{data [..]T}
impl[T] Stack[T]{pub push::fn(self:^Stack[T],item:T){self.data.push(item)} pub pop::fn(self:^Stack[T])->T{return self.data.pop()}}
¬
Stack :: plex [T] {
    data [..]T
}

impl [T] Stack[T] {

    pub push :: fn (self: ^Stack[T], item: T) {
        self.data.push(item)
    }

    pub pop :: fn (self: ^Stack[T]) -> T {
        return self.data.pop()
    }

}
¬
pub impl Display for Point{show::fn(self:Self)=>$"Point({self.x}, {self.y})"}
¬
pub impl Display for Point {

    show :: fn (self: Self) => $"Point({self.x}, {self.y})"

}
¬
main :: fn () {
shader_program=Shader.new(vertex_shader_program,fragment_shader_program).expect("Failed to create shader program")
}
¬
main :: fn () {
    shader_program = Shader.new(vertex_shader_program, fragment_shader_program)
                           .expect("Failed to create shader program")
}
