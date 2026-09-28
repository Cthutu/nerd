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
