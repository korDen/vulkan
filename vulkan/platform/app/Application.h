#pragma once

#include <vulkan/vulkan.h>

class ApplicationController {
 public:
  void invalidate();
  void quit();
  bool createSurface(VkInstance instance, VkSurfaceKHR* surface);
  void* readFile(const char* fileName, uint64_t* size);
};

class Application {
 public:
  virtual ~Application() {}

  virtual bool init(ApplicationController* applicationController) {
    return false;
  }
  virtual void term() {}
  virtual bool initGraphics() { return false; }
  virtual void termGraphics() {}
  virtual void renderFrame() {}
  virtual void resize(float windowWidth, float windowHeight) {}
};

Application* createApplication();
