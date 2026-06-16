main :: fn () {
text := """alpha
beta"""
escaped := "alpha\n  beta\n"
}

plain_shader :: ""
vertex_shader ::
"""
#version 330 core
void main() {
}
"""

typed_shader: string: """alpha
beta"""
¬
main :: fn () {
    text := """
        alpha
        beta
        """
    escaped := """
        alpha
          beta
        """
}

plain_shader  :: ""
vertex_shader :: """
    #version 330 core
    void main() {
    }
    """

typed_shader : string : """
    alpha
    beta
    """
