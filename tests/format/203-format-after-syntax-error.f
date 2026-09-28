broken::fn(){
    -- Keep this comment and malformed address expression.
    call(&value)
}
debug_callback :: fn (messageSeverity : VkDebugUtilsMessageSeverityFlagBitsEXT,
_messageType : VkDebugUtilsMessageTypeFlagsEXT,
callbackData : ^VkDebugUtilsMessengerCallbackDataEXT,
_userData : ^void) -> VkBool32 {
    on messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT => {
        prn($"Vulkan Debug: {callbackData.pMessage}")
    }
    return VK_FALSE
}
¬
broken :: fn () {
    -- Keep this comment and malformed address expression.
    call(& value)
}

debug_callback :: fn (messageSeverity : VkDebugUtilsMessageSeverityFlagBitsEXT,
                      _messageType    : VkDebugUtilsMessageTypeFlagsEXT,
                      callbackData    : ^VkDebugUtilsMessengerCallbackDataEXT,
                      _userData       : ^void) -> VkBool32 {
    on messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT => {
        prn($"Vulkan Debug: {callbackData.pMessage}")
    }
    return VK_FALSE
}
