gfx :: use std.gfx

main :: fn () {
    fs : gfx.FrameSystem

    frame := gfx.Frame {
        handle: 0
        system: nil
        title: ""
        x: 0
        y: 0
        width: 0
        height: 0
        resizable: no
        fullscreen: no
    }
}
¬
gfx :: use std.gfx

main :: fn () {
    fs : gfx.FrameSystem

    frame := gfx.Frame {
        handle    : 0
        system    : nil
        title     : ""
        x         : 0
        y         : 0
        width     : 0
        height    : 0
        resizable : no
        fullscreen: no
    }
}
