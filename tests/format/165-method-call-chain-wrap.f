main :: fn () {
shader_program=Shader.new(vertex_shader_program,fragment_shader_program).expect("Failed to create shader program")
}
¬
main :: fn () {
    shader_program = Shader.new(vertex_shader_program, fragment_shader_program)
                           .expect("Failed to create shader program")
}
