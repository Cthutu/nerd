use std.io

libc::"c"
ffi libc abs(value:i32)->i32
ffi "c"{strlen(text:^u8)->usize write_line::puts(text:^u8)->i32
pub seed_rng::srand(seed:u32)

setlocale(category:i32,locale:^u8)->^u8
mmap(addr:^void,len:usize,prot:i32,flags:i32,fd:i32,offset:usize)->^void}
puts :: ffi "c" puts(text:^u8)
fcntl::ffi"c" fcntl(fd:i32,command:i32,...)->i32
syscall::intrinsic"syscall"(number:u64,a0:u64,a1:u64,a2:u64,a3:u64,a4:u64,a5:u64)->i64
pub ffi "c" public_puts(text:^i8)->i32
pub ffi "c"{public_strlen(text:^i8)->usize}

main::fn(){
value:=abs(-7)
prn($"abs = {value}")
}
¬
use std.io

libc :: "c"

ffi libc abs (value: i32) -> i32

ffi "c" {
                    strlen (text: ^u8) -> usize
    write_line   :: puts   (text: ^u8) -> i32
    pub seed_rng :: srand  (seed: u32)

    setlocale (category: i32, locale: ^u8) -> ^u8
    mmap      (addr   : ^void,
               len    : usize,
               prot   : i32,
               flags  : i32,
               fd     : i32,
               offset : usize) -> ^void
}

puts :: ffi "c" puts (text: ^u8)

fcntl :: ffi "c" fcntl (fd: i32, command: i32, ...) -> i32

syscall :: intrinsic "syscall" (number: u64, a0: u64, a1: u64, a2: u64, a3: u64, a4: u64, a5: u64) -> i64

pub ffi "c" public_puts (text: ^i8) -> i32

pub ffi "c" {
    public_strlen (text: ^i8) -> usize
}

main :: fn () {
    value := abs(-7)
    prn($"abs = {value}")
}
¬
ffi "c" puts (text: ^i8) -> i32

main :: fn () {
    puts(c"hello\n")
    text := "a\0b\tc"
}
¬
ffi "c" puts (text: ^i8) -> i32

main :: fn () {
    puts(c"hello\n")
    text := "a\0b\tc"
}
¬
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
    pub                    PostQuitMessage  (exit_code: i32)
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
¬
sum :: fn(count:i32,args:...)->i32 { return count+args.next[i32]() }
Callback :: fn(count: i32,args:...)->i32
¬
sum :: fn (count: i32, args: ...) -> i32 {
    return count + args.next[i32]()
}

Callback :: fn (count: i32, args: ...) -> i32
¬
forward :: fn (callback: fn (count: i32, args: ...) -> i32, args: ...) -> i32 {
    return callback(args.next[i32]())
}
¬
forward :: fn (callback: fn (count: i32, args: ...) -> i32, args: ...) -> i32 {
    return callback(args.next[i32]())
}
¬
ffi "vulkan" {
-- Create an instance.
pub vkCreateInstance(pCreateInfo:^VkInstanceCreateInfo,pAllocator:^void,pInstance:^VkInstance)->VkResult -- status
pub vkEnumeratePhysicalDevices(instance:VkInstance,pPhysicalDeviceCount:^u32,pPhysicalDevices:^VkPhysicalDevice)->VkResult
-- Only logical devices are destroyed here.
-- Physical devices remain owned by their instance.
pub vkDestroyDevice(device:VkDevice,pAllocator:^void)

-- Read the device properties.
pub vkGetPhysicalDeviceProperties(physicalDevice:VkPhysicalDevice,pProperties:^VkPhysicalDeviceProperties)
-- End of declarations.
}
on "linux" {
ffi "native" {
-- Renamed declaration.
pub create::nativeCreate(first_parameter:^void,second_parameter:^void,third_parameter:^void)->i32
-- Last comment.
}
}
ffi "empty" {
-- No declarations yet.
}

¬
ffi "vulkan" {
    -- Create an instance.
    pub vkCreateInstance           (pCreateInfo : ^VkInstanceCreateInfo,
                                    pAllocator  : ^void,
                                    pInstance   : ^VkInstance) -> VkResult  -- status
    pub vkEnumeratePhysicalDevices (instance             : VkInstance,
                                    pPhysicalDeviceCount : ^u32,
                                    pPhysicalDevices     : ^VkPhysicalDevice) -> VkResult

    -- Only logical devices are destroyed here.
    -- Physical devices remain owned by their instance.
    pub vkDestroyDevice (device: VkDevice, pAllocator: ^void)

    -- Read the device properties.
    pub vkGetPhysicalDeviceProperties (physicalDevice : VkPhysicalDevice,
                                       pProperties    : ^VkPhysicalDeviceProperties)
    -- End of declarations.
}

on "linux" {

    ffi "native" {
        -- Renamed declaration.
        pub create :: nativeCreate (first_parameter  : ^void,
                                    second_parameter : ^void,
                                    third_parameter  : ^void) -> i32
        -- Last comment.
    }

}

ffi "empty" {
    -- No declarations yet.
}
