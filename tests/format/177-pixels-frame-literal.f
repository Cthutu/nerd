main :: fn () {
    frame := Frame {
        id : NEW_FRAME
        width : 640
        height : 480
        title : "Pixels"
        full_screen : no
        resizable : yes
        ...
    }
    defer frame_system.done();
}
¬
main :: fn () {
    frame := Frame {
        id          : NEW_FRAME
        width       : 640
        height      : 480
        title       : "Pixels"
        full_screen : no
        resizable   : yes
        ...
    }
    defer frame_system.done()
}
