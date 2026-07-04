pub Shader :: plex {
    id GLuint
}

impl Shader {

    pub new :: fn (vertex_shader : string,
    fragment_shader : string) -> Option[Self] {
        return Some(Self {
            id: 1
        }
        )
    }
}
¬
pub Shader :: plex {
    id GLuint
}

impl Shader {

    pub new :: fn (vertex_shader   : string,
                   fragment_shader : string) -> Option[Self] {
        return Some(Self {
            id: 1
        })
    }

}
