#include "vulkan/platform/app/Application.h"

#include <vulkan/vulkan.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME "VK_KHR_portability_subset"

class MyApplication : public Application {
public:
	virtual bool init(ApplicationController* applicationController) override {
		_applicationController = applicationController;
	  return true;
	}

	virtual void term() override {}
	virtual bool initGraphics() override {
	  if (!createInstance()) {
	  	fprintf(stderr, "failed to create instance.\n");
	  	return false;
	  }

	  if (!createSurface()) {
	  	fprintf(stderr, "failed to create surface.\n");
	  	return false;
	  }

	  if (!pickPhysicalDevice()) {
	  	fprintf(stderr, "failed to pick physical device.\n");
	  	return false;
	  }

	  if (!createLogicalDevice()) {
	  	fprintf(stderr, "failed to create logical device.\n");
	  	return false;
	  }

	  if (!createSwapChain()) {
	  	fprintf(stderr, "failed to create swap chain.\n");
	  	return false;
	  }

	  if (!createImageViews()) {
	  	fprintf(stderr, "failed to create image views.\n");
	  	return false;
	  }

	  if (!createRenderPass()) {
	  	fprintf(stderr, "failed to create render pass.\n");
	  	return false;
	  }

	  if (!createGraphicsPipeline()) {
	  	fprintf(stderr, "failed to create graphics pipeline.\n");
	  	return false;
	  }

	  if (!createFramebuffers()) {
	  	fprintf(stderr, "failed to create frame buffers.\n");
	  	return false;	
	  }

	  if (!createCommandPool()) {
	  	fprintf(stderr, "failed to create command pool.\n");
	  	return false;
	  }

	  if (!createCommandBuffers()) {
	  	fprintf(stderr, "failed to create command buffers.\n");
	  	return false;
	  }

	  if (!createSyncObjects()) {
	  	fprintf(stderr, "failed to create sync objects.\n");
	  	return false;
	  }

	  fprintf(stderr, "init done.\n");
	  return true;
	}

	uint32_t _currentFrame = 0;
	bool framebufferResized = false;

	virtual void termGraphics() override {}
	virtual void renderFrame() override {
		printf("+renderFrame\n");

		vkWaitForFences(_logicalDevice, 1, &_inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(_logicalDevice, _swapChain, UINT64_MAX, _imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    	printf("ERROR\n");
    	exit(-1);
      // recreateSwapChain();
      return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
      // throw std::runtime_error("failed to acquire swap chain image!");
      printf("ERROR#2\n");
      exit(-1);
      return;
    }

    vkResetFences(_logicalDevice, 1, &_inFlightFences[_currentFrame]);

    vkResetCommandBuffer(_commandBuffers[_currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    recordCommandBuffer(_commandBuffers[_currentFrame], imageIndex);

    VkSemaphore waitSemaphores[] = {_imageAvailableSemaphores[_currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSemaphore signalSemaphores[] = {_renderFinishedSemaphores[_currentFrame]};
    VkSubmitInfo submitInfo {
    	.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    	.waitSemaphoreCount = 1,
    	.pWaitSemaphores = waitSemaphores,
    	.pWaitDstStageMask = waitStages,
    	.commandBufferCount = 1,
    	.pCommandBuffers = &_commandBuffers[_currentFrame],
    	.signalSemaphoreCount = 1,
    	.pSignalSemaphores = signalSemaphores,
    };

    if (vkQueueSubmit(_graphicsQueue, 1, &submitInfo, _inFlightFences[_currentFrame]) != VK_SUCCESS) {
      printf("ERROR#3\n");
      exit(-1);
      return;
    }

    VkSwapchainKHR swapChains[] = {_swapChain};
    VkPresentInfoKHR presentInfo {
    	.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
    	.waitSemaphoreCount = 1,
    	.pWaitSemaphores = signalSemaphores,
    	.swapchainCount = 1,
    	.pSwapchains = swapChains,
    	.pImageIndices = &imageIndex,
    };

    result = vkQueuePresentKHR(_presentationQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
      // framebufferResized = false;
      // recreateSwapChain();
      printf("ERROR#4\n");
      exit(-1);
      return;
    } else if (result != VK_SUCCESS) {
      printf("ERROR#5\n");
      exit(-1);
      return;
    }

    _currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	virtual void resize(float windowWidth, float windowHeight) override {}

private:
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo {
    	.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    };

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
      printf("ERROR#6\n");
      exit(-1);
    }

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    VkRenderPassBeginInfo renderPassInfo {
    	.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
	    .renderPass = _renderPass,
	    .framebuffer = _swapChainFramebuffers[imageIndex],
	    .renderArea = {
	    	.offset = {0, 0},
	    	.extent = _swapChainExtent,
	    },
	    .clearValueCount = 1,
    	.pClearValues = &clearColor,
    };

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
      printf("ERROR#7\n");
      exit(-1);
    }
	}

	bool createInstance() {
		VkApplicationInfo appInfo {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pApplicationName = "Hello Triangle",
      .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
      .pEngineName = "No Engine",
      .engineVersion = VK_MAKE_VERSION(1, 0, 0),
      .apiVersion = VK_API_VERSION_1_1,
		};

		uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    VkLayerProperties* props = (VkLayerProperties*) malloc(layerCount * sizeof(VkLayerProperties));
    vkEnumerateInstanceLayerProperties(&layerCount, props);

    enabledLayerCount = 0;
    ppEnabledLayerNames = (const char**) malloc(layerCount * sizeof(const char*));
    for (uint32_t i = 0; i < layerCount; ++i) {
    	const char* layerName = props[i].layerName;
    	if (strcmp("VK_LAYER_LUNARG_device_simulation", layerName) == 0 ||
    			strcmp("VK_LAYER_LUNARG_gfxreconstruct", layerName) == 0) {
    		continue;
    	}

    	ppEnabledLayerNames[enabledLayerCount] = layerName;
    	++enabledLayerCount;
    }

    uint32_t enabledExtensionCount = 2;
    const char* ppEnabledExtensionNames[] = {
#if defined __APPLE__
    	"VK_EXT_metal_surface",
#elif defined _WIN32
    	"VK_KHR_win32_surface",
#elif defined __linux__
    	"VK_KHR_xcb_surface",
#else
# error Platform not supported.
#endif
    	"VK_KHR_surface",
    };

    VkInstanceCreateInfo createInfo {
    	.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    	.pApplicationInfo = &appInfo,
    	.enabledLayerCount = enabledLayerCount,
    	.ppEnabledLayerNames = ppEnabledLayerNames,
    	.enabledExtensionCount = enabledExtensionCount,
    	.ppEnabledExtensionNames = ppEnabledExtensionNames,
    };

    VkResult result = vkCreateInstance(&createInfo, nullptr, &_instance);
    free(props);

    return result == VK_SUCCESS;
	}

	bool createSurface() {
    return _applicationController->createSurface(_instance, &_surface);
	}

	bool pickPhysicalDevice() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);

		if (deviceCount == 0) {
      printf("failed to find GPUs with Vulkan support!\n");
      return false;
    }

		// For now, skip capabilities check and just return the first device available.
    vkEnumeratePhysicalDevices(_instance, &deviceCount, &_physicalDevice);
    return true;
	}

	bool createLogicalDevice() {
		uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, nullptr);

    VkQueueFamilyProperties* props = (VkQueueFamilyProperties*) malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, props);

    _queueFamilyIndexCount = 2;
    VkDeviceQueueCreateInfo queueCreateInfos[2] = {};

    bool foundGraphicsQueue = false;
    bool foundPresentationQueue = false;

    float queuePriority = 1.0f;
    for (uint32_t i = 0; i < queueFamilyCount; ++i) {
    	VkQueueFamilyProperties& prop = props[i]; 
    	bool supportsGraphics = (prop.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0;

    	VkBool32 supportsPresentation = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice, i, _surface, &supportsPresentation);

      if (supportsGraphics) {
      	foundGraphicsQueue = true;

      	if (supportsPresentation) {
      		foundPresentationQueue = true;

      		// Found a queue that supports both. Prefer one queue that can do both.
	        queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	        queueCreateInfos[0].queueFamilyIndex = i;
	        queueCreateInfos[0].queueCount = 1;
	        queueCreateInfos[0].pQueuePriorities = &queuePriority;
	        _queueFamilyIndices[0] = i;
	        _queueFamilyIndices[1] = i;
	        _queueFamilyIndexCount = 1;
	      	break;
      	}

      	foundGraphicsQueue = true;
      	_queueFamilyIndices[0] = i;
    		queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfos[0].queueFamilyIndex = i;
        queueCreateInfos[0].queueCount = 1;
        queueCreateInfos[0].pQueuePriorities = &queuePriority;
      } else if (supportsPresentation) {
      	foundPresentationQueue = true;
      	_queueFamilyIndices[1] = i;
      	queueCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	      queueCreateInfos[1].queueFamilyIndex = i;
	      queueCreateInfos[1].queueCount = 1;
	      queueCreateInfos[1].pQueuePriorities = &queuePriority;
      }
    }
    free(props);

    if (!foundGraphicsQueue || !foundPresentationQueue) {
    	return false;
    }

    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &extensionCount, nullptr);

    VkExtensionProperties* extProps = (VkExtensionProperties*) malloc(extensionCount * sizeof(VkExtensionProperties));
    vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &extensionCount, extProps);

    const char* ppEnabledExtensionNames[] = {
    	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    	VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME,
    };
    uint32_t enabledExtensionCount = 1;
    for (uint32_t i = 0; i < extensionCount; ++i) {
    	if (memcmp(extProps[i].extensionName, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME, sizeof(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME)) == 0) {
    		enabledExtensionCount = 2;
    	}
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    VkDeviceCreateInfo createInfo {
    	.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
	    .queueCreateInfoCount = _queueFamilyIndexCount,
	    .pQueueCreateInfos = queueCreateInfos,
	    .pEnabledFeatures = &deviceFeatures,
	    .enabledLayerCount = enabledLayerCount,
	    .ppEnabledLayerNames = ppEnabledLayerNames,
	    .enabledExtensionCount = enabledExtensionCount,
	    .ppEnabledExtensionNames = ppEnabledExtensionNames,
    };

    if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_logicalDevice) != VK_SUCCESS) {
    	fprintf(stderr, "Failed to create logical device.");
    	return false;
    }

    vkGetDeviceQueue(_logicalDevice, _queueFamilyIndices[0], 0, &_graphicsQueue);
    vkGetDeviceQueue(_logicalDevice, _queueFamilyIndices[1], 0, &_presentationQueue);

    return true;
	}

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(VkSurfaceFormatKHR* candidates, uint32_t numCandidates) {
    for (uint32_t i = 0; i < numCandidates; ++i) {
    	VkSurfaceFormatKHR candidate = candidates[i];
    	if (candidate.format == VK_FORMAT_B8G8R8A8_SRGB && candidate.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        return candidate;
      }        
    }

    return candidates[0];
	}

	VkPresentModeKHR chooseSwapPresentMode(VkPresentModeKHR* candidates, uint32_t numCandidates) {
		for (uint32_t i = 0; i < numCandidates; ++i) {
    	VkPresentModeKHR candidate = candidates[i];
    	if (candidate == VK_PRESENT_MODE_MAILBOX_KHR) {
    		return candidate;
    	}
    }

    return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
      return capabilities.currentExtent;
    }

    // TODO: query VkExtent2D from the surface.
    printf("Not implemented.\n");
    exit(-1);
	}

	bool createSwapChain() {
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface, &formatCount, nullptr);

    if (formatCount == 0) {
    	return false;
    }

    VkSurfaceFormatKHR* formats = (VkSurfaceFormatKHR*) malloc(formatCount * sizeof(VkSurfaceFormatKHR));
    vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface, &formatCount, formats);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(formats, formatCount);
    free(formats);

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, _surface, &presentModeCount, nullptr);

    if (presentModeCount == 0) {
    	return false;
    }

    VkPresentModeKHR* presentModes = (VkPresentModeKHR*) malloc(presentModeCount * sizeof(VkPresentModeKHR));
    vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, _surface, &presentModeCount, presentModes);

    VkPresentModeKHR presentMode = chooseSwapPresentMode(presentModes, presentModeCount);
    free(presentModes);

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice, _surface, &capabilities);
    VkExtent2D extent = chooseSwapExtent(capabilities);

    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
        imageCount = capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo {
    	.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
	    .surface = _surface,
	    .minImageCount = imageCount,
	    .imageFormat = surfaceFormat.format,
	    .imageColorSpace = surfaceFormat.colorSpace,
	    .imageExtent = extent,
	    .imageArrayLayers = 1,
	    .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
	    .preTransform = capabilities.currentTransform,
	    .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
	    .presentMode = presentMode,
	    .clipped = VK_TRUE,
    };

    if (_queueFamilyIndexCount != 1) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = _queueFamilyIndexCount;
        createInfo.pQueueFamilyIndices = _queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    if (vkCreateSwapchainKHR(_logicalDevice, &createInfo, nullptr, &_swapChain) != VK_SUCCESS) {
        return false;
    }

    _swapChainImageFormat = surfaceFormat.format;
    _swapChainExtent = extent;

    return true;
	}

	bool createImageViews() {
		vkGetSwapchainImagesKHR(_logicalDevice, _swapChain, &_swapChainImageCount, nullptr);

		VkImage* swapChainImages = (VkImage*) malloc(_swapChainImageCount * sizeof(VkImage));
    vkGetSwapchainImagesKHR(_logicalDevice, _swapChain, &_swapChainImageCount, swapChainImages);
		
		_swapChainImageViews = (VkImageView*) malloc(_swapChainImageCount * sizeof(VkImageView));
		VkImageViewCreateInfo createInfo {
     	.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
     	.viewType = VK_IMAGE_VIEW_TYPE_2D,
     	.format = _swapChainImageFormat,
     	.components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
     	.components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
     	.components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
     	.components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
     	.subresourceRange = {
     		.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
	     	.baseMipLevel = 0,
	     	.levelCount = 1,
	     	.baseArrayLayer = 0,
	     	.layerCount = 1,
     	},
    };

    for (uint32_t i = 0; i < _swapChainImageCount; i++) {
        createInfo.image = swapChainImages[i];

        if (vkCreateImageView(_logicalDevice, &createInfo, nullptr, &_swapChainImageViews[i]) != VK_SUCCESS) {
            return false;
        }
    }

    free(swapChainImages);
    return true;
	}

	bool createRenderPass() {
		VkAttachmentDescription colorAttachment {
			.format = _swapChainImageFormat,
	    .samples = VK_SAMPLE_COUNT_1_BIT,
	    .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
	    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
	    .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
	    .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
	    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	    .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		};
    

    VkAttachmentReference colorAttachmentRef {
    	.attachment = 0,
    	.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkSubpassDescription subpass {
    	.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
	    .colorAttachmentCount = 1,
	    .pColorAttachments = &colorAttachmentRef,
    };

    VkSubpassDependency dependency {
    	.srcSubpass = VK_SUBPASS_EXTERNAL,
	    .dstSubpass = 0,
	    .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	    .srcAccessMask = 0,
	    .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	    .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    };
    
    VkRenderPassCreateInfo renderPassInfo {
    	.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
	    .attachmentCount = 1,
	    .pAttachments = &colorAttachment,
	    .subpassCount = 1,
	    .pSubpasses = &subpass,
	    .dependencyCount = 1,
	    .pDependencies = &dependency,
    };

    return vkCreateRenderPass(_logicalDevice, &renderPassInfo, nullptr, &_renderPass) == VK_SUCCESS;
	}

	bool createShaderModule(const char* fileName, VkShaderModule* shaderModule) {
		uint64_t codeSize;
		void* code = _applicationController->readFile(fileName, &codeSize);

    VkShaderModuleCreateInfo createInfo {
    	.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .codeSize = codeSize,
      .pCode = reinterpret_cast<const uint32_t*>(code),
    };

    VkResult result = vkCreateShaderModule(_logicalDevice, &createInfo, nullptr, shaderModule);
    free(code);

    return result == VK_SUCCESS;
  }

	bool createGraphicsPipeline() {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo {
    	.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    	.setLayoutCount = 0,
    	.pushConstantRangeCount = 0,
    };

    VkPipelineLayout pipelineLayout;
    if (vkCreatePipelineLayout(_logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
      return false;
    }

    VkShaderModule vertShaderModule;
		if (!createShaderModule("vert.spv", &vertShaderModule)) {
			return false;
		}

		VkShaderModule fragShaderModule;
		if (!createShaderModule("frag.spv", &fragShaderModule)) {
			return false;
		}

		VkPipelineShaderStageCreateInfo vertShaderStageInfo {
    	.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
	    .stage = VK_SHADER_STAGE_VERTEX_BIT,
	    .module = vertShaderModule,
	    .pName = "main",
    };
    
    VkPipelineShaderStageCreateInfo fragShaderStageInfo {
    	.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
	    .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
	    .module = fragShaderModule,
	    .pName = "main",
    };

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo {
    	.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    	.vertexBindingDescriptionCount = 0,
    	.vertexAttributeDescriptionCount = 0,
    };

    VkViewport viewport {
    	.x = 0.0f,
	    .y = 0.0f,
	    .width = (float) _swapChainExtent.width,
	    .height = (float) _swapChainExtent.height,
	    .minDepth = 0.0f,
	    .maxDepth = 1.0f,	
    };

    VkRect2D scissor {
    	.offset = {0, 0},
    	.extent = _swapChainExtent,
    };

    VkPipelineViewportStateCreateInfo viewportState {
    	.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
	    .viewportCount = 1,
	    .pViewports = &viewport,
	    .scissorCount = 1,
	    .pScissors = &scissor,
    };

    VkPipelineMultisampleStateCreateInfo multisampling {
    	.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
    	.sampleShadingEnable = VK_FALSE,
    	.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    };

    VkPipelineRasterizationStateCreateInfo rasterizer {
    	.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
	    .depthClampEnable = VK_FALSE,
	    .rasterizerDiscardEnable = VK_FALSE,
	    .polygonMode = VK_POLYGON_MODE_FILL,
	    .lineWidth = 1.0f,
	    .cullMode = VK_CULL_MODE_BACK_BIT,
	    .frontFace = VK_FRONT_FACE_CLOCKWISE,
	    .depthBiasEnable = VK_FALSE,
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssembly {
    	.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
    	.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    	.primitiveRestartEnable = VK_FALSE,
    };

    VkPipelineColorBlendAttachmentState colorBlendAttachment {
    	.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    	.blendEnable = VK_FALSE,
    };

    VkPipelineColorBlendStateCreateInfo colorBlending {
    	.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
	    .logicOpEnable = VK_FALSE,
	    .logicOp = VK_LOGIC_OP_COPY,
	    .attachmentCount = 1,
	    .pAttachments = &colorBlendAttachment,
	    .blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f, },
    };

    VkGraphicsPipelineCreateInfo pipelineInfo {
    	.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
	    .stageCount = 2,
	    .pStages = shaderStages,
	    .pVertexInputState = &vertexInputInfo,
	    .pInputAssemblyState = &inputAssembly,
	    .pViewportState = &viewportState,
	    .pRasterizationState = &rasterizer,
	    .pMultisampleState = &multisampling,
	    .pColorBlendState = &colorBlending,
	    .layout = pipelineLayout,
	    .renderPass = _renderPass,
	    .subpass = 0,
	    .basePipelineHandle = VK_NULL_HANDLE,
    };
    
    VkResult result = vkCreateGraphicsPipelines(_logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_graphicsPipeline);

    vkDestroyShaderModule(_logicalDevice, fragShaderModule, nullptr);
    vkDestroyShaderModule(_logicalDevice, vertShaderModule, nullptr);

    return result == VK_SUCCESS;
	}

	bool createFramebuffers() {
		_swapChainFramebuffers = (VkFramebuffer*) malloc(_swapChainImageCount * sizeof(VkFramebuffer));

		VkFramebufferCreateInfo framebufferInfo {
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .renderPass = _renderPass,
      .attachmentCount = 1,
      
      .width = _swapChainExtent.width,
      .height = _swapChainExtent.height,
      .layers = 1,
		};

  	for (uint32_t i = 0; i < _swapChainImageCount; i++) {
      framebufferInfo.pAttachments = &_swapChainImageViews[i];

      if (vkCreateFramebuffer(_logicalDevice, &framebufferInfo, nullptr, &_swapChainFramebuffers[i]) != VK_SUCCESS) {
          return false;
      }
    }

    return true;
	}

	bool createCommandPool() {
		VkCommandPoolCreateInfo poolInfo {
	    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
	    .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
	    .queueFamilyIndex = _queueFamilyIndices[0],
	  };

    return vkCreateCommandPool(_logicalDevice, &poolInfo, nullptr, &_commandPool) == VK_SUCCESS;
	}

	bool createCommandBuffers() {
    VkCommandBufferAllocateInfo allocInfo {
    	.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
	    .commandPool = _commandPool,
	    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
	    .commandBufferCount = MAX_FRAMES_IN_FLIGHT,
    };

    return vkAllocateCommandBuffers(_logicalDevice, &allocInfo, _commandBuffers) == VK_SUCCESS;
	}

	bool createSyncObjects() {
    VkSemaphoreCreateInfo semaphoreInfo {
    	.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    VkFenceCreateInfo fenceInfo {
    	.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    	.flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(_logicalDevice, &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(_logicalDevice, &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(_logicalDevice, &fenceInfo, nullptr, &_inFlightFences[i]) != VK_SUCCESS) {
            return false;
        }
    }

    return true;
	}

	enum {
		MAX_FRAMES_IN_FLIGHT = 2,
	};

	ApplicationController* _applicationController;
	VkInstance _instance;
	VkSurfaceKHR _surface;
	VkPhysicalDevice _physicalDevice;
	VkDevice _logicalDevice;
	VkSwapchainKHR _swapChain;
	VkImageView* _swapChainImageViews;
	VkFormat _swapChainImageFormat;
	VkExtent2D _swapChainExtent;
	VkRenderPass _renderPass;
	VkPipeline _graphicsPipeline;
	VkFramebuffer* _swapChainFramebuffers;
	VkCommandPool _commandPool;
	VkCommandBuffer _commandBuffers[MAX_FRAMES_IN_FLIGHT];
	VkSemaphore _imageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT];
  VkSemaphore _renderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT];
  VkFence _inFlightFences[MAX_FRAMES_IN_FLIGHT];
  VkQueue _graphicsQueue;
  VkQueue _presentationQueue;
	uint32_t _swapChainImageCount;
	uint32_t _queueFamilyIndexCount;
	uint32_t _queueFamilyIndices[2];
	
	uint32_t enabledLayerCount;
  const char** ppEnabledLayerNames;
};

Application* createApplication() {
	return new MyApplication();
}
