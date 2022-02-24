#include "vulkan/platform/app/Application.h"

#include <vulkan/vulkan.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME "VK_KHR_portability_subset"

#if defined NDEBUG
#define CHECK_CURRENT_STATE(X)
#define POINT_OF_FAILURE(X) X
#else
static_assert(__COUNTER__ == 0);

#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)

#define CHECK_STATE_DEF(var, initState, defValue, fmt) if ((_currentState < initState) != (var == defValue)) { fprintf(stderr, "check failed for %s, currentState: %d, initState: %d, value: " fmt " in %s\n", #var, _currentState, initState, var, function); exit(-1); }
#define CHECK_STATE(var, initState) CHECK_STATE_DEF(var, initState, nullptr, "%p")
#define CHECK_CURRENT_STATE(expectedState) if (_currentState != expectedState) { fprintf(stderr, "Expected state %d but _currentState is %d at %s:%d.\n", expectedState, _currentState, __FUNCTION__, __LINE__); exit(-1); } else { checkInvariant(__FUNCTION__); }

#define POINT_OF_FAILURE(method) _pointOfFailure == CONCAT(POINT_OF_FAILURE_, __COUNTER__) ? VK_NOT_READY : method

enum PointOfFailure {
	NO_FAILURE = 0,
	POINT_OF_FAILURE_1,
	POINT_OF_FAILURE_2,
	POINT_OF_FAILURE_3,
	POINT_OF_FAILURE_4,
	POINT_OF_FAILURE_5,
	POINT_OF_FAILURE_6,
	POINT_OF_FAILURE_7,
	POINT_OF_FAILURE_8,
	POINT_OF_FAILURE_9,
	POINT_OF_FAILURE_10,
	POINT_OF_FAILURE_11,
	POINT_OF_FAILURE_12,
	POINT_OF_FAILURE_13,
	POINT_OF_FAILURE_14,
	POINT_OF_FAILURE_15,
	POINT_OF_FAILURE_16,
	POINT_OF_FAILURE_17,
	POINT_OF_FAILURE_18,
	POINT_OF_FAILURE_19,
	POINT_OF_FAILURE_20,
	POINT_OF_FAILURE_21,
	POINT_OF_FAILURE_22,
	POINT_OF_FAILURE_MAX,
};
#endif

class MyApplication : public Application {
public:
	virtual bool init(ApplicationController* applicationController) override {
#if !defined NDEBUG
		checkInvariant(__FUNCTION__);
#endif

		_applicationController = applicationController;
	  return true;
	}

#if !defined NDEBUG
	void checkInvariant(const char* function) {
		if (_enabledLayerCount != 0) {
			if (_ppEnabledLayerNames == nullptr) {
				fprintf(stderr, "_enabledLayerCount is not 0 but _ppEnabledLayerNames is nullptr.\n");
				exit(-1);
			}

			// Should only be initialized if _currentState is STATE_LAYERS_INITIALIZED or higher.
			if (_currentState < STATE_LAYERS_INITIALIZED) {
				fprintf(stderr, "_enabledLayerCount is not 0 but _currentState is %d.\n", _currentState);
				exit(-1);
			}
		} else if (_ppEnabledLayerNames != nullptr) {
			fprintf(stderr, "_enabledLayerCount is 0 but _ppEnabledLayerNames is not nullptr.\n");
			exit(-1);
		}

		CHECK_STATE(_instance, STATE_INSTANCE_CREATED);
		CHECK_STATE(_surface, STATE_SURFACE_CREATED);
		CHECK_STATE(_physicalDevice, STATE_PHYSICAL_DEVICE_CREATED);
		CHECK_STATE(_logicalDevice, STATE_LOGICAL_DEVICE_CREATED);
		CHECK_STATE_DEF(_queueFamilyIndexCount, STATE_LOGICAL_DEVICE_CREATED, -1, "%d");
		CHECK_STATE_DEF(_queueFamilyIndices[0], STATE_LOGICAL_DEVICE_CREATED, -1, "%d");
		CHECK_STATE_DEF(_queueFamilyIndices[1], STATE_LOGICAL_DEVICE_CREATED, -1, "%d");
		CHECK_STATE(_graphicsQueue, STATE_LOGICAL_DEVICE_CREATED);
		CHECK_STATE(_presentationQueue, STATE_LOGICAL_DEVICE_CREATED);
		CHECK_STATE(_swapChain, STATE_SWAP_CHAIN_CREATED);
		CHECK_STATE_DEF(_swapChainImageFormat, STATE_SWAP_CHAIN_CREATED, VK_FORMAT_UNDEFINED, "%d");
		CHECK_STATE_DEF(_swapChainExtent.width, STATE_SWAP_CHAIN_CREATED, -1, "%d");
		CHECK_STATE_DEF(_swapChainExtent.height, STATE_SWAP_CHAIN_CREATED, -1, "%d");
		CHECK_STATE(_swapChainImageViews, STATE_SWAP_CHAIN_IMAGE_VIEWS_CREATED);		
		CHECK_STATE_DEF(_swapChainImageCount, STATE_SWAP_CHAIN_IMAGE_VIEWS_CREATED, -1, "%d");
		CHECK_STATE(_renderPass, STATE_RENDER_PASS_CREATED);
		CHECK_STATE(_pipelineLayout, STATE_PIPELINE_LAYOUT_CREATED);
		CHECK_STATE(_graphicsPipeline, STATE_GRAPHICS_PIPELINE_CREATED);
		CHECK_STATE(_swapChainFramebuffers, STATE_SWAP_CHAIN_FRAMEBUFFERS_CREATED);
		CHECK_STATE(_commandPool, STATE_COMMAND_POOL_CREATED);
		CHECK_STATE(_commandBuffers[0], STATE_COMMAND_BUFFERS_CREATED);
		CHECK_STATE(_commandBuffers[1], STATE_COMMAND_BUFFERS_CREATED);
		CHECK_STATE(_imageAvailableSemaphores[0], STATE_SYNC_OBJECTS_CREATED);
		CHECK_STATE(_imageAvailableSemaphores[1], STATE_SYNC_OBJECTS_CREATED);
		CHECK_STATE(_renderFinishedSemaphores[0], STATE_SYNC_OBJECTS_CREATED);
		CHECK_STATE(_renderFinishedSemaphores[1], STATE_SYNC_OBJECTS_CREATED);
		CHECK_STATE(_inFlightFences[0], STATE_SYNC_OBJECTS_CREATED);
		CHECK_STATE(_inFlightFences[1], STATE_SYNC_OBJECTS_CREATED);	
	}
#endif

	bool initialize() {
		if (!initializeLayers()) {
			return deinitialize(STATE_UNINITIALIZED);
		}
		CHECK_CURRENT_STATE(STATE_LAYERS_INITIALIZED);

	  if (!createInstance()) {
	  	return deinitialize(STATE_LAYERS_INITIALIZED);
	  }
	  CHECK_CURRENT_STATE(STATE_INSTANCE_CREATED);

	  if (!createSurface()) {
	  	return deinitialize(STATE_INSTANCE_CREATED);
	  }
	  CHECK_CURRENT_STATE(STATE_SURFACE_CREATED);

	  if (!pickPhysicalDevice()) {
	  	return deinitialize(STATE_SURFACE_CREATED);
	  }
	  CHECK_CURRENT_STATE(STATE_PHYSICAL_DEVICE_CREATED);

	  if (!createLogicalDevice()) {
	  	return deinitialize(STATE_PHYSICAL_DEVICE_CREATED);
	  }
	  CHECK_CURRENT_STATE(STATE_LOGICAL_DEVICE_CREATED);

	  if (!createCommandPool()) {
	  	return deinitialize(STATE_LOGICAL_DEVICE_CREATED);
	  }
	  CHECK_CURRENT_STATE(STATE_COMMAND_POOL_CREATED);

	  if (!createCommandBuffers()) {
	  	return deinitialize(STATE_COMMAND_POOL_CREATED);
	  }
	  CHECK_CURRENT_STATE(STATE_COMMAND_BUFFERS_CREATED);

	  if (!createSyncObjects()) {
	  	return deinitialize(STATE_COMMAND_BUFFERS_CREATED);
	  }
	  CHECK_CURRENT_STATE(STATE_SYNC_OBJECTS_CREATED);

	  if (!createSwapChain()) {
	  	return deinitialize(STATE_SYNC_OBJECTS_CREATED);
	  }
	  CHECK_CURRENT_STATE(STATE_SWAP_CHAIN_CREATED);

	  if (!createSwapChainImageViews()) {
	  	return deinitialize(STATE_SWAP_CHAIN_CREATED);
	  }
	  CHECK_CURRENT_STATE(STATE_SWAP_CHAIN_IMAGE_VIEWS_CREATED);

	  if (!createRenderPass()) {
	  	return deinitialize(STATE_SWAP_CHAIN_IMAGE_VIEWS_CREATED);
	  }
	  CHECK_CURRENT_STATE(STATE_RENDER_PASS_CREATED);

	  if (!createFramebuffers()) {
	  	return deinitialize(STATE_RENDER_PASS_CREATED);
	  }
	  CHECK_CURRENT_STATE(STATE_SWAP_CHAIN_FRAMEBUFFERS_CREATED);

	  if (!createPipelineLayout()) {
	  	return deinitialize(STATE_SWAP_CHAIN_FRAMEBUFFERS_CREATED);
	  }
	  CHECK_CURRENT_STATE(STATE_PIPELINE_LAYOUT_CREATED);

	  if (!createGraphicsPipeline()) {
	  	return deinitialize(STATE_PIPELINE_LAYOUT_CREATED);
	  }
	  CHECK_CURRENT_STATE(STATE_GRAPHICS_PIPELINE_CREATED);

	  return true;
	}

	void deinitializeLayers() {
		CHECK_CURRENT_STATE(STATE_LAYERS_INITIALIZED);
		for (uint32_t i = 0; i < _enabledLayerCount; ++i) {
			free((char*) _ppEnabledLayerNames[i]);
		}
		free(_ppEnabledLayerNames);
		_enabledLayerCount = 0;
		_ppEnabledLayerNames = nullptr;

#if !defined NDEBUG
		--_currentState;
		checkInvariant("deinitializeLayers");
#endif
	}

	void deinitializeInstance() {
		CHECK_CURRENT_STATE(STATE_INSTANCE_CREATED);
		vkDestroyInstance(_instance, nullptr);
		_instance = nullptr;

#if !defined NDEBUG
		--_currentState;
		checkInvariant("deinitializeInstance");
#endif
	}

	void deinitializeSurface() {
		CHECK_CURRENT_STATE(STATE_SURFACE_CREATED);
		// TODO: also destroy associated CAMetalLayer on MacOS.
		vkDestroySurfaceKHR(_instance, _surface, nullptr);
		_surface = nullptr;

#if !defined NDEBUG
		--_currentState;
		checkInvariant("deinitializeSurface");
#endif
	}

	void deinitializePhysicalDevice() {
		CHECK_CURRENT_STATE(STATE_PHYSICAL_DEVICE_CREATED);
		_physicalDevice = nullptr;

#if !defined NDEBUG
		--_currentState;
		checkInvariant("deinitializePhysicalDevice");
#endif
	}

	void deinitializeLogicalDevice() {
		CHECK_CURRENT_STATE(STATE_LOGICAL_DEVICE_CREATED);
		vkDestroyDevice(_logicalDevice, nullptr);
		_logicalDevice = nullptr;

		_queueFamilyIndexCount = 0xFFFFFFFF;
		_queueFamilyIndices[0] = 0xFFFFFFFF;
		_queueFamilyIndices[1] = 0xFFFFFFFF;
		_graphicsQueue = nullptr;
		_presentationQueue = nullptr;

#if !defined NDEBUG
		--_currentState;
		checkInvariant("deinitializeLogicalDevice");
#endif
	}

	void deinitializeSwapChain() {
		CHECK_CURRENT_STATE(STATE_SWAP_CHAIN_CREATED);
		vkDestroySwapchainKHR(_logicalDevice, _swapChain, nullptr);

		_swapChain = nullptr;
		_swapChainImageFormat = VK_FORMAT_UNDEFINED;
		_swapChainExtent.width = 0xFFFFFFFF;
		_swapChainExtent.height = 0xFFFFFFFF;

#if !defined NDEBUG
		--_currentState;
		checkInvariant("deinitializeSwapChain");
#endif
	}

	void deinitializeSwapChainImageViews() {
		CHECK_CURRENT_STATE(STATE_SWAP_CHAIN_IMAGE_VIEWS_CREATED);
		for (uint32_t i = 0; i < _swapChainImageCount; ++i) {
    	vkDestroyImageView(_logicalDevice, _swapChainImageViews[i], nullptr);
    }
    free(_swapChainImageViews);
    _swapChainImageViews = nullptr;
    _swapChainImageCount = 0xFFFFFFFF;

#if !defined NDEBUG
		--_currentState;
		checkInvariant("deinitializeSwapChainImageViews");
#endif
	}

	void deinitializeRenderPass() {
		CHECK_CURRENT_STATE(STATE_RENDER_PASS_CREATED);
		vkDestroyRenderPass(_logicalDevice, _renderPass, nullptr);
		_renderPass = nullptr;

#if !defined NDEBUG
		--_currentState;
		checkInvariant("deinitializeRenderPass");
#endif
	}

	void deinitializePipelineLayout() {
		CHECK_CURRENT_STATE(STATE_PIPELINE_LAYOUT_CREATED);
		vkDestroyPipelineLayout(_logicalDevice, _pipelineLayout, nullptr);
		_pipelineLayout = nullptr;

#if !defined NDEBUG
		--_currentState;
		checkInvariant("deinitializePipelineLayout");
#endif
	}

	void deinitializeGraphicsPipeline() {
		CHECK_CURRENT_STATE(STATE_GRAPHICS_PIPELINE_CREATED);
		vkDestroyPipeline(_logicalDevice, _graphicsPipeline, nullptr);
		_graphicsPipeline = nullptr;

#if !defined NDEBUG
		--_currentState;
		checkInvariant("deinitializeGraphicsPipeline");
#endif
	}

	void deinitializeFramebuffers() {
		CHECK_CURRENT_STATE(STATE_SWAP_CHAIN_FRAMEBUFFERS_CREATED);
		for (uint32_t i = 0; i < _swapChainImageCount; ++i) {
			vkDestroyFramebuffer(_logicalDevice, _swapChainFramebuffers[i], nullptr);
		}
		free(_swapChainFramebuffers);
		_swapChainFramebuffers = nullptr;

#if !defined NDEBUG
		--_currentState;
		checkInvariant("deinitializeFramebuffers");
#endif
	}

	void deinitializeCommandPool() {
		CHECK_CURRENT_STATE(STATE_COMMAND_POOL_CREATED);
		vkDestroyCommandPool(_logicalDevice, _commandPool, nullptr);
		_commandPool = nullptr;

#if !defined NDEBUG
		--_currentState;
		checkInvariant("deinitializeCommandPool");
#endif
	}

	void deinitializeCommandBuffers() {
		CHECK_CURRENT_STATE(STATE_COMMAND_BUFFERS_CREATED);
		vkFreeCommandBuffers(_logicalDevice, _commandPool, MAX_FRAMES_IN_FLIGHT, _commandBuffers);
		_commandBuffers[0] = nullptr;
		_commandBuffers[1] = nullptr;

#if !defined NDEBUG
		--_currentState;
    checkInvariant("deinitializeCommandBuffers");
#endif
	}

	void deinitializeSyncObjects() {
		CHECK_CURRENT_STATE(STATE_SYNC_OBJECTS_CREATED);
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      vkDestroySemaphore(_logicalDevice, _renderFinishedSemaphores[i], nullptr);
      _renderFinishedSemaphores[i] = nullptr;

      vkDestroySemaphore(_logicalDevice, _imageAvailableSemaphores[i], nullptr);
      _imageAvailableSemaphores[i] = nullptr;

      vkDestroyFence(_logicalDevice, _inFlightFences[i], nullptr);
      _inFlightFences[i] = nullptr;
    }

#if !defined NDEBUG
    --_currentState;
		checkInvariant("deinitializeSyncObjects");
#endif
	}

	bool deinitialize(int currentState) {
#if !defined NDEBUG
		if (_currentState != currentState) {
			fprintf(stderr, "Current state didn't match expected state: %d != %d.\n", _currentState, currentState);
		}
		checkInvariant("deinitialize");
#endif

		switch (currentState) {
			case STATE_GRAPHICS_PIPELINE_CREATED:
				deinitializeGraphicsPipeline();

			case STATE_PIPELINE_LAYOUT_CREATED:
				deinitializePipelineLayout();

			case STATE_SWAP_CHAIN_FRAMEBUFFERS_CREATED:
				deinitializeFramebuffers();

			case STATE_RENDER_PASS_CREATED:
				deinitializeRenderPass();

			case STATE_SWAP_CHAIN_IMAGE_VIEWS_CREATED:
				deinitializeSwapChainImageViews();

			case STATE_SWAP_CHAIN_CREATED:
				deinitializeSwapChain();

			case STATE_SYNC_OBJECTS_CREATED:
				deinitializeSyncObjects();

			case STATE_COMMAND_BUFFERS_CREATED:
				deinitializeCommandBuffers();

			case STATE_COMMAND_POOL_CREATED:
				deinitializeCommandPool();

			case STATE_LOGICAL_DEVICE_CREATED:
				deinitializeLogicalDevice();

			case STATE_PHYSICAL_DEVICE_CREATED:
				deinitializePhysicalDevice();

			case STATE_SURFACE_CREATED:
				deinitializeSurface();

			case STATE_INSTANCE_CREATED:
				deinitializeInstance();

			case STATE_LAYERS_INITIALIZED:
				deinitializeLayers();

			case STATE_UNINITIALIZED:
				break;
		}

		return false;
	}

	virtual void term() override {}
	virtual bool initGraphics() override {
		// _pointOfFailure = NO_FAILURE;
		// checkInvariant("1");
		// if (!initialize()) {
		// 	fprintf(stderr, "initialize() expected to succeed but failed.\n");
		// 	exit(-1);
		// }
		// deinitialize(STATE_GRAPHICS_PIPELINE_CREATED);
		// checkInvariant("2");

		// for (int i = POINT_OF_FAILURE_1; i < POINT_OF_FAILURE_MAX; ++i) {
		// 	_pointOfFailure = (PointOfFailure) i;

		// 	checkInvariant("before");
		// 	if (initialize()) {
		// 		fprintf(stderr, "Expected to fail initialize, _pointOfFailure: %d.\n", _pointOfFailure);
		// 		exit(-1);
		// 	}
		// 	if (_currentState != STATE_UNINITIALIZED) {
		// 		fprintf(stderr, "initialize() failed but the state is not STATE_UNINITIALIZED, _pointOfFailure: %d.\n", _pointOfFailure);
		// 		exit(-1);
		// 	}
		// 	checkInvariant("after");
		// }

		return initialize();
	}


	uint32_t _currentFrame = 0;
	bool framebufferResized = false;

	virtual void termGraphics() override {
		if (_logicalDevice != nullptr) {
			// Wait for the device to become available.
			VkResult result = vkDeviceWaitIdle(_logicalDevice);
			if (result != VK_SUCCESS) {
				fprintf(stderr, "vkDeviceWaitIdle returned %d, don't know how to proceed.\n", result);
				exit(-1);
			}

			deinitialize(STATE_GRAPHICS_PIPELINE_CREATED);
		} else {
			deinitialize(STATE_UNINITIALIZED);
		}
	}

	virtual void renderFrame() override {
		if (_logicalDevice == nullptr) {
			// recreateSwapChain() failed?
			// TODO: attempt to initialize() manually?
			return;
		}

		VkResult result = vkWaitForFences(_logicalDevice, 1, &_inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX);
		if (result != VK_SUCCESS) {
			return;
		}

    uint32_t imageIndex;
    result = vkAcquireNextImageKHR(_logicalDevice, _swapChain, UINT64_MAX, _imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
      recreateSwapChain();
      return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    	// TODO: implement.
     return;
    }

    result = vkResetFences(_logicalDevice, 1, &_inFlightFences[_currentFrame]);
    if (result != VK_SUCCESS) {
    	return;
    }

    result = vkResetCommandBuffer(_commandBuffers[_currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    if (result != VK_SUCCESS) {
    	return;
    }
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

    result = vkQueueSubmit(_graphicsQueue, 1, &submitInfo, _inFlightFences[_currentFrame]);
    if (result != VK_SUCCESS) {
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
      framebufferResized = false;
      recreateSwapChain();
      return;
    } else if (result != VK_SUCCESS) {
      return;
    }

    _currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	virtual void resize(float windowWidth, float windowHeight) override {}

private:
	bool recreateSwapChain() {
		VkResult result = vkDeviceWaitIdle(_logicalDevice);
		if (result != VK_SUCCESS) {
			fprintf(stderr, "vkDeviceWaitIdle returned %d, don't know how to proceed.\n", result);
			exit(-1);
		}

		deinitializeGraphicsPipeline();
		deinitializePipelineLayout();
		deinitializeFramebuffers();
		deinitializeRenderPass();
		deinitializeSwapChainImageViews();
		deinitializeSwapChain();

		CHECK_CURRENT_STATE(STATE_SYNC_OBJECTS_CREATED);
    if (!createSwapChain()) {
    	return deinitialize(STATE_SYNC_OBJECTS_CREATED);
    }

	  CHECK_CURRENT_STATE(STATE_SWAP_CHAIN_CREATED);
	  if (!createSwapChainImageViews()) {
	  	return deinitialize(STATE_SWAP_CHAIN_CREATED);
	  }

	  CHECK_CURRENT_STATE(STATE_SWAP_CHAIN_IMAGE_VIEWS_CREATED);
	  if (!createRenderPass()) {
	  	return deinitialize(STATE_SWAP_CHAIN_IMAGE_VIEWS_CREATED);
	  }

	  CHECK_CURRENT_STATE(STATE_RENDER_PASS_CREATED);
	  if (!createFramebuffers()) {
	  	return deinitialize(STATE_RENDER_PASS_CREATED);
	  }

	  CHECK_CURRENT_STATE(STATE_SWAP_CHAIN_FRAMEBUFFERS_CREATED);
	  if (!createPipelineLayout()) {
	  	return deinitialize(STATE_SWAP_CHAIN_FRAMEBUFFERS_CREATED);
	  }

	  CHECK_CURRENT_STATE(STATE_PIPELINE_LAYOUT_CREATED);
	  if (!createGraphicsPipeline()) {
	  	return deinitialize(STATE_PIPELINE_LAYOUT_CREATED);
	  }

	  CHECK_CURRENT_STATE(STATE_GRAPHICS_PIPELINE_CREATED);
	  return true;
	}

	bool recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo {
    	.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    };

    VkResult result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    if (result != VK_SUCCESS) {
      return false;
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

    result = vkEndCommandBuffer(commandBuffer);
    if (result != VK_SUCCESS) {
    	return false;
    }

    return true;
	}

	bool advanceState(int newState, const char* function) {
#if !defined NDEBUG
		if (newState != _currentState + 1) {
			fprintf(stderr, "Cannot advance from state %d to state %d\n", _currentState, newState);
			exit(-1);
		}

		_currentState = newState;
		checkInvariant(function);
#endif
		return true;
	}

	bool initializeLayers() {
#if defined NDEBUG
		return true;
#else
		CHECK_CURRENT_STATE(STATE_UNINITIALIZED);

		uint32_t layerCount;
    VkResult result = POINT_OF_FAILURE(vkEnumerateInstanceLayerProperties)(&layerCount, nullptr);
    if (result != VK_SUCCESS) {
    	return false;
    }

    if (layerCount != 0) {
    	VkLayerProperties* props = (VkLayerProperties*) malloc(layerCount * sizeof(VkLayerProperties));
	    result = POINT_OF_FAILURE(vkEnumerateInstanceLayerProperties)(&layerCount, props);
	    if (result != VK_SUCCESS) {
	    	free(props);
	    	return false;
	    }

    	uint32_t enabledLayerCount = 0;
	    const char** enabledLayerNames = (const char**) malloc(layerCount * sizeof(const char*));
	    
	    // Enable all except blackisted layers that we are not interested in.
	    for (uint32_t i = 0; i < layerCount; ++i) {
	    	const char* layerName = props[i].layerName;
	    	if (strcmp("VK_LAYER_LUNARG_device_simulation", layerName) == 0 ||
	    			strcmp("VK_LAYER_LUNARG_gfxreconstruct", layerName) == 0) {
	    		continue;
	    	}

	    	enabledLayerNames[enabledLayerCount] = strdup(layerName);
	    	++enabledLayerCount;
	    }

	    if (enabledLayerCount == 0) {
	    	free(enabledLayerNames);
	    } else {
	    	_enabledLayerCount = enabledLayerCount;
	    	_ppEnabledLayerNames = enabledLayerNames;
	    }

	    free(props);
	  }

    return advanceState(STATE_LAYERS_INITIALIZED, __FUNCTION__);
#endif
	}

	bool createInstance() {
		CHECK_CURRENT_STATE(STATE_LAYERS_INITIALIZED);

		VkApplicationInfo appInfo {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pApplicationName = "Hello Triangle",
      .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
      .pEngineName = "No Engine",
      .engineVersion = VK_MAKE_VERSION(1, 0, 0),
      .apiVersion = VK_API_VERSION_1_1,
		};

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
    	.enabledLayerCount = _enabledLayerCount,
    	.ppEnabledLayerNames = _ppEnabledLayerNames,
    	.enabledExtensionCount = enabledExtensionCount,
    	.ppEnabledExtensionNames = ppEnabledExtensionNames,
    };

    VkResult result = POINT_OF_FAILURE(vkCreateInstance)(&createInfo, nullptr, &_instance);
    if (result != VK_SUCCESS) {
    	return false;
    }

    return advanceState(STATE_INSTANCE_CREATED, __FUNCTION__);
	}

	bool createSurface() {
		CHECK_CURRENT_STATE(STATE_INSTANCE_CREATED);
    if (!_applicationController->createSurface(_instance, &_surface)) {
    	return false;
    }

    return advanceState(STATE_SURFACE_CREATED, __FUNCTION__);
	}

	bool pickPhysicalDevice() {
		CHECK_CURRENT_STATE(STATE_SURFACE_CREATED);
		uint32_t deviceCount = 0;
		VkResult result = POINT_OF_FAILURE(vkEnumeratePhysicalDevices)(_instance, &deviceCount, nullptr);
		if (result != VK_SUCCESS) {
			return false;
		}

		if (deviceCount == 0) {
      return false;
    }

		VkPhysicalDevice* devices = (VkPhysicalDevice*) malloc(deviceCount * sizeof(VkPhysicalDevice));
    result = POINT_OF_FAILURE(vkEnumeratePhysicalDevices)(_instance, &deviceCount, devices);
    if (result != VK_SUCCESS) {
    	return false;
    }

    // For now, skip capabilities check and just return the first device available.
		// deviceCount *must* be set to 1 so that we don't overwrite memory beyond _physicalDevice.
    _physicalDevice = devices[0];
    free(devices);

    return advanceState(STATE_PHYSICAL_DEVICE_CREATED, __FUNCTION__);
	}

	bool createLogicalDevice() {
		CHECK_CURRENT_STATE(STATE_PHYSICAL_DEVICE_CREATED);
		uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, nullptr);

    VkQueueFamilyProperties* props = (VkQueueFamilyProperties*) malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, props);

    uint32_t queueFamilyIndexCount = 2;
    VkDeviceQueueCreateInfo queueCreateInfos[2] = {};

    bool foundGraphicsQueue = false;
    bool foundPresentationQueue = false;

    float queuePriority = 1.0f;
    uint32_t queueFamilyIndices[2];
    for (uint32_t i = 0; i < queueFamilyCount; ++i) {
    	VkQueueFamilyProperties& prop = props[i]; 
    	bool supportsGraphics = (prop.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0;

    	VkBool32 supportsPresentation = false;
      VkResult result = POINT_OF_FAILURE(vkGetPhysicalDeviceSurfaceSupportKHR)(_physicalDevice, i, _surface, &supportsPresentation);
      if (result != VK_SUCCESS) {
      	return false;
      }

      if (supportsGraphics) {
      	foundGraphicsQueue = true;

      	if (supportsPresentation) {
      		foundPresentationQueue = true;

      		// Found a queue that supports both. Prefer one queue that can do both.
	        queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	        queueCreateInfos[0].queueFamilyIndex = i;
	        queueCreateInfos[0].queueCount = 1;
	        queueCreateInfos[0].pQueuePriorities = &queuePriority;
	        queueFamilyIndices[0] = i;
	        queueFamilyIndices[1] = i;
	        queueFamilyIndexCount = 1;
	      	break;
      	}

      	foundGraphicsQueue = true;
      	queueFamilyIndices[0] = i;
    		queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfos[0].queueFamilyIndex = i;
        queueCreateInfos[0].queueCount = 1;
        queueCreateInfos[0].pQueuePriorities = &queuePriority;
      } else if (supportsPresentation) {
      	foundPresentationQueue = true;
      	queueFamilyIndices[1] = i;
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
    VkResult result = POINT_OF_FAILURE(vkEnumerateDeviceExtensionProperties)(_physicalDevice, nullptr, &extensionCount, nullptr);
    if (result != VK_SUCCESS) {
    	return false;
    }

    VkExtensionProperties* extProps = (VkExtensionProperties*) malloc(extensionCount * sizeof(VkExtensionProperties));
    result = POINT_OF_FAILURE(vkEnumerateDeviceExtensionProperties)(_physicalDevice, nullptr, &extensionCount, extProps);
    if (result != VK_SUCCESS) {
    	return false;
    }

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
    free(extProps);

    VkPhysicalDeviceFeatures deviceFeatures{};
    VkDeviceCreateInfo createInfo {
    	.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
	    .queueCreateInfoCount = queueFamilyIndexCount,
	    .pQueueCreateInfos = queueCreateInfos,
	    .enabledLayerCount = _enabledLayerCount,
	    .ppEnabledLayerNames = _ppEnabledLayerNames,
	    .enabledExtensionCount = enabledExtensionCount,
	    .ppEnabledExtensionNames = ppEnabledExtensionNames,
	    .pEnabledFeatures = &deviceFeatures,
    };

    result = POINT_OF_FAILURE(vkCreateDevice)(_physicalDevice, &createInfo, nullptr, &_logicalDevice);
    if (result != VK_SUCCESS) {
    	return false;
    }

    _queueFamilyIndexCount = queueFamilyIndexCount;
    _queueFamilyIndices[0] = queueFamilyIndices[0];
    _queueFamilyIndices[1] = queueFamilyIndices[1];

    vkGetDeviceQueue(_logicalDevice, queueFamilyIndices[0], 0, &_graphicsQueue);

    if (queueFamilyIndexCount == 1) {
    	_presentationQueue = _graphicsQueue;
    } else {
    	vkGetDeviceQueue(_logicalDevice, _queueFamilyIndices[1], 0, &_presentationQueue);	
    }
    
    return advanceState(STATE_LOGICAL_DEVICE_CREATED, __FUNCTION__);
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
    fprintf(stderr, "ERROR#8\n");
    fflush(stderr);
    exit(-1);
	}

	bool createSwapChain() {
		CHECK_CURRENT_STATE(STATE_SYNC_OBJECTS_CREATED);

    uint32_t formatCount;
    VkResult result = POINT_OF_FAILURE(vkGetPhysicalDeviceSurfaceFormatsKHR)(_physicalDevice, _surface, &formatCount, nullptr);
    if (result != VK_SUCCESS) {
    	return false;
    }

    if (formatCount == 0) {
    	return false;
    }

    VkSurfaceFormatKHR* formats = (VkSurfaceFormatKHR*) malloc(formatCount * sizeof(VkSurfaceFormatKHR));
    result = POINT_OF_FAILURE(vkGetPhysicalDeviceSurfaceFormatsKHR)(_physicalDevice, _surface, &formatCount, formats);
    if (result != VK_SUCCESS) {
    	return false;
    }

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(formats, formatCount);
    free(formats);

    uint32_t presentModeCount;
    result = POINT_OF_FAILURE(vkGetPhysicalDeviceSurfacePresentModesKHR)(_physicalDevice, _surface, &presentModeCount, nullptr);
    if (result != VK_SUCCESS) {
    	return false;
    }

    if (presentModeCount == 0) {
    	return false;
    }

    VkPresentModeKHR* presentModes = (VkPresentModeKHR*) malloc(presentModeCount * sizeof(VkPresentModeKHR));
    result = POINT_OF_FAILURE(vkGetPhysicalDeviceSurfacePresentModesKHR)(_physicalDevice, _surface, &presentModeCount, presentModes);
    if (result != VK_SUCCESS) {
    	return false;
    }

    VkPresentModeKHR presentMode = chooseSwapPresentMode(presentModes, presentModeCount);
    free(presentModes);

    VkSurfaceCapabilitiesKHR capabilities;
    result = POINT_OF_FAILURE(vkGetPhysicalDeviceSurfaceCapabilitiesKHR)(_physicalDevice, _surface, &capabilities);
    if (result != VK_SUCCESS) {
    	return false;
    }

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

    result = POINT_OF_FAILURE(vkCreateSwapchainKHR)(_logicalDevice, &createInfo, nullptr, &_swapChain);
    if (result != VK_SUCCESS) {
    	return false;
    }

    _swapChainImageFormat = surfaceFormat.format;
    _swapChainExtent = extent;

    return advanceState(STATE_SWAP_CHAIN_CREATED, __FUNCTION__);
	}

	bool createSwapChainImageViews() {
		CHECK_CURRENT_STATE(STATE_SWAP_CHAIN_CREATED);

		uint32_t swapChainImageCount;
		VkResult result = POINT_OF_FAILURE(vkGetSwapchainImagesKHR)(_logicalDevice, _swapChain, &swapChainImageCount, nullptr);
		if (result != VK_SUCCESS) {
    	return false;
    }

		VkImage* swapChainImages = (VkImage*) malloc(swapChainImageCount * sizeof(VkImage));
    result = POINT_OF_FAILURE(vkGetSwapchainImagesKHR)(_logicalDevice, _swapChain, &swapChainImageCount, swapChainImages);
    if (result != VK_SUCCESS) {
    	return false;
    }
		
		VkImageView* swapChainImageViews = (VkImageView*) malloc(swapChainImageCount * sizeof(VkImageView));
		VkImageViewCreateInfo createInfo {
     	.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
     	.viewType = VK_IMAGE_VIEW_TYPE_2D,
     	.format = _swapChainImageFormat,
     	.components = {
     		.r = VK_COMPONENT_SWIZZLE_IDENTITY,
     		.g = VK_COMPONENT_SWIZZLE_IDENTITY,
     		.b = VK_COMPONENT_SWIZZLE_IDENTITY,
     		.a = VK_COMPONENT_SWIZZLE_IDENTITY,
     	},
     	.subresourceRange = {
     		.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
	     	.baseMipLevel = 0,
	     	.levelCount = 1,
	     	.baseArrayLayer = 0,
	     	.layerCount = 1,
     	},
    };

    for (uint32_t i = 0; i < swapChainImageCount; i++) {
        createInfo.image = swapChainImages[i];

        VkResult result = POINT_OF_FAILURE(vkCreateImageView)(_logicalDevice, &createInfo, nullptr, &swapChainImageViews[i]);
        if (result != VK_SUCCESS) {
        	// Destroy all swapChain ImageViews that we may have created.
        	for (uint32_t j = 0; j < i; ++j) {
        		vkDestroyImageView(_logicalDevice, swapChainImageViews[j], nullptr);
        	}
        	free(swapChainImageViews);
        	free(swapChainImages);
          return false;
        }
    }

    free(swapChainImages);

    _swapChainImageCount = swapChainImageCount;
    _swapChainImageViews = swapChainImageViews;
    return advanceState(STATE_SWAP_CHAIN_IMAGE_VIEWS_CREATED, __FUNCTION__);
	}

	bool createRenderPass() {
		CHECK_CURRENT_STATE(STATE_SWAP_CHAIN_IMAGE_VIEWS_CREATED);

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
	    .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	    .srcAccessMask = 0,
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

    VkResult result = POINT_OF_FAILURE(vkCreateRenderPass)(_logicalDevice, &renderPassInfo, nullptr, &_renderPass);
    if (result != VK_SUCCESS) {
    	return false;
    }

    return advanceState(STATE_RENDER_PASS_CREATED, __FUNCTION__);
	}

	bool createShaderModule(const char* fileName, VkShaderModule* shaderModule) {
		uint64_t codeSize;
		void* code = _applicationController->readFile(fileName, &codeSize);

    VkShaderModuleCreateInfo createInfo {
    	.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .codeSize = codeSize,
      .pCode = reinterpret_cast<const uint32_t*>(code),
    };

    VkResult result = POINT_OF_FAILURE(vkCreateShaderModule)(_logicalDevice, &createInfo, nullptr, shaderModule);
    if (result != VK_SUCCESS) {
    	return false;
    }
    free(code);

    return true;
  }

  bool createPipelineLayout() {
  	CHECK_CURRENT_STATE(STATE_SWAP_CHAIN_FRAMEBUFFERS_CREATED);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo {
    	.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    	.setLayoutCount = 0,
    	.pushConstantRangeCount = 0,
    };

    VkResult result = POINT_OF_FAILURE(vkCreatePipelineLayout)(_logicalDevice, &pipelineLayoutInfo, nullptr, &_pipelineLayout);
    if (result != VK_SUCCESS) {
    	return false;
    }

    return advanceState(STATE_PIPELINE_LAYOUT_CREATED, __FUNCTION__);
  }

	bool createGraphicsPipeline() {
		CHECK_CURRENT_STATE(STATE_PIPELINE_LAYOUT_CREATED);

    VkShaderModule vertShaderModule;
		if (!createShaderModule("vert.spv", &vertShaderModule)) {
			return false;
		}

		VkShaderModule fragShaderModule;
		if (!createShaderModule("frag.spv", &fragShaderModule)) {
			vkDestroyShaderModule(_logicalDevice, vertShaderModule, nullptr);
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
	    .width = static_cast<float>(_swapChainExtent.width),
	    .height = static_cast<float>(_swapChainExtent.height),
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
    	.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    	.sampleShadingEnable = VK_FALSE,
    };

    VkPipelineRasterizationStateCreateInfo rasterizer {
    	.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
	    .depthClampEnable = VK_FALSE,
	    .rasterizerDiscardEnable = VK_FALSE,
	    .polygonMode = VK_POLYGON_MODE_FILL,
	    .cullMode = VK_CULL_MODE_BACK_BIT,
	    .frontFace = VK_FRONT_FACE_CLOCKWISE,
	    .depthBiasEnable = VK_FALSE,
	    .lineWidth = 1.0f,
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssembly {
    	.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
    	.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    	.primitiveRestartEnable = VK_FALSE,
    };

    VkPipelineColorBlendAttachmentState colorBlendAttachment {
    	.blendEnable = VK_FALSE,
    	.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
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
	    .layout = _pipelineLayout,
	    .renderPass = _renderPass,
	    .subpass = 0,
	    .basePipelineHandle = VK_NULL_HANDLE,
    };
    
    VkResult result = POINT_OF_FAILURE(vkCreateGraphicsPipelines)(_logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_graphicsPipeline);
    vkDestroyShaderModule(_logicalDevice, fragShaderModule, nullptr);
    vkDestroyShaderModule(_logicalDevice, vertShaderModule, nullptr);
    if (result != VK_SUCCESS) {
    	return false;
    }

    return advanceState(STATE_GRAPHICS_PIPELINE_CREATED, __FUNCTION__);
	}

	bool createFramebuffers() {
		CHECK_CURRENT_STATE(STATE_RENDER_PASS_CREATED);

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

    return advanceState(STATE_SWAP_CHAIN_FRAMEBUFFERS_CREATED, __FUNCTION__);
	}

	bool createCommandPool() {
		CHECK_CURRENT_STATE(STATE_LOGICAL_DEVICE_CREATED);
		VkCommandPoolCreateInfo poolInfo {
	    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
	    .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
	    .queueFamilyIndex = _queueFamilyIndices[0],
	  };

    if (vkCreateCommandPool(_logicalDevice, &poolInfo, nullptr, &_commandPool) != VK_SUCCESS) {
    	return false;
    }
    return advanceState(STATE_COMMAND_POOL_CREATED, __FUNCTION__);
	}

	bool createCommandBuffers() {
		CHECK_CURRENT_STATE(STATE_COMMAND_POOL_CREATED);

    VkCommandBufferAllocateInfo allocInfo {
    	.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
	    .commandPool = _commandPool,
	    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
	    .commandBufferCount = MAX_FRAMES_IN_FLIGHT,
    };

    if (vkAllocateCommandBuffers(_logicalDevice, &allocInfo, _commandBuffers) != VK_SUCCESS) {
    	return false;
    }

    return advanceState(STATE_COMMAND_BUFFERS_CREATED, __FUNCTION__);
	}

	bool createSyncObjects() {
		CHECK_CURRENT_STATE(STATE_COMMAND_BUFFERS_CREATED);

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

    return advanceState(STATE_SYNC_OBJECTS_CREATED, __FUNCTION__);
	}

	enum {
		MAX_FRAMES_IN_FLIGHT = 2,
	};

	enum {
		STATE_UNINITIALIZED,
		STATE_LAYERS_INITIALIZED,
		STATE_INSTANCE_CREATED,
		STATE_SURFACE_CREATED,
		STATE_PHYSICAL_DEVICE_CREATED,
		STATE_LOGICAL_DEVICE_CREATED,
		STATE_COMMAND_POOL_CREATED,
		STATE_COMMAND_BUFFERS_CREATED,
		STATE_SYNC_OBJECTS_CREATED,
		STATE_SWAP_CHAIN_CREATED,
		STATE_SWAP_CHAIN_IMAGE_VIEWS_CREATED,
		STATE_RENDER_PASS_CREATED,
		STATE_SWAP_CHAIN_FRAMEBUFFERS_CREATED,
		STATE_PIPELINE_LAYOUT_CREATED,
		STATE_GRAPHICS_PIPELINE_CREATED,
	};

	ApplicationController* _applicationController;

#if !defined NDEBUG
	uint32_t _currentState {STATE_UNINITIALIZED};
	PointOfFailure _pointOfFailure { NO_FAILURE };
#endif

	VkInstance _instance {nullptr};
	VkSurfaceKHR _surface {nullptr};
	VkPhysicalDevice _physicalDevice {nullptr};
	VkDevice _logicalDevice {nullptr};
	VkQueue _graphicsQueue { nullptr };
  VkQueue _presentationQueue { nullptr };
	VkSwapchainKHR _swapChain {nullptr};
	VkImageView* _swapChainImageViews {nullptr};
	VkFormat _swapChainImageFormat {VK_FORMAT_UNDEFINED};
	VkExtent2D _swapChainExtent {0xFFFFFFFF, 0xFFFFFFFF};
	VkRenderPass _renderPass {nullptr};
	VkPipelineLayout _pipelineLayout {nullptr};
	VkPipeline _graphicsPipeline {nullptr};
	VkFramebuffer* _swapChainFramebuffers {nullptr};
	VkCommandPool _commandPool {nullptr};
	VkCommandBuffer _commandBuffers[MAX_FRAMES_IN_FLIGHT] {
		nullptr,
		nullptr,
	};
	VkSemaphore _imageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT] {
		nullptr,
		nullptr,
	};
  VkSemaphore _renderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT] {
  	nullptr,
  	nullptr,
  };
  VkFence _inFlightFences[MAX_FRAMES_IN_FLIGHT] {
  	nullptr,
  	nullptr,
  };
	uint32_t _swapChainImageCount {0xFFFFFFFF};
	uint32_t _queueFamilyIndexCount {0xFFFFFFFF};
	uint32_t _queueFamilyIndices[2] {0xFFFFFFFF, 0xFFFFFFFF};
	uint32_t _enabledLayerCount {0};
  const char** _ppEnabledLayerNames {nullptr};
};

Application* createApplication() {
	return new MyApplication();
}
