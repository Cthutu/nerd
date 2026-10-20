build{-- settings
 define:local
 windowed:no -- console
 on "windows"{library_path:$VULKAN_SDK/Lib}
 on !"local" { library_path: "other libs" }
}
main::fn(){}
¬
build {  -- settings
    define: local
    windowed: no  -- console
    on "windows" {
        library_path: $VULKAN_SDK/Lib
    }
    on !"local" {
        library_path: "other libs"
    }
}

main :: fn () {
}
¬
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
¬
answer::42
build{windowed:yes}
main::fn(){}
¬
answer :: 42

build {
    windowed: yes
}

main :: fn () {
}
¬
build -- block
{ -- opening
    -- leading
    define -- key
    : -- colon
    local -- value
    on -- on
    "windows" -- guard
    { -- open guard
        library_path: $VULKAN_SDK/Lib -- path
    } -- close guard
} -- end
main :: fn () {}
¬
-- block
build {  -- opening
    -- leading
    -- key
    -- colon
    define: local  -- value
    -- on
    -- guard
    on "windows" {  -- open guard
        library_path: $VULKAN_SDK/Lib  -- path
    }  -- close guard
}  -- end

main :: fn () {
}
