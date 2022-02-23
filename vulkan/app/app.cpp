#include "vulkan/platform/app/Application.h"

#include <vulkan/vulkan.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class MyApplication : public Application {
public:
	virtual bool init(ApplicationController* applicationController) override {
		_applicationController = applicationController;
	  return true;
	}

	virtual void term() override {}
	virtual bool initGraphics() override {
	  if (!createInstance() || !createSurface() || !pickPhysicalDevice() || !createLogicalDevice()) {
	  	return false;
	  }

	  printf("init done.\n");
	  return false;
	}
	virtual void termGraphics() override {}
	virtual void renderFrame() override {}
	virtual void resize(float windowWidth, float windowHeight) override {}

private:
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

    uint32_t enabledLayerCount = 0;
    const char** ppEnabledLayerNames = (const char**) malloc(layerCount * sizeof(const char*));
    for (uint32_t i = 0; i < layerCount; ++i) {
    	const char* layerName = props[i].layerName;
    	if (strcmp("VK_LAYER_LUNARG_device_simulation", layerName) != 0) {
    		ppEnabledLayerNames[enabledLayerCount] = layerName;
    		++enabledLayerCount;
    	}
    }

    const char* ppEnabledExtensionNames[] = {
    	// Either of these two is required to create a metal surface.
    	"VK_EXT_metal_surface",
    	// "VK_MVK_macos_surface", // Deprecated.

    	// Required by VK_EXT_metal_surface and VK_MVK_macos_surface.
    	"VK_KHR_surface",
    };
    VkInstanceCreateInfo createInfo {
    	.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    	.pApplicationInfo = &appInfo,
    	.enabledLayerCount = enabledLayerCount,
    	.ppEnabledLayerNames = ppEnabledLayerNames,
    	.enabledExtensionCount = 2,
    	.ppEnabledExtensionNames = ppEnabledExtensionNames,
    };

    VkResult result = vkCreateInstance(&createInfo, nullptr, &_instance);
    free(props);
    free(ppEnabledLayerNames);

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
    vkEnumeratePhysicalDevices(_instance, &deviceCount, &_device);
    return true;
	}

	bool createLogicalDevice() {
		// TODO: implement.
		return false;
	}

	ApplicationController* _applicationController;
	VkInstance _instance;
	VkSurfaceKHR _surface;
	VkPhysicalDevice _device;
};

Application* createApplication() {
	return new MyApplication();
}
