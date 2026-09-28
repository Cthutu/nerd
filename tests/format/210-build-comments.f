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
