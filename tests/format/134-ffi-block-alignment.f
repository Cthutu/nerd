ffi "user32" { pub LoadCursor::LoadCursorA(instance:HINSTANCE,cursor_name:^u8)->HCURSOR
pub RegisterClassEx::RegisterClassExA(wnd_class:^WNDCLASSEXA)->u16
pub DefWindowProc::DefWindowProcA(hwnd:HWND,msg:u32,wParam:WPARAM,lParam:LPARAM)->LRESULT
pub PostQuitMessage(exit_code:i32)
pub CreateWindowEx::CreateWindowExA(ex_style:u32,class_name:LPCSTR,window_name:LPCSTR,style:u32,x:i32,y:i32,width:i32,height:i32,parent:HWND,menu:HMENU,instance:HINSTANCE,param:^void)->HWND}
¬
ffi "user32" {
    pub LoadCursor      :: LoadCursorA      (instance    : HINSTANCE,
                                             cursor_name : ^u8) -> HCURSOR
    pub RegisterClassEx :: RegisterClassExA (wnd_class: ^WNDCLASSEXA) -> u16
    pub DefWindowProc   :: DefWindowProcA   (hwnd   : HWND,
                                             msg    : u32,
                                             wParam : WPARAM,
                                             lParam : LPARAM) -> LRESULT
    pub PostQuitMessage                     (exit_code: i32)
    pub CreateWindowEx  :: CreateWindowExA  (ex_style    : u32,
                                             class_name  : LPCSTR,
                                             window_name : LPCSTR,
                                             style       : u32,
                                             x           : i32,
                                             y           : i32,
                                             width       : i32,
                                             height      : i32,
                                             parent      : HWND,
                                             menu        : HMENU,
                                             instance    : HINSTANCE,
                                             param       : ^void) -> HWND
}
