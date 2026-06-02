main :: fn () {
    frame := Frame {
        id : NEW_FRAME
        width : 800
        height : 600
        title : "OpenGL Triangle"
        full_screen : no
        resizable : yes
        ...
    }

    on scan_code {
        Escape, Q => {
            close()
        }
    }
}
¬
main :: fn () {
    frame := Frame {
        id          : NEW_FRAME
        width       : 800
        height      : 600
        title       : "OpenGL Triangle"
        full_screen : no
        resizable   : yes
        ...
    }

    on scan_code {
        Escape, Q => {
            close()
        }
    }
}
