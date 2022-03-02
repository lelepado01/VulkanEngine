
#include <stdexcept>
#include <string>

#include "Engine.h"

class EngineImage {
private: 
    VkImage handle;
    VkDeviceMemory memory;
    VkImageView view;

	Engine& engine; 

    unsigned int width;
    unsigned int height;

private:
	void createImageView(VkFormat format, VkImageAspectFlags aspectFlags);
	void createImage(); 
	void copyFromBuffer(VkBuffer& buffer, VkCommandBuffer& command_buffer); 
	
	int find_memory_index(unsigned int type_filter, unsigned int property_flags); 

public:
	EngineImage(Engine& engineRef, std::string& path, unsigned int w, unsigned int h);
	~EngineImage();
	
	void TransitionLayout(VkCommandBuffer& commandBuffer, VkImageLayout old_layout, VkImageLayout new_layout, unsigned int graphicsQueueIndex);
};
