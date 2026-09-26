use std.frame
use std.vulkan

main :: fn () -> i32 {
    marker: i32 = 0
    -- These invalid-frame paths must return before calling Vulkan.
    instance: VkInstance = ^marker
    surface: VkSurfaceKHR = instance
    on vk_create_surface(instance, nil, nil) != VK_ERROR_INITIALIZATION_FAILED => return 1
    on vk_create_surface(nil, nil, ^surface) != VK_ERROR_INITIALIZATION_FAILED => return 2
    on surface != nil => return 3
    surface = instance
    on vk_create_surface(instance, nil, ^surface) != VK_ERROR_INITIALIZATION_FAILED => return 4
    on surface != nil => return 5
    frame := Frame { id: NEW_FRAME, ... }
    surface = instance
    on vk_create_surface(instance, ^frame, ^surface) != VK_ERROR_INITIALIZATION_FAILED => return 6
    on surface != nil => return 7
    system := FrameSystem.init()
    defer system.done()
    frame.system = ^system
    frame.id = CLOSED_FRAME
    surface = instance
    on vk_create_surface(instance, ^frame, ^surface) != VK_ERROR_INITIALIZATION_FAILED => return 8
    on surface != nil => return 9
    prn("Invalid surface inputs leave a null handle")
    return 0
}
¬
0
¬
Invalid surface inputs leave a null handle

¬
delete
¬
