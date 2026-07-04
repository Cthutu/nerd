pub VAO::plex{id GLuint}

impl VAO {
pub new::fn()->Option[Self]{
vao:GLuint
return on vao{0=>None else=>Some(Self{id:vao})}
}
}
¬
pub VAO :: plex {
    id GLuint
}

impl VAO {

    pub new :: fn () -> Option[Self] {
        vao : GLuint
        return on vao { 0 => None else => Some(Self { id: vao }) }
    }

}
