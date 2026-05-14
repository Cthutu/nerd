main :: fn () {
    wndclass :: WNDCLASSA {
        hInstance: instance
        lpszClassName: c"HandmadeHeroWindowClass"
        ...
    }

    point := Point { x: 1, y: 2, ... }
}
¬
main :: fn () {
    wndclass :: WNDCLASSA {
        hInstance     : instance
        lpszClassName : c"HandmadeHeroWindowClass"
        ...
    }

    point := Point { x: 1 y: 2 ... }
}
