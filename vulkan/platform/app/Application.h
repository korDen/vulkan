#pragma once

class ApplicationController {
 public:
  ApplicationController(void* state) : _state(state) {}

  void invalidate();
  void quit();

 private:
  void* _state;
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
