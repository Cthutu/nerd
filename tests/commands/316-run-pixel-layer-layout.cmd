use std.frame
use std.gfx

main :: fn () -> i32 {
    frame := Frame { id: NEW_FRAME width: 640 height: 480 ... }
    gfx := GfxSystem.init()
    defer gfx.done()
    _handle := gfx.create_pixel_layer(^frame, PixelLayerMode.FixedSizeAutoScale {
        width: 160 height: 120
    })
    layer := ^gfx.pixel_layers[0]
    layer.clear(0xff123456)
    on layer.pixels().count != 19200 => return 2
    on layer.pixels()[19199] != 0xff123456 => return 3
    return 0
}
¬
0
¬
