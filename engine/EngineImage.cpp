
#include "EngineImage.h"

EngineImage::EngineImage(Engine& engineRef, std::string& path, unsigned int w, unsigned int h) : engine{engineRef}{
 	width = w;
    height = h;
	
	// engine = engineRef;

	createImage();
}

EngineImage::~EngineImage(){
	if (view) {
        vkDestroyImageView(engine.GetDevice(), view, nullptr);
        view = 0;
    }
    if (memory) {
        vkFreeMemory(engine.GetDevice(), memory, nullptr);
        memory = 0;
    }
    if (handle) {
        vkDestroyImage(engine.GetDevice(), handle, nullptr);
        handle = 0;
    }
}

void EngineImage::copyFromBuffer(VkBuffer& buffer, VkCommandBuffer& command_buffer){
	VkBufferImageCopy region;
    memset(&region, 0, sizeof(VkBufferImageCopy));
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageExtent.width = width;
    region.imageExtent.height = height;
    region.imageExtent.depth = 1;

    vkCmdCopyBufferToImage(
        command_buffer,
        buffer,
        handle,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region);
}


int EngineImage::find_memory_index(unsigned int type_filter, unsigned int property_flags) {
    VkPhysicalDeviceMemoryProperties memory_properties;
    //vkGetPhysicalDeviceMemoryProperties(context.device.physical_device, &memory_properties);

    for (unsigned int i = 0; i < memory_properties.memoryTypeCount; ++i) {
        // Check each memory type to see if its bit is set to 1.
        if (type_filter & (1 << i) && (memory_properties.memoryTypes[i].propertyFlags & property_flags) == property_flags) {
            return i;
        }
    }

    throw std::runtime_error("Unable to find suitable memory type!");

    return -1;
}


void EngineImage::createImage(){

	VkImageTiling tiling;
    VkImageUsageFlags usage;
   	VkMemoryPropertyFlags memory_flags;
	VkFormat format;
	VkImageAspectFlags view_aspect_flags;

    // Creation info.
    VkImageCreateInfo image_create_info = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.extent.width = width;
    image_create_info.extent.height = height;
    image_create_info.extent.depth = 1;  // TODO: Support configurable depth.
    image_create_info.mipLevels = 4;     // TODO: Support mip mapping
    image_create_info.arrayLayers = 1;   // TODO: Support number of layers in the image.
    image_create_info.format = format;
    image_create_info.tiling = tiling;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.usage = usage;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;          // TODO: Configurable sample count.
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;  // TODO: Configurable sharing mode.

    if(vkCreateImage(engine.GetDevice(), &image_create_info, nullptr, &handle) != VK_SUCCESS){
        throw std::runtime_error("failed to create the image!");
	}

    // Query memory requirements.
    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(engine.GetDevice(), handle, &memory_requirements);

    int memory_type = find_memory_index(memory_requirements.memoryTypeBits, memory_flags);
    if (memory_type == -1) {
        throw std::runtime_error("Required memory type not found. Image not valid.");
    }

    // Allocate memory
    VkMemoryAllocateInfo memory_allocate_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = memory_type;
    if(vkAllocateMemory(engine.GetDevice(), &memory_allocate_info, nullptr, &memory)){
        throw std::runtime_error("failed to allocalte image memory.");
	}

    // Bind the memory
    if(vkBindImageMemory(engine.GetDevice(), handle, memory, 0)){
        throw std::runtime_error("failed to bind image memory.");
	}  // TODO: configurable memory offset.

    // Create view
    // if (create_view) {
	view = 0;
	createImageView(format, view_aspect_flags);
    // }	
}


void EngineImage::createImageView(VkFormat format, VkImageAspectFlags aspectFlags) {
	VkImageViewCreateInfo view_create_info = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    view_create_info.image = handle;
    view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;  // TODO: Make configurable.
    view_create_info.format = format;
    view_create_info.subresourceRange.aspectMask = aspectFlags;

    // TODO: Make configurable
    view_create_info.subresourceRange.baseMipLevel = 0;
    view_create_info.subresourceRange.levelCount = 1;
    view_create_info.subresourceRange.baseArrayLayer = 0;
    view_create_info.subresourceRange.layerCount = 1;

    if(vkCreateImageView(engine.GetDevice(), &view_create_info, nullptr, &view) != VK_SUCCESS){
		throw std::runtime_error("failed to create image view!");
	}
}

void EngineImage::TransitionLayout(VkCommandBuffer& commandBuffer, VkImageLayout old_layout, VkImageLayout new_layout, unsigned int graphicsQueueIndex) {
	VkImageMemoryBarrier barrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = graphicsQueueIndex;
    barrier.dstQueueFamilyIndex = graphicsQueueIndex;
    barrier.image = handle;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags source_stage;
    VkPipelineStageFlags dest_stage;

    // Don't care about the old layout - transition to optimal layout (for the underlying implementation).
    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        // Don't care what stage the pipeline is in at the start.
        source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

        // Used for copying
        dest_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        // Transitioning from a transfer destination layout to a shader-readonly layout.
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        // From a copying stage to...
        source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;

        // The fragment stage.
        dest_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::runtime_error("unsupported layout transition!");
        return;
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        source_stage, dest_stage,
        0, 0, 0, 0, 0,
        1, &barrier);
}