use std.opengl

main :: fn () {
    framebuffer : GLuint = 0
    renderbuffer : GLuint = 0
    attachments : [2]GLenum = [GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1]

    glGenFramebuffers(1, ^framebuffer)
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer)
    glGenRenderbuffers(1, ^renderbuffer)
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer)
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 640, 480)
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                              GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER,
                              renderbuffer)
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           0,
                           0)
    glDrawBuffers(attachments.count.as(GLsizei), attachments[..].data)
    glDrawBuffer(GL_COLOR_ATTACHMENT0)
    glReadBuffer(GL_COLOR_ATTACHMENT0)
    _status := glCheckFramebufferStatus(GL_FRAMEBUFFER)

    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD)
    glBlendColor(0.0, 0.0, 0.0, 0.0)
    glBlendFuncSeparate(GL_SRC_ALPHA,
                        GL_ONE_MINUS_SRC_ALPHA,
                        GL_ONE,
                        GL_ONE_MINUS_SRC_ALPHA)
    glStencilFuncSeparate(GL_FRONT_AND_BACK, GL_ALWAYS, 0, 0xff)
    glStencilOpSeparate(GL_FRONT_AND_BACK, GL_KEEP, GL_KEEP, GL_REPLACE)
    glStencilMaskSeparate(GL_FRONT_AND_BACK, 0xff)
    glClearStencil(0)

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, 1, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, nil)
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, nil)
    glGenerateMipmap(GL_TEXTURE_3D)
    _mapped := glMapBufferRange(GL_ARRAY_BUFFER, 0, 16, GL_MAP_WRITE_BIT)
    glFlushMappedBufferRange(GL_ARRAY_BUFFER, 0, 16)

    glDisableVertexAttribArray(0)
    _is_array := glIsVertexArray(0)
    _extension := glGetStringi(GL_EXTENSIONS, 0)
}
¬
0
¬

¬
delete
¬

¬
check
