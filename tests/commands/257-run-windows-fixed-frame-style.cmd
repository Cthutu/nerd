-- test-platform: windows
use std.frame
use os.windows

check_tracking :: fn (window: HWND, fixed: bool) {
    limits : MINMAXINFO
    limits.ptMinTrackSize.x = 1
    limits.ptMinTrackSize.y = 2
    limits.ptMaxTrackSize.x = 10000
    limits.ptMaxTrackSize.y = 10001
    _ := SendMessage(window, WM_GETMINMAXINFO, 0, (^limits).as(isize))
    on fixed => {
        rect : RECT
        assert GetWindowRect(window, ^rect) != 0
        assert limits.ptMinTrackSize.x == rect.right - rect.left
        assert limits.ptMinTrackSize.y == rect.bottom - rect.top
        assert limits.ptMaxTrackSize.x == limits.ptMinTrackSize.x
        assert limits.ptMaxTrackSize.y == limits.ptMinTrackSize.y
    } else {
        assert limits.ptMinTrackSize.x == 1
        assert limits.ptMinTrackSize.y == 2
        assert limits.ptMaxTrackSize.x == 10000
        assert limits.ptMaxTrackSize.y == 10001
    }
}

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
            check_tracking(context.window, yes)

            -- Native sizing honours the limits without needing a WM_SIZE undo.
            _ := SetWindowPos(context.window, nil, 0, 0, 640, 480,
                              SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE)
            rect : RECT
            assert GetClientRect(context.window, ^rect) != 0
            assert rect.right - rect.left == 320
            assert rect.bottom - rect.top == 240

            frame.width = 400
            frame.height = 300
            system.apply(^frame)
            assert GetClientRect(context.window, ^rect) != 0
            assert rect.right - rect.left == 400
            assert rect.bottom - rect.top == 300
            check_tracking(context.window, yes)

            _ := ShowWindow(context.window, SW_MINIMIZE)
            _ := ShowWindow(context.window, SW_RESTORE)
            assert GetClientRect(context.window, ^rect) != 0
            assert rect.right - rect.left == 400
            assert rect.bottom - rect.top == 300
            check_tracking(context.window, yes)

            frame.resizable = yes
            system.apply(^frame)
            check_tracking(context.window, no)
            frame.resizable = no
            system.apply(^frame)
            check_tracking(context.window, yes)

            frame.full_screen = yes
            system.apply(^frame)
            check_tracking(context.window, no)
            assert GetClientRect(context.window, ^rect) != 0
            assert rect.right - rect.left == GetSystemMetrics(SM_CXSCREEN)
            assert rect.bottom - rect.top == GetSystemMetrics(SM_CYSCREEN)
            frame.full_screen = no
            system.apply(^frame)
            check_tracking(context.window, yes)
            assert GetClientRect(context.window, ^rect) != 0
            assert rect.right - rect.left == 400
            assert rect.bottom - rect.top == 300
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
