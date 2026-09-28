broken :: fn () { call(&value) }
build{on "windows"{library_path:$VULKAN_SDK/Lib} define:local}
debug_callback::fn(a:i32,b:i32)->i32{return a+b}
¬
broken :: fn () {
    call(& value)
}

build {
    on "windows" {
        library_path: $VULKAN_SDK/Lib
    }
    define: local
}

debug_callback :: fn (a: i32, b: i32) -> i32 {
    return a + b
}
