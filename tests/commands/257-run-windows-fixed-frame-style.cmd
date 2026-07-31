-- test-platform: windows
use std.frame
use os.windows

main :: fn () {
    system := FrameSystem.init()
    defer system.done()

    frame := Frame {
        system      : ^system
        id          : NEW_FRAME
        width       : 320
        height      : 240
        title       : "fixed frame style test"
        full_screen : no
        resizable   : no
    }
    system.apply(^frame)

    on frame.context() {
        context => {
            style := GetWindowLongPtr(context.window, GWL_STYLE).as(u32)
            assert (style & WS_THICKFRAME.as(u32)) == 0
            assert (style & WS_MAXIMIZEBOX.as(u32)) == 0
        }
        _error! => {
            assert no
        }
    }
}
¬
0
¬

¬
delete
¬
