main :: fn () {
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
    indicies.count.as(GLsizeiptr),
    ^indicies,
    GL_STATIC_DRAW)
    on ok => {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indicies.count.as(GLsizeiptr),
        ^indicies,
        GL_STATIC_DRAW)
    }
}
¬
main :: fn () {
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indicies.count.as(GLsizeiptr),
                 ^indicies,
                 GL_STATIC_DRAW)
    on ok => {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     indicies.count.as(GLsizeiptr),
                     ^indicies,
                     GL_STATIC_DRAW)
    }
}
