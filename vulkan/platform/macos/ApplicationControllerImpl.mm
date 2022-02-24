#include "ApplicationControllerImpl.h"

#import <QuartzCore/QuartzCore.h>

#include <vulkan/vulkan.h>

void ApplicationControllerImpl::init(NSWindow* window, NSView* view) {
  _window = window;
  _view = view;
  _app = createApplication();

  _app->init(this);
}

void ApplicationControllerImpl::prepareGraphics() {
	_canRender = _app->initGraphics();
}

void ApplicationControllerImpl::resize(CGFloat windowWidth, CGFloat windowHeight) {
	_app->resize(static_cast<float>(windowWidth), static_cast<float>(windowHeight));
}

bool ApplicationControllerImpl::renderFrame() {
  if (!_canRender) {
    return false;
  }

  _redrawScheduled = false;
  _drawing = true;
  _app->renderFrame();
  _drawing = false;

  return true;
}

void ApplicationControllerImpl::cleanup() {
  if (_canRender) {
    _app->termGraphics();
  }
  _app->term();

  delete _app;
}

void ApplicationController::invalidate() {
  ApplicationControllerImpl* impl = (ApplicationControllerImpl*) this;
  if (impl->_redrawScheduled) {
    return;
  }

  impl->_redrawScheduled = true;
  if (impl->_drawing) {
    dispatch_async(dispatch_get_main_queue(), ^{
      [impl->_view setNeedsDisplay:YES];
    });
  } else {
    [impl->_view setNeedsDisplay:YES];
  }
}

void ApplicationController::quit() {
  ApplicationControllerImpl* impl = (ApplicationControllerImpl*) this;
  [[NSApplication sharedApplication] terminate:impl->_view];
}

#define APIENTRY

typedef VkFlags VkMetalSurfaceCreateFlagsEXT;
typedef struct VkMetalSurfaceCreateInfoEXT
{
    VkStructureType                 sType;
    const void*                     pNext;
    VkMetalSurfaceCreateFlagsEXT    flags;
    const void*                     pLayer;
} VkMetalSurfaceCreateInfoEXT;

typedef VkResult (APIENTRY *PFN_vkCreateMetalSurfaceEXT)(VkInstance,const VkMetalSurfaceCreateInfoEXT*,const VkAllocationCallbacks*,VkSurfaceKHR*);

bool createMetalSurface(VkInstance instance, CALayer* layer, VkSurfaceKHR* surface) {
	PFN_vkCreateMetalSurfaceEXT vkCreateMetalSurfaceEXT = (PFN_vkCreateMetalSurfaceEXT) vkGetInstanceProcAddr(instance, "vkCreateMetalSurfaceEXT");
    if (vkCreateMetalSurfaceEXT == nullptr) {
      // Most likely the extension is disabled.
    	return false;
    }

    VkMetalSurfaceCreateInfoEXT sci = {
      .sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT,
      .pNext = nullptr,
      .flags = 0,
      .pLayer = (__bridge void*) layer,
    };

    return vkCreateMetalSurfaceEXT(instance, &sci, nullptr, surface) == VK_SUCCESS;
}

typedef VkFlags VkMacOSSurfaceCreateFlagsMVK;

typedef struct VkMacOSSurfaceCreateInfoMVK
{
    VkStructureType                 sType;
    const void*                     pNext;
    VkMacOSSurfaceCreateFlagsMVK    flags;
    const void*                     pView;
} VkMacOSSurfaceCreateInfoMVK;

typedef VkResult (APIENTRY *PFN_vkCreateMacOSSurfaceMVK)(VkInstance,const VkMacOSSurfaceCreateInfoMVK*,const VkAllocationCallbacks*,VkSurfaceKHR*);

bool createMetalSurfaceFallback(VkInstance instance, CALayer* layer, VkSurfaceKHR* surface) {
	VkMacOSSurfaceCreateInfoMVK sci {
		.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK,
		.pView = (__bridge void*) layer,
	};

    PFN_vkCreateMacOSSurfaceMVK vkCreateMacOSSurfaceMVK = (PFN_vkCreateMacOSSurfaceMVK) vkGetInstanceProcAddr(instance, "vkCreateMacOSSurfaceMVK");
    if (vkCreateMacOSSurfaceMVK == nullptr) {
    	return false;
    }

    return vkCreateMacOSSurfaceMVK(instance, &sci, nullptr, surface) == VK_SUCCESS;
}

bool ApplicationController::createSurface(VkInstance instance, VkSurfaceKHR* surface) {
	ApplicationControllerImpl* impl = (ApplicationControllerImpl*) this;
	CALayer* layer = [[CAMetalLayer alloc] init];
	[layer setContentsScale:[impl->_window backingScaleFactor]];

	NSView* view = impl->_view;
	[view setLayer:layer];
  [view setWantsLayer:YES];

  return createMetalSurface(instance, layer, surface) || createMetalSurfaceFallback(instance, layer, surface);
}

struct foo{};

void* ApplicationController::readFile(const char* fileName, uint64_t* size) {
  NSString* resourcePath = [[NSBundle mainBundle] resourcePath];
  NSString* fullPath = [resourcePath stringByAppendingPathComponent:[NSString stringWithUTF8String:fileName]];
  int fd = open([fullPath UTF8String], O_RDONLY);
  if (fd == -1) {
    return nullptr;
  }

  off_t fileSize = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);

  void* data = malloc(fileSize);
  ssize_t bytesRead = read(fd, data, fileSize);
  close(fd);

  if (bytesRead != fileSize) {
    return nullptr;
  }

  *size = fileSize;
  return data;
}
