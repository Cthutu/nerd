render_layer::fn(gfx:^GfxSystem,handle:GfxLayerHandle)->bool\GfxError{
layer:=gfx.get_pixel_layer(handle)?
layer.clear(0xff000000)
return yes
}
¬
render_layer :: fn (gfx: ^GfxSystem, handle: GfxLayerHandle) -> bool\GfxError {
    layer := gfx.get_pixel_layer(handle)?
    layer.clear(0xff000000)
    return yes
}
