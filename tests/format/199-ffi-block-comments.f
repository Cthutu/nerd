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
