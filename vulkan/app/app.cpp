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
	  if (!createInstance()) {
	  	fprintf(stderr, "failed to create instance.");
	  	return false;
	  }

	  if (!createSurface()) {
	  	fprintf(stderr, "failed to create surface.");
	  	return false;
	  }

	  if (!pickPhysicalDevice()) {
	  	fprintf(stderr, "failed to pick physical device.");
	  	return false;
	  }

	   if (!createLogicalDevice()) {
	  	fprintf(stderr, "failed to create logical device.");
	  	return false;
	  }

	  fprintf(stderr, "init done.");
	  return true;
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

    uint32_t enabledExtensionCount = 2;
    const char* ppEnabledExtensionNames[] = {
#if defined OS_MACOSX
    	"VK_EXT_metal_surface",
#elif defined _WIN32
    	"VK_KHR_win32_surface",
#elif defined LINUX
    	"VK_KHR_xcb_surface",
#endif
    	// Always required to create a surface.
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
		return true;
	}

	ApplicationController* _applicationController;
	VkInstance _instance;
	VkSurfaceKHR _surface;
	VkPhysicalDevice _device;
};

Application* createApplication() {
	return new MyApplication();
}
