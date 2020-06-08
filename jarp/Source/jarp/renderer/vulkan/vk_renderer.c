#include "vk_renderer.h"

#include <stb_image.h>
#include <volk.h>

#include "jarp/file.h"
#include "jarp/log.h"
#include "jarp/shared.h"
#include "jarp/math/mat4.h"
#include "jarp/math/math.h"
#include "jarp/math/vec2.h"
#include "jarp/math/vec3.h"

#ifdef VK_USE_PLATFORM_WIN32_KHR
#include "jarp/platform/win32/win32_main.h"
#endif

#include <stdlib.h>
#include <string.h>

// TODO: proper handling of VkResult return values

void record_command_buffer(void);

struct Vertex {
    Vec3f position;
    Vec3f normal;
    Vec3f color;
    Vec2f texture_coordinate;
};
struct UniformBufferObject {
    Mat4f Model;
    Mat4f View;
    Mat4f Projection;
    Vec3f LightPosition;
};

#if 0
struct Vertex model_vertices[] = {
    {{ -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }},     // 0
    {{  0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f }},     // 1
    {{  0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }},     // 2
    {{ -0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f }},     // 3

    {{ -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }},    // 4
    {{  0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f }},    // 5
    {{  0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f }},    // 6
    {{ -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f }}     // 7
};
uint32_t model_indices[] = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4
};
#else
struct Vertex model_vertices[] = {
    {{  0.0f,  0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }},     // 0
    {{ -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f }},     // 1
    {{  0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }},     // 2
};
uint32_t model_indices[] = {
    0, 1, 2
};
#endif

struct UniformBufferObject uniform_buffer_object = { 0 };

bool use_vsync = false;

// ===============

VkPhysicalDeviceMemoryProperties physical_device_memory_properties = { 0 };

uint32_t graphics_family_index = -1;
//uint32_t compute_family_index = -1;
//uint32_t transfer_family_index = -1;

uint32_t swapchain_image_count = 0;
VkExtent2D swapchain_extent = { 0 };
VkFormat depth_format = { 0 };

VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_infos[2] = { 0 };

uint32_t active_image_index = 0;
uint32_t current_frame = 0;
#define MAX_FRAMES_IN_FLIGHT 2

// ===============

// TODO: all object handles that are based on swapchain_image_count can probably not be checked
//       against VK_NULL_HANDLE at destruction

VkDebugUtilsMessengerEXT debug_utils = VK_NULL_HANDLE;
VkInstance instance = VK_NULL_HANDLE;
VkSurfaceKHR surface = VK_NULL_HANDLE;
VkPhysicalDevice physical_device = VK_NULL_HANDLE;
VkDevice device = VK_NULL_HANDLE;
VkQueue graphics_queue = VK_NULL_HANDLE;
VkQueue present_queue = VK_NULL_HANDLE;
VkSwapchainKHR swapchain = VK_NULL_HANDLE;
VkImageView* image_views = NULL;
VkImage depth_image = VK_NULL_HANDLE;
VkDeviceMemory depth_image_device_memory = VK_NULL_HANDLE;
VkImageView depth_image_view = VK_NULL_HANDLE;
VkRenderPass render_pass = VK_NULL_HANDLE;
VkDescriptorSetLayout descriptor_set_layouts[1] = { VK_NULL_HANDLE }; // if the amount of layouts ever increases, the descriptor sets also have to get updated
VkShaderModule shader_modules[2] = { VK_NULL_HANDLE };
VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
VkPipeline pipeline = VK_NULL_HANDLE;

VkFramebuffer* framebuffers = NULL;
VkCommandPool command_pool = VK_NULL_HANDLE;
VkCommandBuffer* command_buffers = NULL;
VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;

VkBuffer vertex_buffer = VK_NULL_HANDLE;
VkDeviceMemory vertex_buffer_memory = VK_NULL_HANDLE;
VkBuffer index_buffer = VK_NULL_HANDLE;
VkDeviceMemory index_buffer_memory = VK_NULL_HANDLE;
VkBuffer uniform_buffers[4] = { VK_NULL_HANDLE }; // TODO: base on swapchain_image_count
VkDeviceMemory uniform_buffer_memories[4] = { VK_NULL_HANDLE }; // TODO: base on swapchain_image_count
VkDescriptorSet* descriptor_sets = NULL;
VkSemaphore rendering_finished_semaphores[MAX_FRAMES_IN_FLIGHT] = { VK_NULL_HANDLE };
VkSemaphore image_available_semaphores[MAX_FRAMES_IN_FLIGHT] = { VK_NULL_HANDLE };
VkFence fences_in_flight[MAX_FRAMES_IN_FLIGHT] = { VK_NULL_HANDLE };
VkFence images_in_flight[4] = { VK_NULL_HANDLE }; // TODO: base on swapchain_image_count

VkSampler texture_sampler = VK_NULL_HANDLE;
VkImage texture_image = VK_NULL_HANDLE;
VkDeviceMemory texture_image_memory = VK_NULL_HANDLE;
VkImageView texture_image_view = VK_NULL_HANDLE;

/*
====================
debug_callback
====================
*/
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                     VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                     const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                     void* pUserData) {
    const char* type;
    if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
        type = "GENERAL";
    } else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
        type = "VALIDATION";
    } else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
        type = "PERFORMANCE";
    }

    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        log_debug("VERBOSE | %s - Validation layer: %s", type, pCallbackData->pMessage);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        log_debug("INFO | %s - Validation layer: %s", type, pCallbackData->pMessage);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        log_debug("WARNING | %s - Validation layer: %s", type, pCallbackData->pMessage);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        log_debug("ERROR | %s - Validation layer: %s", type, pCallbackData->pMessage);
    }

    return VK_FALSE;
}

/*
====================
get_memory_type_index
====================
*/
uint32_t get_memory_type_index(VkMemoryRequirements memory_requirements, VkMemoryPropertyFlags desired_memory_properties) {
    for (uint32_t i = 0; i < physical_device_memory_properties.memoryTypeCount; i++) {
        // Pick the first found memory type that supports the memory type bits
        if ((memory_requirements.memoryTypeBits & (1 << i)) &&
            ((physical_device_memory_properties.memoryTypes[i].propertyFlags & desired_memory_properties) == desired_memory_properties)) {
            return i;
        }
    }
    return UINT32_MAX;
}

/*
====================
create_image_with_view
====================
*/
bool create_2d_image_with_view(VkImage* image, VkDeviceMemory* image_memory, VkImageView* image_view,
                               VkFormat format, VkExtent2D extent, VkImageUsageFlags usage_flags, VkImageAspectFlags aspect_flags) {
    VkImageCreateInfo image_create_info = { 0 };
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_create_info.pNext = NULL;
    image_create_info.flags = 0;
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.format = format;
    image_create_info.extent.width = extent.width;
    image_create_info.extent.height = extent.height;
    image_create_info.extent.depth = 1;
    image_create_info.mipLevels = 1;
    image_create_info.arrayLayers = 1;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT; // TODO: Change this in case of multisampling support
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_create_info.usage = usage_flags;
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_create_info.queueFamilyIndexCount = 0;
    image_create_info.pQueueFamilyIndices = NULL;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    vkCreateImage(device, &image_create_info, NULL, image);

    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(device, *image, &memory_requirements);

    uint32_t memory_type_index = get_memory_type_index(memory_requirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (memory_type_index == UINT32_MAX) {
        log_fatal("Failed to find a suitable memory type for depth image");
        return false;
    }

    VkMemoryAllocateInfo memory_allocate_info = { 0 };
    memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocate_info.pNext = NULL;
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = memory_type_index;

    vkAllocateMemory(device, &memory_allocate_info, NULL, image_memory);
    vkBindImageMemory(device, *image, *image_memory, 0);

    VkImageViewCreateInfo image_view_create_info = { 0 };
    image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.pNext = NULL;
    image_view_create_info.flags = 0;
    image_view_create_info.image = *image;
    image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_create_info.format = format;
    image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.subresourceRange.aspectMask = aspect_flags;
    image_view_create_info.subresourceRange.baseMipLevel = 0;
    image_view_create_info.subresourceRange.levelCount = 1;
    image_view_create_info.subresourceRange.baseArrayLayer = 0;
    image_view_create_info.subresourceRange.layerCount = 1;

    vkCreateImageView(device, &image_view_create_info, NULL, image_view);

    return true;
}

/*
====================
create_buffer
====================
*/
bool create_buffer(VkBuffer* buffer, VkDeviceMemory* buffer_memory, VkDeviceSize buffer_size, VkBufferUsageFlags buffer_usage_flags, VkMemoryPropertyFlags memory_properties) {
    VkBufferCreateInfo buffer_create_info = { 0 };
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.pNext = NULL;
    buffer_create_info.flags = 0;
    buffer_create_info.size = buffer_size;
    buffer_create_info.usage = buffer_usage_flags;
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buffer_create_info.queueFamilyIndexCount = 0; // needed if the buffer is shared between multiple different queues
    buffer_create_info.pQueueFamilyIndices = NULL;

    vkCreateBuffer(device, &buffer_create_info, NULL, buffer);

    VkMemoryRequirements buffer_memory_requirements;
    vkGetBufferMemoryRequirements(device, *buffer, &buffer_memory_requirements);
    uint32_t buffer_memory_type_index = get_memory_type_index(buffer_memory_requirements, memory_properties);
    if (buffer_memory_type_index == UINT32_MAX) {
        log_fatal("Could  not find a suitable memory type for the index buffer");
        return false;
    }

    VkMemoryAllocateInfo buffer_memory_allocate_info = { 0 };
    buffer_memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    buffer_memory_allocate_info.pNext = NULL;
    buffer_memory_allocate_info.allocationSize = buffer_memory_requirements.size;
    buffer_memory_allocate_info.memoryTypeIndex = buffer_memory_type_index;

    vkAllocateMemory(device, &buffer_memory_allocate_info, NULL, buffer_memory);
    vkBindBufferMemory(device, *buffer, *buffer_memory, 0);

    return true;
}

/*
====================
end_one_time_submit_command
====================
*/
VkCommandBuffer begin_one_time_submit_command(VkCommandPool* command_pool) {
    VkCommandBufferAllocateInfo command_buffer_allocate_info = { 0 };
    command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_allocate_info.pNext = NULL;
    command_buffer_allocate_info.commandPool = *command_pool;
    command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_allocate_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer = VK_NULL_HANDLE;
    vkAllocateCommandBuffers(device, &command_buffer_allocate_info, &command_buffer);

    VkCommandBufferBeginInfo command_buffer_begin_info = { 0 };
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.pNext = NULL;
    command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    command_buffer_begin_info.pInheritanceInfo = NULL;

    vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);

    return command_buffer;
}

/*
====================
end_one_time_submit_command
====================
*/
void end_one_time_submit_command(VkCommandBuffer* command_buffer, VkCommandPool* command_pool) {
    vkEndCommandBuffer(*command_buffer);

    VkSubmitInfo submitInfo = { 0 };
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = NULL;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = NULL;
    submitInfo.pWaitDstStageMask = NULL;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = command_buffer;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = NULL;

    vkQueueSubmit(graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphics_queue);

    vkFreeCommandBuffers(device, *command_pool, 1, command_buffer);
}

/*
====================
upload_buffer
====================
*/
bool upload_buffer(VkBuffer buffer, VkDeviceSize buffer_size, void* data) {
    // Create staging buffer and copy data to GPU
    VkBuffer staging_buffer = VK_NULL_HANDLE;
    VkDeviceMemory staging_memory = VK_NULL_HANDLE;
    if (!create_buffer(&staging_buffer, &staging_memory, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
        return false;
    }

    void* raw_data = NULL;
    vkMapMemory(device, staging_memory, 0, buffer_size, 0, &raw_data);
    memcpy(raw_data, data, (size_t)buffer_size);
    vkUnmapMemory(device, staging_memory);

    // Execute copy command
    VkCommandBuffer command_buffer = begin_one_time_submit_command(&command_pool);

    VkBufferCopy buffer_copy = { 0 };
    buffer_copy.srcOffset = 0;
    buffer_copy.dstOffset = 0;
    buffer_copy.size = buffer_size;

    vkCmdCopyBuffer(command_buffer, staging_buffer, buffer, 1, &buffer_copy);

    end_one_time_submit_command(&command_buffer, &command_pool);

    vkFreeMemory(device, staging_memory, NULL);
    vkDestroyBuffer(device, staging_buffer, NULL);

    return true;
}

/*
====================
transition_image_layout
====================
*/
void transition_image_layout(VkCommandPool* command_pool, VkImage* image, VkImageLayout old_layout, VkImageLayout new_layout) {
    VkCommandBuffer transient_command_buffer = begin_one_time_submit_command(command_pool);

    VkPipelineStageFlags source_stage = 0;
    VkPipelineStageFlags destination_stage = 0;

    VkImageMemoryBarrier image_memory_barrier = { 0 };
    image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    image_memory_barrier.pNext = NULL;

    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED) {
        source_stage = VK_PIPELINE_STAGE_HOST_BIT;
        image_memory_barrier.srcAccessMask = 0;
        if (new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        } else if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            destination_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            image_memory_barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        }
    } else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        if (new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        }
    }
    
    image_memory_barrier.oldLayout = old_layout;
    image_memory_barrier.newLayout = new_layout;
    image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.image = *image;

    if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        if (depth_format == VK_FORMAT_D32_SFLOAT_S8_UINT || depth_format == VK_FORMAT_D24_UNORM_S8_UINT) {
            image_memory_barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    } else {
        image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    image_memory_barrier.subresourceRange.baseMipLevel = 0;
    image_memory_barrier.subresourceRange.levelCount = 1;
    image_memory_barrier.subresourceRange.baseArrayLayer = 0;
    image_memory_barrier.subresourceRange.layerCount = 1;

    vkCmdPipelineBarrier(transient_command_buffer, source_stage, destination_stage, 0, 0, NULL, 0, NULL, 1, &image_memory_barrier);

    end_one_time_submit_command(&transient_command_buffer, command_pool);
}

/*
====================
vk_renderer_init
====================
*/
bool vk_renderer_init(void* window, char* application_path) {
    if (volkInitialize() != VK_SUCCESS) {
        log_fatal("Failed initializing volk");
        return false;
    }

    // ===============
    // Instance
    // ===============
    {
        const char* instance_extensions[] = {
            VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef VK_USE_PLATFORM_WIN32_KHR
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
#ifdef _DEBUG
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
        };

        const char* instance_layers[] = {
            "VK_LAYER_KHRONOS_validation",
            //"VK_LAYER_LUANRG_standard_validation" // TODO: which of those two?
        };

        VkApplicationInfo application_info = { 0 };
        application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        application_info.pNext = NULL;
        application_info.pApplicationName = NULL;
        application_info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
        application_info.pEngineName = "jarp";
        application_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);
        application_info.apiVersion = VK_API_VERSION_1_2; // TODO: check if this version is available

        VkInstanceCreateInfo instance_create_info = { 0 };
        instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_create_info.pNext = NULL;
        instance_create_info.flags = 0;
        instance_create_info.pApplicationInfo = &application_info;
        instance_create_info.enabledLayerCount = ARRAY_COUNT(instance_layers);
        instance_create_info.ppEnabledLayerNames = instance_layers;
        instance_create_info.enabledExtensionCount = ARRAY_COUNT(instance_extensions);
        instance_create_info.ppEnabledExtensionNames = instance_extensions;

        if (vkCreateInstance(&instance_create_info, NULL, &instance) != VK_SUCCESS) {
            log_fatal("Failed to create vulkan instance");
            return false;
        }
        volkLoadInstance(instance);
    }

    // ===============
    // Debug Callback
    // ===============
    {
        VkDebugUtilsMessengerCreateInfoEXT create_info = { 0 };
        create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        create_info.pNext = NULL;
        create_info.flags = 0;
        create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        create_info.pfnUserCallback = debug_callback;
        create_info.pUserData = NULL;

        vkCreateDebugUtilsMessengerEXT(instance, &create_info, NULL, &debug_utils);
    }
    
    // ===============
    // Surface
    // ===============
    {
#if VK_USE_PLATFORM_WIN32_KHR
        VkWin32SurfaceCreateInfoKHR surface_create_info = { 0 };
        surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surface_create_info.pNext = NULL;
        surface_create_info.flags = 0;
        surface_create_info.hinstance = ((struct Win32Window*)window)->hinstance;
        surface_create_info.hwnd = ((struct Win32Window*)window)->hwnd;

        vkCreateWin32SurfaceKHR(instance, &surface_create_info, NULL, &surface);
#endif
    }

    // ===============
    // Device
    // ===============
    {
        const char* device_extensions[] = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        uint32_t physical_device_count = 0;
        vkEnumeratePhysicalDevices(instance, &physical_device_count, NULL);
        VkPhysicalDevice* physical_devices = (VkPhysicalDevice*)malloc(physical_device_count * sizeof(VkPhysicalDevice));
        vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices);
        {
            // TODO: properly check queue families, device extensions, device features, swapchain and surface support
            //       and bundle it here
            // Get info about the available physical devices and pick one for use
            for (uint32_t i = 0; i < physical_device_count; i++) {
                VkPhysicalDeviceProperties physical_device_properties = { 0 };
                uint32_t queueFamilyCount;
                vkGetPhysicalDeviceProperties(physical_devices[i], &physical_device_properties);
                vkGetPhysicalDeviceMemoryProperties(physical_devices[i], &physical_device_memory_properties);
                vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[i], &queueFamilyCount, NULL);
                if (queueFamilyCount < 1)
                    continue;

                uint32_t property_count = 0;
                vkEnumerateDeviceExtensionProperties(physical_devices[i], NULL, &property_count, NULL);
                if (property_count > 0) {
                    VkExtensionProperties* extension_properties = (VkExtensionProperties*)malloc(property_count * sizeof(VkExtensionProperties));
                    vkEnumerateDeviceExtensionProperties(physical_devices[i], NULL, &property_count, extension_properties);

                    bool extensions_supported = false;
                    for (uint32_t i = 0; i < property_count; i++) {
                        if (strcmp(extension_properties[i].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
                            extensions_supported = true;
                            break;
                        }
                    }
                    free(extension_properties);
                    if (!extensions_supported) {
                        continue;
                    }
                }

                physical_device = physical_devices[i];
                if (physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                    break;
                }
            }
        }
        free(physical_devices);

        if (physical_device == VK_NULL_HANDLE) {
            log_fatal("Could not find a suitable physical device");
            return false;
        }

        // Find a supported depth format
        VkFormat formats[] = {
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D24_UNORM_S8_UINT
        };
        for (uint32_t i = 0; i < 3; i++) {
            VkFormatProperties format_properties;
            vkGetPhysicalDeviceFormatProperties(physical_device, formats[i], &format_properties);

            //if ((image_tiling == VK_IMAGE_TILING_LINEAR && (format_properties.linearTilingFeatures & format_feature_flags) == format_feature_flags) ||
            //    (image_tiling == VK_IMAGE_TILING_OPTIMAL && (format_properties.optimalTilingFeatures & format_feature_flags) == format_feature_flags)) {
            //    return formats[i];
            //}

            if ((format_properties.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT ||
                (format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
                depth_format = formats[i];
            }
        }

        // TODO: check support and enable desired features
        VkPhysicalDeviceFeatures device_features = { 0 };
        device_features.samplerAnisotropy = VK_TRUE;
        VkPhysicalDeviceFeatures physical_device_features = { 0 };
        vkGetPhysicalDeviceFeatures(physical_device, &physical_device_features);
        if (physical_device_features.samplerAnisotropy == VK_FALSE) {
            log_error("Not all desired features are enabled");
            return false;
        }

        // Pick a queue
        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, NULL);
        VkQueueFamilyProperties* queue_family_properties = (VkQueueFamilyProperties*)malloc(queue_family_count * sizeof(VkQueueFamilyProperties));
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_family_properties);

        VkDeviceQueueCreateInfo device_queue_create_infos[1];
        uint32_t number_of_queue_priorities = 0;

        for (uint32_t i = 0; i < queue_family_count; i++) {
            bool is_valid_queue = false;

            if (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                if (graphics_family_index == -1) {
                    graphics_family_index = i;
                    is_valid_queue = true;
                }
            }
            //if (queue_family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            //    if (compute_family_index == -1) {
            //        compute_family_index = i;
            //        is_valid_queue = true;
            //    }
            //}
            //if (queue_family_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            //    if (transfer_family_index == -1) {
            //        transfer_family_index = i;
            //        is_valid_queue = true;
            //    }
            //}

            if (!is_valid_queue) {
                continue;
            }
            else {
                float queue_priority = 1.0f;

                device_queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                device_queue_create_infos[i].pNext = NULL;
                device_queue_create_infos[i].flags = 0;
                device_queue_create_infos[i].queueFamilyIndex = i;
                device_queue_create_infos[i].queueCount = 1;
                device_queue_create_infos[i].pQueuePriorities = &queue_priority;

                break;
            }
        }

        if (graphics_family_index == -1) {
            log_fatal("Could not find a graphics queue");
            return false;
        }

        // Create the actual device
        VkDeviceCreateInfo device_create_info = { 0 };
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.pNext = NULL;
        device_create_info.flags = 0;
        device_create_info.queueCreateInfoCount = 1;
        device_create_info.pQueueCreateInfos = device_queue_create_infos;
        device_create_info.enabledLayerCount = 0;       // deprecated
        device_create_info.ppEnabledLayerNames = NULL;  // deprecated
        device_create_info.enabledExtensionCount = ARRAY_COUNT(device_extensions);
        device_create_info.ppEnabledExtensionNames = device_extensions;
        device_create_info.pEnabledFeatures = &device_features;

        if (vkCreateDevice(physical_device, &device_create_info, NULL, &device) != VK_SUCCESS) {
            log_fatal("Could not create vulkan device");
            return false;
        }
        volkLoadDevice(device);

        // ===============
        
        VkBool32 is_present_supported;
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, graphics_family_index, surface, &is_present_supported);
        if (!(queue_family_properties[graphics_family_index].queueCount > 0 && is_present_supported)) {
            log_fatal("Graphics queue does not support present");
            return false;
        }
        free(queue_family_properties);

        vkGetDeviceQueue(device, graphics_family_index, 0, &graphics_queue);
        vkGetDeviceQueue(device, graphics_family_index, 0, &present_queue);
    }

    // ===============
    // Swapchain
    // ===============
    // Query swapchain support structures
    VkSurfaceCapabilitiesKHR surface_capabilities = { 0 };
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_capabilities);

    uint32_t surface_format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surface_format_count, NULL);
    VkSurfaceFormatKHR* surface_formats = (VkSurfaceFormatKHR*)malloc(surface_format_count * sizeof(VkSurfaceFormatKHR));
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surface_format_count, surface_formats);

    uint32_t present_mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, NULL);
    VkPresentModeKHR* present_modes = (VkPresentModeKHR*)malloc(present_mode_count * sizeof(VkPresentModeKHR));
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, present_modes);

    // Check image count of swapchain
    // If max image count is 0 then there are no limits besides memory requirements
    uint32_t min_image_count = surface_capabilities.minImageCount + 1;
    if (surface_capabilities.maxImageCount > 0 && min_image_count > surface_capabilities.maxImageCount) {
        min_image_count = surface_capabilities.maxImageCount;
    }

    // Choose extent
    if (surface_capabilities.currentExtent.width != UINT32_MAX) {
        swapchain_extent = surface_capabilities.currentExtent;
    }
    else {
        uint32_t min_width = min(surface_capabilities.maxImageExtent.width, ((struct Win32Window*)window)->width);
        uint32_t min_height = min(surface_capabilities.maxImageExtent.height, ((struct Win32Window*)window)->height);
        swapchain_extent.width = max(surface_capabilities.minImageExtent.width, min_width);
        swapchain_extent.height = max(surface_capabilities.minImageExtent.height, min_height);
    }

    // Choose surface format
    VkSurfaceFormatKHR surface_format = surface_formats[0];
    if (surface_format_count == 1 && surface_formats[0].format == VK_FORMAT_UNDEFINED) {
        surface_format.format = VK_FORMAT_B8G8R8A8_UNORM;
        surface_format.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    }
    else {
        for (uint32_t i = 0; i < surface_format_count; i++) {
            if (surface_formats[i].format == VK_FORMAT_B8G8R8A8_UNORM && surface_formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                surface_format = surface_formats[i];
                break;
            }
        }
    }
    free(surface_formats);

    // Choose present mode
    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
    if (!use_vsync) {
        for (uint32_t i = 0; i < present_mode_count; i++) {
            if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
                present_mode = present_modes[i];
                break;
            }
            else if (present_modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
                present_mode = present_modes[i];
            }
        }
    }
    free(present_modes);

    // Prefer non rotated transforms
    VkSurfaceTransformFlagBitsKHR pre_transform = 0;
    if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        pre_transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
    else {
        pre_transform = surface_capabilities.currentTransform;
    }

    // Find a composite alpha to use as not all devices support alpha opaque
    VkCompositeAlphaFlagBitsKHR composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    if (surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) {
        composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    }
    else if (surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) {
        composite_alpha = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
    }
    else if (surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) {
        composite_alpha = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
    }
    else if (surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) {
        composite_alpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
    }

    // Create the actual swapchain
    VkSwapchainCreateInfoKHR swapchain_create_info = { 0 };
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.pNext = NULL;
    swapchain_create_info.flags = 0;
    swapchain_create_info.surface = surface;
    swapchain_create_info.minImageCount = min_image_count;
    swapchain_create_info.imageFormat = surface_format.format;
    swapchain_create_info.imageColorSpace = surface_format.colorSpace;
    swapchain_create_info.imageExtent = swapchain_extent;
    swapchain_create_info.imageArrayLayers = surface_capabilities.maxImageArrayLayers;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_create_info.queueFamilyIndexCount = 0;
    swapchain_create_info.pQueueFamilyIndices = NULL;
    swapchain_create_info.preTransform = pre_transform;
    swapchain_create_info.compositeAlpha = composite_alpha;
    swapchain_create_info.presentMode = present_mode;
    swapchain_create_info.clipped = VK_TRUE;
    swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

    vkCreateSwapchainKHR(device, &swapchain_create_info, NULL, &swapchain);

    // ===============

    // Get images from the swapchain and create image views
    vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, NULL);
    VkImage* swapchain_images = (VkImage*)malloc(swapchain_image_count * sizeof(VkImage));
    vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, swapchain_images);

    image_views = (VkImageView*)malloc(swapchain_image_count * sizeof(VkImage));

    for (uint32_t i = 0; i < swapchain_image_count; i++) {
        VkImageViewCreateInfo image_view_create_info = { 0 };
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.pNext = NULL;
        image_view_create_info.flags = 0;
        image_view_create_info.image = swapchain_images[i];
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.format = surface_format.format;
        image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_create_info.subresourceRange.baseMipLevel = 0;
        image_view_create_info.subresourceRange.levelCount = 1;
        image_view_create_info.subresourceRange.baseArrayLayer = 0;
        image_view_create_info.subresourceRange.layerCount = 1;

        vkCreateImageView(device, &image_view_create_info, NULL, &image_views[i]);
    }
    free(swapchain_images);

    // ===============
    // Render pass
    // ===============
    {
        VkAttachmentDescription attachment_descriptions[2] = { 0 };

        // Color attachment
        attachment_descriptions[0].flags = 0;
        attachment_descriptions[0].format = surface_format.format;
        attachment_descriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
        attachment_descriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachment_descriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment_descriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment_descriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        // VK_IMAGE_LAYOUT_UNDEFINED => The image will be transitioned automatically from
        //                              UNDEFINED to COLOR_ATTACHMENT_OPTIMAL for rendering,
        //                              then out to PRESENT_SRC_KHR at the end
        attachment_descriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachment_descriptions[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        // Depth attachment
        attachment_descriptions[1].flags = 0;
        attachment_descriptions[1].format = depth_format;
        attachment_descriptions[1].samples = VK_SAMPLE_COUNT_1_BIT;
        attachment_descriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachment_descriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment_descriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment_descriptions[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment_descriptions[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachment_descriptions[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference color_attachment_reference = { 0 };
        color_attachment_reference.attachment = 0;
        color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        VkAttachmentReference depth_attachment_reference = { 0 };
        depth_attachment_reference.attachment = 1;
        depth_attachment_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass_description = { 0 };
        subpass_description.flags = 0;
        subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass_description.inputAttachmentCount = 0;
        subpass_description.pInputAttachments = NULL;
        subpass_description.colorAttachmentCount = 1;
        subpass_description.pColorAttachments = &color_attachment_reference;
        subpass_description.pResolveAttachments = NULL;
        subpass_description.pDepthStencilAttachment = &depth_attachment_reference;
        subpass_description.preserveAttachmentCount = 0;
        subpass_description.pPreserveAttachments = NULL;

        VkSubpassDependency subpass_dependency = { 0 };
        subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        subpass_dependency.dstSubpass = 0; // Refers to our one and only subpass
        subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // Needs to be pWaitDstStageMask in the WSI semaphore
        subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpass_dependency.srcAccessMask = 0;
        subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        subpass_dependency.dependencyFlags = 0;

        /*
        Normally, we would need an external dependency at the end as well since we are changing layout in finalLayout,
        but since we are signaling a semaphore, we can rely on Vulkan's default behavior,
        which injects an external dependency here with
            dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            dstAccessMask = 0
        */

        VkRenderPassCreateInfo render_pass_create_info = { 0 };
        render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_create_info.pNext = NULL;
        render_pass_create_info.flags = 0;
        render_pass_create_info.attachmentCount = ARRAY_COUNT(attachment_descriptions);
        render_pass_create_info.pAttachments = attachment_descriptions;
        render_pass_create_info.subpassCount = 1;
        render_pass_create_info.pSubpasses = &subpass_description;
        render_pass_create_info.dependencyCount = 1;
        render_pass_create_info.pDependencies = &subpass_dependency;

        vkCreateRenderPass(device, &render_pass_create_info, NULL, &render_pass);
    }

    // ===============
    // Descriptor Set Layout
    // ===============
    {
        VkDescriptorSetLayoutBinding descriptor_set_layout_bindings[2] = { 0 };
        descriptor_set_layout_bindings[0].binding = 0;
        descriptor_set_layout_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptor_set_layout_bindings[0].descriptorCount = 1;
        descriptor_set_layout_bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        descriptor_set_layout_bindings[0].pImmutableSamplers = NULL;
        descriptor_set_layout_bindings[1].binding = 1;
        descriptor_set_layout_bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptor_set_layout_bindings[1].descriptorCount = 1;
        descriptor_set_layout_bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        descriptor_set_layout_bindings[1].pImmutableSamplers = NULL;

        VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = { 0 };
        descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptor_set_layout_create_info.pNext = NULL;
        descriptor_set_layout_create_info.flags = 0;
        descriptor_set_layout_create_info.bindingCount = ARRAY_COUNT(descriptor_set_layout_bindings);
        descriptor_set_layout_create_info.pBindings = descriptor_set_layout_bindings;

        vkCreateDescriptorSetLayout(device, &descriptor_set_layout_create_info, NULL, &descriptor_set_layouts[0]);
    }

    // ===============
    // Shader
    // ===============
    {
        // TODO: replace with non-platform specific code
        char vertex_shader_path[MAX_PATH];
        strcpy(vertex_shader_path, application_path);
        strcat(vertex_shader_path, "..\\..\\..\\jarp\\Shader\\Basic.vert.spv");
        char fragment_shader_path[MAX_PATH];
        strcpy(fragment_shader_path, application_path);
        strcat(fragment_shader_path, "..\\..\\..\\jarp\\Shader\\Basic.frag.spv");

        struct FileContent vertex_shader_code = file_read_bin(vertex_shader_path);
        struct FileContent fragment_shader_code = file_read_bin(fragment_shader_path);

        if ((sizeof(char) * vertex_shader_code.length) % 4 != 0) {
            log_error("SPIR-V shader code must be a multiple of 4!");
            return false;
        }
        if ((sizeof(char) * fragment_shader_code.length) % 4 != 0) {
            log_error("SPIR-V shader code must be a multiple of 4!");
            return false;
        }

        VkShaderModuleCreateInfo shader_module_create_infos[2] = { 0 };
        shader_module_create_infos[0].sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shader_module_create_infos[0].pNext = NULL;
        shader_module_create_infos[0].flags = 0;
        shader_module_create_infos[0].codeSize = vertex_shader_code.length;
        shader_module_create_infos[0].pCode = (const uint32_t*)vertex_shader_code.buffer;
        shader_module_create_infos[1].sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shader_module_create_infos[1].pNext = NULL;
        shader_module_create_infos[1].flags = 0;
        shader_module_create_infos[1].codeSize = fragment_shader_code.length;
        shader_module_create_infos[1].pCode = (const uint32_t*)fragment_shader_code.buffer;

        vkCreateShaderModule(device, &shader_module_create_infos[0], NULL, &shader_modules[0]);
        vkCreateShaderModule(device, &shader_module_create_infos[1], NULL, &shader_modules[1]);

        pipeline_shader_stage_create_infos[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        pipeline_shader_stage_create_infos[0].pNext = NULL;
        pipeline_shader_stage_create_infos[0].flags = 0;
        pipeline_shader_stage_create_infos[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        pipeline_shader_stage_create_infos[0].module = shader_modules[0];
        pipeline_shader_stage_create_infos[0].pName = "main";
        pipeline_shader_stage_create_infos[0].pSpecializationInfo = NULL;
        pipeline_shader_stage_create_infos[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        pipeline_shader_stage_create_infos[1].pNext = NULL;
        pipeline_shader_stage_create_infos[1].flags = 0;
        pipeline_shader_stage_create_infos[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        pipeline_shader_stage_create_infos[1].module = shader_modules[1];
        pipeline_shader_stage_create_infos[1].pName = "main";
        pipeline_shader_stage_create_infos[1].pSpecializationInfo = NULL;

        free(vertex_shader_code.buffer);
        free(fragment_shader_code.buffer);
    }

    // ===============
    // Graphics pipeline
    // ===============
    {
        VkVertexInputBindingDescription vertex_input_binding_description = { 0 };
        vertex_input_binding_description.binding = 0;
        vertex_input_binding_description.stride = sizeof(struct Vertex);
        vertex_input_binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        VkVertexInputAttributeDescription vertex_input_attribute_descriptions[4] = { 0 };
        vertex_input_attribute_descriptions[0].location = 0;
        vertex_input_attribute_descriptions[0].binding = 0;
        vertex_input_attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        vertex_input_attribute_descriptions[0].offset = offsetof(struct Vertex, position);
        vertex_input_attribute_descriptions[1].location = 1;
        vertex_input_attribute_descriptions[1].binding = 0;
        vertex_input_attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        vertex_input_attribute_descriptions[1].offset = offsetof(struct Vertex, normal);
        vertex_input_attribute_descriptions[2].location = 2;
        vertex_input_attribute_descriptions[2].binding = 0;
        vertex_input_attribute_descriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        vertex_input_attribute_descriptions[2].offset = offsetof(struct Vertex, color);
        vertex_input_attribute_descriptions[3].location = 3;
        vertex_input_attribute_descriptions[3].binding = 0;
        vertex_input_attribute_descriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
        vertex_input_attribute_descriptions[3].offset = offsetof(struct Vertex, texture_coordinate);

        VkPipelineVertexInputStateCreateInfo pipeline_vertex_input_state_create_info = { 0 };
        pipeline_vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        pipeline_vertex_input_state_create_info.pNext = NULL;
        pipeline_vertex_input_state_create_info.flags = 0;
        pipeline_vertex_input_state_create_info.vertexBindingDescriptionCount = 1;
        pipeline_vertex_input_state_create_info.pVertexBindingDescriptions = &vertex_input_binding_description;
        pipeline_vertex_input_state_create_info.vertexAttributeDescriptionCount = ARRAY_COUNT(vertex_input_attribute_descriptions);
        pipeline_vertex_input_state_create_info.pVertexAttributeDescriptions = vertex_input_attribute_descriptions;

        // ===============

        VkPipelineInputAssemblyStateCreateInfo pipeline_input_assembly_state_create_info = { 0 };
        pipeline_input_assembly_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        pipeline_input_assembly_state_create_info.pNext = NULL;
        pipeline_input_assembly_state_create_info.flags = 0;
        pipeline_input_assembly_state_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        pipeline_input_assembly_state_create_info.primitiveRestartEnable = VK_FALSE;

        VkPipelineDepthStencilStateCreateInfo pipeline_depth_stencil_create_info = { 0 };
        pipeline_depth_stencil_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        pipeline_depth_stencil_create_info.pNext = NULL;
        pipeline_depth_stencil_create_info.flags = 0;
        pipeline_depth_stencil_create_info.depthTestEnable = VK_TRUE;
        pipeline_depth_stencil_create_info.depthWriteEnable = VK_TRUE;
        pipeline_depth_stencil_create_info.depthCompareOp = VK_COMPARE_OP_LESS;
        pipeline_depth_stencil_create_info.depthBoundsTestEnable = VK_FALSE;
        pipeline_depth_stencil_create_info.stencilTestEnable = VK_FALSE;
        pipeline_depth_stencil_create_info.front.failOp = 0;
        pipeline_depth_stencil_create_info.front.passOp = 0;
        pipeline_depth_stencil_create_info.front.depthFailOp = 0;
        pipeline_depth_stencil_create_info.front.compareOp = 0;
        pipeline_depth_stencil_create_info.front.compareMask = 0;
        pipeline_depth_stencil_create_info.front.writeMask = 0;
        pipeline_depth_stencil_create_info.front.reference = 0;
        pipeline_depth_stencil_create_info.back.failOp = 0;
        pipeline_depth_stencil_create_info.back.passOp = 0;
        pipeline_depth_stencil_create_info.back.depthFailOp = 0;
        pipeline_depth_stencil_create_info.back.compareOp = 0;
        pipeline_depth_stencil_create_info.back.compareMask = 0;
        pipeline_depth_stencil_create_info.back.writeMask = 0;
        pipeline_depth_stencil_create_info.back.reference = 0;
        pipeline_depth_stencil_create_info.minDepthBounds = 0.0f;
        pipeline_depth_stencil_create_info.maxDepthBounds = 1.0f;

        VkPipelineRasterizationStateCreateInfo pipeline_rasterization_state_create_info = { 0 };
        pipeline_rasterization_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        pipeline_rasterization_state_create_info.pNext = NULL;
        pipeline_rasterization_state_create_info.flags = 0;
        pipeline_rasterization_state_create_info.depthClampEnable = VK_FALSE;
        pipeline_rasterization_state_create_info.rasterizerDiscardEnable = VK_FALSE;
        pipeline_rasterization_state_create_info.polygonMode = VK_POLYGON_MODE_FILL;
        pipeline_rasterization_state_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
        pipeline_rasterization_state_create_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        pipeline_rasterization_state_create_info.depthBiasEnable = VK_FALSE;
        pipeline_rasterization_state_create_info.depthBiasConstantFactor = 0.0f;
        pipeline_rasterization_state_create_info.depthBiasClamp = 0.0f;
        pipeline_rasterization_state_create_info.depthBiasSlopeFactor = 0.0f;
        pipeline_rasterization_state_create_info.lineWidth = 1.0f;

        VkPipelineMultisampleStateCreateInfo pipeline_multisample_state_create_info = { 0 };
        pipeline_multisample_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        pipeline_multisample_state_create_info.pNext = NULL;
        pipeline_multisample_state_create_info.flags = 0;
        pipeline_multisample_state_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        pipeline_multisample_state_create_info.sampleShadingEnable = VK_FALSE;
        pipeline_multisample_state_create_info.minSampleShading = 0.0f;
        pipeline_multisample_state_create_info.pSampleMask = NULL;
        pipeline_multisample_state_create_info.alphaToCoverageEnable = VK_FALSE;
        pipeline_multisample_state_create_info.alphaToOneEnable = VK_FALSE;

        VkPipelineColorBlendAttachmentState pipeline_color_blend_attachment_state = { 0 };
        pipeline_color_blend_attachment_state.blendEnable = VK_TRUE;
        pipeline_color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        pipeline_color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        pipeline_color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
        pipeline_color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        pipeline_color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        pipeline_color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;
        pipeline_color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo pipeline_color_blend_state_create_info = { 0 };
        pipeline_color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        pipeline_color_blend_state_create_info.pNext = NULL;
        pipeline_color_blend_state_create_info.flags = 0;
        pipeline_color_blend_state_create_info.logicOpEnable = VK_FALSE;
        pipeline_color_blend_state_create_info.logicOp = VK_LOGIC_OP_NO_OP;
        pipeline_color_blend_state_create_info.attachmentCount = 1;
        pipeline_color_blend_state_create_info.pAttachments = &pipeline_color_blend_attachment_state;
        pipeline_color_blend_state_create_info.blendConstants[0] = 0.0f;
        pipeline_color_blend_state_create_info.blendConstants[1] = 0.0f;
        pipeline_color_blend_state_create_info.blendConstants[2] = 0.0f;
        pipeline_color_blend_state_create_info.blendConstants[3] = 0.0f;

        VkViewport viewport = { 0.0f };
        viewport.x = 0.0f;
        viewport.y = (float)swapchain_extent.height;
        viewport.width = (float)swapchain_extent.width;
        viewport.height = -(float)swapchain_extent.height; // Flip the coordinate system
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor = { 0 };
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent = swapchain_extent;

        VkPipelineViewportStateCreateInfo pipeline_viewport_state_create_info = { 0 };
        pipeline_viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        pipeline_viewport_state_create_info.pNext = NULL;
        pipeline_viewport_state_create_info.flags = 0;
        pipeline_viewport_state_create_info.viewportCount = 1;
        pipeline_viewport_state_create_info.pViewports = &viewport;
        pipeline_viewport_state_create_info.scissorCount = 1;
        pipeline_viewport_state_create_info.pScissors = &scissor;

        VkPipelineDynamicStateCreateInfo pipeline_dynamic_state_create_info = { 0 };
        pipeline_dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        pipeline_dynamic_state_create_info.pNext = NULL;
        pipeline_dynamic_state_create_info.flags = 0;
        pipeline_dynamic_state_create_info.dynamicStateCount = 0;
        pipeline_dynamic_state_create_info.pDynamicStates = NULL;

        VkPipelineLayoutCreateInfo pipeline_layout_create_info = { 0 };
        pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_create_info.pNext = NULL;
        pipeline_layout_create_info.flags = 0;
        pipeline_layout_create_info.setLayoutCount = ARRAY_COUNT(descriptor_set_layouts);
        pipeline_layout_create_info.pSetLayouts = descriptor_set_layouts;
        pipeline_layout_create_info.pushConstantRangeCount = 0;
        pipeline_layout_create_info.pPushConstantRanges = NULL;

        vkCreatePipelineLayout(device, &pipeline_layout_create_info, NULL, &pipeline_layout);

        VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = { 0 };
        graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        graphics_pipeline_create_info.pNext = NULL;
        graphics_pipeline_create_info.flags = 0;
        graphics_pipeline_create_info.stageCount = ARRAY_COUNT(pipeline_shader_stage_create_infos);
        graphics_pipeline_create_info.pStages = pipeline_shader_stage_create_infos;
        graphics_pipeline_create_info.pVertexInputState = &pipeline_vertex_input_state_create_info;
        graphics_pipeline_create_info.pInputAssemblyState = &pipeline_input_assembly_state_create_info;
        graphics_pipeline_create_info.pTessellationState = NULL;
        graphics_pipeline_create_info.pViewportState = &pipeline_viewport_state_create_info;
        graphics_pipeline_create_info.pRasterizationState = &pipeline_rasterization_state_create_info;
        graphics_pipeline_create_info.pMultisampleState = &pipeline_multisample_state_create_info;
        graphics_pipeline_create_info.pDepthStencilState = &pipeline_depth_stencil_create_info;
        graphics_pipeline_create_info.pColorBlendState = &pipeline_color_blend_state_create_info;
        graphics_pipeline_create_info.pDynamicState = &pipeline_dynamic_state_create_info;
        graphics_pipeline_create_info.layout = pipeline_layout;
        graphics_pipeline_create_info.renderPass = render_pass;
        graphics_pipeline_create_info.subpass = 0;
        graphics_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
        graphics_pipeline_create_info.basePipelineIndex = -1;

        vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, NULL, &pipeline);
    }

    // ===============
    // Command Pool / Buffer
    // ===============
    {
        VkCommandPoolCreateInfo command_pool_create_info = { 0 };
        command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        command_pool_create_info.pNext = NULL;
        command_pool_create_info.flags = 0;
        command_pool_create_info.queueFamilyIndex = graphics_family_index;

        vkCreateCommandPool(device, &command_pool_create_info, NULL, &command_pool);

        command_buffers = (VkCommandBuffer)malloc(swapchain_image_count * sizeof(VkCommandBuffer));
        for (uint32_t i = 0; i < swapchain_image_count; i++) {
            VkCommandBufferAllocateInfo command_buffer_allocate_info = { 0 };
            command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            command_buffer_allocate_info.pNext = NULL;
            command_buffer_allocate_info.commandPool = command_pool;
            command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            command_buffer_allocate_info.commandBufferCount = 1;

            vkAllocateCommandBuffers(device, &command_buffer_allocate_info, &command_buffers[i]);
        }
    }

    // ===============

    // Create image and image view for depth
    if (!create_2d_image_with_view(&depth_image, &depth_image_device_memory, &depth_image_view,
        depth_format, swapchain_extent, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT)) {
        return false;
    }

    transition_image_layout(&command_pool, &depth_image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    // ===============
    // Framebuffer
    // ===============
    {
        framebuffers = (VkFramebuffer*)malloc(swapchain_image_count * sizeof(VkFramebuffer));
        for (uint32_t i = 0; i < swapchain_image_count; i++) {
            VkImageView attachments[] = { image_views[i], depth_image_view };

            VkFramebufferCreateInfo framebuffer_create_info = { 0 };
            framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_create_info.pNext = NULL;
            framebuffer_create_info.flags = 0;
            framebuffer_create_info.renderPass = render_pass;
            framebuffer_create_info.attachmentCount = ARRAY_COUNT(attachments);
            framebuffer_create_info.pAttachments = attachments;
            framebuffer_create_info.width = swapchain_extent.width;
            framebuffer_create_info.height = swapchain_extent.height;
            framebuffer_create_info.layers = 1;

            vkCreateFramebuffer(device, &framebuffer_create_info, NULL, &framebuffers[i]);
        }
    }

    // ===============
    // Descriptor Pool
    // ===============
    {
        VkDescriptorPoolSize descriptor_pool_sizes[2] = { 0 };
        descriptor_pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptor_pool_sizes[0].descriptorCount = swapchain_image_count;
        descriptor_pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptor_pool_sizes[1].descriptorCount = swapchain_image_count;

        VkDescriptorPoolCreateInfo descriptor_pool_create_info = { 0 };
        descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptor_pool_create_info.pNext = NULL;
        descriptor_pool_create_info.flags = 0;
        descriptor_pool_create_info.maxSets = swapchain_image_count;
        descriptor_pool_create_info.poolSizeCount = ARRAY_COUNT(descriptor_pool_sizes);
        descriptor_pool_create_info.pPoolSizes = descriptor_pool_sizes;

        vkCreateDescriptorPool(device, &descriptor_pool_create_info, NULL, &descriptor_pool);
    }

    // ===============
    // Buffer
    // ===============
    {
        if (!create_buffer(&vertex_buffer, &vertex_buffer_memory,
            sizeof(model_vertices[0]) * ARRAY_COUNT(model_vertices),
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
            return false;
        }
        upload_buffer(vertex_buffer, sizeof(model_vertices[0]) * ARRAY_COUNT(model_vertices),
            model_vertices);

        if (!create_buffer(&index_buffer, &index_buffer_memory,
            sizeof(model_indices[0]) * ARRAY_COUNT(model_indices),
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
            return false;
        }
        upload_buffer(index_buffer, sizeof(model_indices[0]) * ARRAY_COUNT(model_indices),
            model_indices);

        for (uint32_t i = 0; i < swapchain_image_count; i++) {
            if (!create_buffer(&uniform_buffers[i], &uniform_buffer_memories[i],
                sizeof(uniform_buffer_object),
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
                return false;
            }
        }
    }

    // ===============
    // Texture
    // ===============
    {
        // TODO: use the texture and render it onto something
        char texture_path[MAX_PATH];
        strcpy(texture_path, application_path);
        strcat(texture_path, "..\\..\\..\\Game\\Content\\sample.png");

        {
            VkBuffer staging_buffer = VK_NULL_HANDLE;
            VkDeviceMemory staging_buffer_memory = VK_NULL_HANDLE;

            int texture_width;
            int texture_height;
            {
                int channels;
                stbi_uc* pixels = stbi_load(texture_path, &texture_width, &texture_height, &channels, STBI_rgb_alpha);
                if (!pixels) {
                    log_error("Failed to load texture image");
                    return false;
                }

                VkDeviceSize image_size = (VkDeviceSize)texture_width * texture_height * 4;
                create_buffer(&staging_buffer, &staging_buffer_memory, image_size,
                    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

                void* raw_data;
                vkMapMemory(device, staging_buffer_memory, 0, image_size, 0, &raw_data);
                memcpy(raw_data, pixels, image_size);
                vkUnmapMemory(device, staging_buffer_memory);

                stbi_image_free(pixels);
            }

            VkExtent2D texture_extent;
            texture_extent.width = texture_width;
            texture_extent.height = texture_height;
            create_2d_image_with_view(&texture_image, &texture_image_memory, &texture_image_view,
                VK_FORMAT_R8G8B8A8_UNORM, texture_extent,
                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_IMAGE_ASPECT_COLOR_BIT);

            transition_image_layout(&command_pool, &texture_image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);


            VkBufferImageCopy buffer_image_copy = { 0 };
            buffer_image_copy.bufferOffset = 0;
            buffer_image_copy.bufferRowLength = 0;
            buffer_image_copy.bufferImageHeight = 0;
            buffer_image_copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            buffer_image_copy.imageSubresource.mipLevel = 0;
            buffer_image_copy.imageSubresource.baseArrayLayer = 0;
            buffer_image_copy.imageSubresource.layerCount = 1;
            buffer_image_copy.imageOffset.x = 0;
            buffer_image_copy.imageOffset.y = 0;
            buffer_image_copy.imageOffset.z = 0;
            buffer_image_copy.imageExtent.width = texture_width;
            buffer_image_copy.imageExtent.height = texture_height;
            buffer_image_copy.imageExtent.depth = 1;

            {
                VkCommandBuffer transient_command_buffer = VK_NULL_HANDLE;
                transient_command_buffer = begin_one_time_submit_command(&command_pool);
                vkCmdCopyBufferToImage(transient_command_buffer, staging_buffer, texture_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &buffer_image_copy);
                end_one_time_submit_command(&transient_command_buffer, &command_pool);
            }

            transition_image_layout(&command_pool, &texture_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

            vkFreeMemory(device, staging_buffer_memory, NULL);
            vkDestroyBuffer(device, staging_buffer, NULL);
        }

        VkSamplerCreateInfo sampler_create_info = { 0 };
        sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_create_info.pNext = NULL;
        sampler_create_info.flags = 0;
        sampler_create_info.magFilter = VK_FILTER_LINEAR;
        sampler_create_info.minFilter = VK_FILTER_LINEAR;
        sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_create_info.mipLodBias = 0.0f;
        sampler_create_info.anisotropyEnable = VK_TRUE; // Also needs to be enabled in the physical device features
        sampler_create_info.maxAnisotropy = 16;
        sampler_create_info.compareEnable = VK_FALSE;
        sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
        sampler_create_info.minLod = 0.0f;
        sampler_create_info.maxLod = 0.0f;
        sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        sampler_create_info.unnormalizedCoordinates = VK_FALSE;

        vkCreateSampler(device, &sampler_create_info, NULL, &texture_sampler);
    }

    // ===============
    // Descriptor Set
    // ===============
    {
        VkDescriptorSetLayout* _descriptor_set_layouts = (VkDescriptorSetLayout*)malloc(swapchain_image_count * sizeof(VkDescriptorSetLayout));
        for (int32_t i = 0; i < swapchain_image_count; i++) {
            _descriptor_set_layouts[i] = descriptor_set_layouts[0];
        }

        VkDescriptorSetAllocateInfo descriptor_set_allocate_info = { 0 };
        descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptor_set_allocate_info.pNext = NULL;
        descriptor_set_allocate_info.descriptorPool = descriptor_pool;
        descriptor_set_allocate_info.descriptorSetCount = swapchain_image_count; // ARRAY_COUNT on the pointer does not work here
        descriptor_set_allocate_info.pSetLayouts = _descriptor_set_layouts;

        descriptor_sets = (VkDescriptorSet*)malloc(swapchain_image_count * sizeof(VkDescriptorSet));
        vkAllocateDescriptorSets(device, &descriptor_set_allocate_info, descriptor_sets);

        for (size_t i = 0; i < swapchain_image_count; i++) {
            VkDescriptorBufferInfo descriptor_buffer_info = { 0 };
            descriptor_buffer_info.buffer = uniform_buffers[i];
            descriptor_buffer_info.offset = 0;
            descriptor_buffer_info.range = sizeof(uniform_buffer_object);

            VkDescriptorImageInfo descriptor_image_info = { 0 };
            descriptor_image_info.sampler = texture_sampler;
            descriptor_image_info.imageView = texture_image_view;
            descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            VkWriteDescriptorSet write_descriptor_sets[2] = { 0 };
            write_descriptor_sets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write_descriptor_sets[0].pNext = NULL;
            write_descriptor_sets[0].dstSet = descriptor_sets[i];
            write_descriptor_sets[0].dstBinding = 0;
            write_descriptor_sets[0].dstArrayElement = 0;
            write_descriptor_sets[0].descriptorCount = 1;
            write_descriptor_sets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            write_descriptor_sets[0].pImageInfo = NULL;
            write_descriptor_sets[0].pBufferInfo = &descriptor_buffer_info;
            write_descriptor_sets[0].pTexelBufferView = NULL;
            write_descriptor_sets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write_descriptor_sets[1].pNext = NULL;
            write_descriptor_sets[1].dstSet = descriptor_sets[i];
            write_descriptor_sets[1].dstBinding = 1;
            write_descriptor_sets[1].dstArrayElement = 0;
            write_descriptor_sets[1].descriptorCount = 1;
            write_descriptor_sets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            write_descriptor_sets[1].pImageInfo = &descriptor_image_info;
            write_descriptor_sets[1].pBufferInfo = NULL;
            write_descriptor_sets[1].pTexelBufferView = NULL;

            vkUpdateDescriptorSets(device, ARRAY_COUNT(write_descriptor_sets), write_descriptor_sets, 0, NULL);
        }
    }

    // ===============
    // Semaphore / Fence
    // ===============
    {
        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            VkSemaphoreCreateInfo semaphore_create_info = { 0 };
            semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            semaphore_create_info.flags = 0;
            semaphore_create_info.pNext = NULL;
            vkCreateSemaphore(device, &semaphore_create_info, NULL, &rendering_finished_semaphores[i]);
            vkCreateSemaphore(device, &semaphore_create_info, NULL, &image_available_semaphores[i]);

            VkFenceCreateInfo fenceCreateInfo = { 0 };
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceCreateInfo.pNext = NULL;
            // Create fences in a signaled state, so the wait command at the start of the draw function doesn't throw debug errors
            fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // TODO: is this really a must?
            vkCreateFence(device, &fenceCreateInfo, NULL, &fences_in_flight[i]);
        }
    }

    // TODO:
    // - recreate swapchain on resize

    record_command_buffer();

    return true;
}

/*
====================
vk_renderer_shutdown
====================
*/
void vk_renderer_shutdown(void) {
    vkDeviceWaitIdle(device);

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (fences_in_flight[i] != VK_NULL_HANDLE) {
            vkDestroyFence(device, fences_in_flight[i], NULL);
        }
        if (image_available_semaphores[i] != VK_NULL_HANDLE) {
            vkDestroySemaphore(device, image_available_semaphores[i], NULL);
        }
        if (rendering_finished_semaphores[i] != VK_NULL_HANDLE) {
            vkDestroySemaphore(device, rendering_finished_semaphores[i], NULL);
        }
    }
    if (texture_sampler != VK_NULL_HANDLE) {
        vkDestroySampler(device, texture_sampler, NULL);
    }
    if (texture_image_view != VK_NULL_HANDLE) {
        vkDestroyImageView(device, texture_image_view, NULL);
    }
    if (texture_image_memory != VK_NULL_HANDLE) {
        vkFreeMemory(device, texture_image_memory, NULL);
    }
    if (texture_image != VK_NULL_HANDLE) {
        vkDestroyImage(device, texture_image, NULL);
    }
    for (uint32_t i = 0; i < swapchain_image_count; i++) {
        if (uniform_buffer_memories[i] != VK_NULL_HANDLE) {
            vkFreeMemory(device, uniform_buffer_memories[i], NULL);
        }
        if (uniform_buffers[i] != VK_NULL_HANDLE) {
            vkDestroyBuffer(device, uniform_buffers[i], NULL);
        }
    }
    if (index_buffer_memory != VK_NULL_HANDLE) {
        vkFreeMemory(device, index_buffer_memory, NULL);
    }
    if (index_buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, index_buffer, NULL);
    }
    if (vertex_buffer_memory != VK_NULL_HANDLE) {
        vkFreeMemory(device, vertex_buffer_memory, NULL);
    }
    if (vertex_buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, vertex_buffer, NULL);
    }
    // We don't touch individual descriptor sets after creating them,
    // so they can just be implicitly destroyed by their descriptor pool
    //for (uint32_t i = 0; i < swapchain_image_count; i++) {
    //    if (descriptor_sets[i] != VK_NULL_HANDLE) {
    //        vkFreeDescriptorSets(device, descriptor_pool, 1, &descriptor_sets[i]);
    //    }
    //}
    free(descriptor_sets);
    if (descriptor_pool != VK_NULL_HANDLE) {
        vkResetDescriptorPool(device, descriptor_pool, 0);
        vkDestroyDescriptorPool(device, descriptor_pool, NULL);
    }
    for (uint32_t i = 0; i < swapchain_image_count; i++) {
        if (command_buffers[i] != VK_NULL_HANDLE) {
            vkFreeCommandBuffers(device, command_pool, 1, &command_buffers[i]);
        }
    }
    free(command_buffers);
    if (command_pool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(device, command_pool, NULL);
    }
    for (uint32_t i = 0; i < swapchain_image_count; i++) {
        if (framebuffers[i] != VK_NULL_HANDLE) {
            vkDestroyFramebuffer(device, framebuffers[i], NULL);
        }
    }
    free(framebuffers);
    if (pipeline_layout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(device, pipeline_layout, NULL);
    }
    if (pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(device, pipeline, NULL);
    }
    for (uint32_t i = 0; i < ARRAY_COUNT(shader_modules); i++) {
        if (shader_modules[i] != VK_NULL_HANDLE) {
            vkDestroyShaderModule(device, shader_modules[i], NULL);
        }
    }
    for (uint32_t i = 0; i < ARRAY_COUNT(descriptor_set_layouts); i++) {
        if (descriptor_set_layouts[i] != VK_NULL_HANDLE) {
            vkDestroyDescriptorSetLayout(device, descriptor_set_layouts[i], NULL);
        }
    }
    if (render_pass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(device, render_pass, NULL);
    }
    if (depth_image_device_memory != VK_NULL_HANDLE) {
        vkFreeMemory(device, depth_image_device_memory, NULL);
    }
    if (depth_image != VK_NULL_HANDLE) {
        vkDestroyImage(device, depth_image, NULL);
    }
    if (depth_image_view != VK_NULL_HANDLE) {
        vkDestroyImageView(device, depth_image_view, NULL);
    }
    for (uint32_t i = 0; i < swapchain_image_count; i++) {
        if (image_views[i] != VK_NULL_HANDLE) {
            vkDestroyImageView(device, image_views[i], NULL);
        }
    }
    free(image_views);
    if (swapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device, swapchain, NULL);
    }
    if (device != VK_NULL_HANDLE) {
        vkDestroyDevice(device, NULL);
    }
    if (surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(instance, surface, NULL);
    }
    if (debug_utils != VK_NULL_HANDLE) {
        vkDestroyDebugUtilsMessengerEXT(instance, debug_utils, NULL);
    }
    if (instance != VK_NULL_HANDLE) {
        vkDestroyInstance(instance, NULL);
    }
}

void record_command_buffer(void) {
    for (uint32_t i = 0; i < swapchain_image_count; i++) {
        VkCommandBufferBeginInfo command_buffer_begin_info = { 0 };
        command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        command_buffer_begin_info.pNext = NULL;
        command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        command_buffer_begin_info.pInheritanceInfo = NULL; // The buffers used here are primary command buffers, so the value can be ignored

        vkBeginCommandBuffer(command_buffers[i], &command_buffer_begin_info);

        VkClearValue clear_values[2] = {
            { 0.2f, 0.2f, 0.3f, 1.0f },
            { 1.0f, 0.0f } // Initial value should be the furthest possible depth (= 1.0)
        };

        VkRenderPassBeginInfo render_pass_begin_info = { 0 };
        render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_begin_info.pNext = NULL;
        render_pass_begin_info.renderPass = render_pass;
        render_pass_begin_info.framebuffer = framebuffers[i];
        render_pass_begin_info.renderArea.extent = swapchain_extent;
        render_pass_begin_info.renderArea.offset.x = 0;
        render_pass_begin_info.renderArea.offset.y = 0;
        render_pass_begin_info.clearValueCount = ARRAY_COUNT(clear_values);
        render_pass_begin_info.pClearValues = clear_values;

        //VkBuffer vertex_buffers[] = { vertex_buffer };
        VkDeviceSize offsets[] = { 0 };

        vkCmdBeginRenderPass(command_buffers[i], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE); // We only have primary command buffers, so an inline subpass suffices
        {
            vkCmdBindPipeline(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
            vkCmdBindVertexBuffers(command_buffers[i], 0, 1, &vertex_buffer, offsets);
            vkCmdBindIndexBuffer(command_buffers[i], index_buffer, 0, VK_INDEX_TYPE_UINT32);
            vkCmdBindDescriptorSets(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout, 0, 1, &descriptor_sets[i], 0, NULL);

            int tmp = ARRAY_COUNT(model_indices);
            vkCmdDrawIndexed(command_buffers[i], ARRAY_COUNT(model_indices), 1, 0, 0, 0);
        }
        vkCmdEndRenderPass(command_buffers[i]);
        vkEndCommandBuffer(command_buffers[i]);
    }
}

void vk_renderer_draw(void) {
    // TODO: set and update MVP matrix
    
    vkWaitForFences(device, 1, &fences_in_flight[current_frame], VK_TRUE, UINT64_MAX);

    vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, image_available_semaphores[current_frame], VK_NULL_HANDLE, &active_image_index);

    if (images_in_flight[active_image_index] != VK_NULL_HANDLE) {
        vkWaitForFences(device, 1, &images_in_flight[active_image_index], VK_TRUE, UINT64_MAX);
    }
    images_in_flight[active_image_index] = fences_in_flight[current_frame];

    VkSemaphore wait_semaphores[] = { image_available_semaphores[current_frame] };
    VkSemaphore signal_semaphores[] = { rendering_finished_semaphores[current_frame] };
    VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submit_info = { 0 };
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.pNext = NULL;
    submit_info.waitSemaphoreCount = ARRAY_COUNT(wait_semaphores);
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount =  1;
    submit_info.pCommandBuffers = &command_buffers[active_image_index];
    submit_info.signalSemaphoreCount = ARRAY_COUNT(signal_semaphores);
    submit_info.pSignalSemaphores = signal_semaphores;

    vkResetFences(device, 1, &fences_in_flight[current_frame]);
    vkQueueSubmit(graphics_queue, 1, &submit_info, fences_in_flight[current_frame]); // TODO: handle possible error

    VkSwapchainKHR swapchains[] = { swapchain };

    VkPresentInfoKHR present_info = { 0 };
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.pNext = NULL;
    present_info.waitSemaphoreCount = ARRAY_COUNT(signal_semaphores);
    present_info.pWaitSemaphores = signal_semaphores;
    present_info.swapchainCount = ARRAY_COUNT(swapchains);
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &active_image_index;
    present_info.pResults = NULL; // Not necessary if using a single swapchain

    vkQueuePresentKHR(graphics_queue, &present_info);

    current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
}
