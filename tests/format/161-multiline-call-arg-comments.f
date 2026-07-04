main :: fn () {
    glVertexAttribPointer(0, -- attribute location
                          3, -- number of components (x, y, z)
                          GL_FLOAT, -- data type
                          GL_FALSE, -- normalised?
                          6 * f32.size.as(i32), -- stride (size of a single vertex in bytes)
                          0.as(^void)) -- offset (position starts at the beginning of the vertex)
}
¬
main :: fn () {
    glVertexAttribPointer(0,                     -- attribute location
                          3,                     -- number of components (x, y,
                                                 --     z)
                          GL_FLOAT,              -- data type
                          GL_FALSE,              -- normalised?
                          6 * f32.size.as(i32),  -- stride (size of a single
                                                 --     vertex in bytes)
                          0.as(^void))           -- offset (position starts at
                                                 --     the beginning of the
                                                 --     vertex)
}
