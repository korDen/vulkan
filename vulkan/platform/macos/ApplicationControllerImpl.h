#pragma once

#import <AppKit/AppKit.h>
#include "vulkan/platform/app/Application.h"

class ApplicationControllerImpl : ApplicationController {
public:
	void init(NSWindow* window, NSView* view);
	void prepareGraphics();
	void resize(CGFloat windowWidth, CGFloat windowHeight);
	bool renderFrame();
	void cleanup();

private:
	NSWindow* _window;
	NSView* _view;
	Application* _app;
  bool _canRender;
  bool _drawing;
  bool _redrawScheduled;

  friend class ApplicationController;
};
