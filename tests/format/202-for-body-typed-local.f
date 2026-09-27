VulkanContext :: plex {
frame ^Frame
instance VkInstance
physical_device VkPhysicalDevice
}
choose :: fn () {
for device in devices {
properties: VkPhysicalDeviceProperties
vkGetPhysicalDeviceProperties(device^, ^properties)
}
for device in devices {
}
for device in devices $search {
properties: VkPhysicalDeviceProperties
use_properties(^properties)
}
}

¬
VulkanContext :: plex {
    frame           ^Frame
    instance        VkInstance
    physical_device VkPhysicalDevice
}

choose :: fn () {
    for device in devices {
        properties : VkPhysicalDeviceProperties
        vkGetPhysicalDeviceProperties(device^, ^properties)
    }
    for device in devices {
    }
    for device in devices $search {
        properties : VkPhysicalDeviceProperties
        use_properties(^properties)
    }
}
