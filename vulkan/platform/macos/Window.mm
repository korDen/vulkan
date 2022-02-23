#if defined OS_MACOSX

#include <AppKit/NSApplication.h>

#include "vulkan/platform/app/Application.h"
#include "vulkan/platform/macos/Window.h"

struct ApplicationControllerState {
  Window* window;
};

@implementation Window {
  ApplicationControllerState* _applicationControllerState;
  ApplicationController* _applicationController;
  Application* _app;
  NSTimer* _timer;
  bool _canRender;
  bool _drawing;
  bool _redrawScheduled;
}

void ApplicationController::invalidate() {
  ApplicationControllerState* state = (ApplicationControllerState*)_state;
  Window* window = state->window;
  if (window->_redrawScheduled) {
    return;
  }

  window->_redrawScheduled = true;
  if (window->_drawing) {
    dispatch_async(dispatch_get_main_queue(), ^{
      [window setNeedsDisplay:YES];
    });
  } else {
    [window setNeedsDisplay:YES];
  }
}

void ApplicationController::quit() {
  ApplicationControllerState* state = (ApplicationControllerState*)_state;
  [[NSApplication sharedApplication] terminate:state->window];
}

- (void)quit {
  if (_canRender) {
    _app->termGraphics();
  }
  _app->term();
  
  delete _applicationControllerState;
  delete _applicationController;
  delete _app;
}

- (instancetype)initWithFrame:(NSRect)frame pixelFormat:(NSOpenGLPixelFormat*)format {
  if (self = [super initWithFrame:frame pixelFormat:format]) {
    _app = createApplication();

    _applicationControllerState = new ApplicationControllerState();
    _applicationControllerState->window = self;

    _applicationController = new ApplicationController(_applicationControllerState);
    _app->init(_applicationController);
  }
  return self;
}

- (void)prepareOpenGL {
  [super prepareOpenGL];

  // Enables double buffering.
  GLint swapInterval = 1;
  [[self openGLContext] setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];

  // Enables hidpi rendering.
  [self setWantsBestResolutionOpenGLSurface:YES];

  _canRender = _app->initGraphics();
}

- (void)drawRect:(NSRect)dirtyRect {
  if (!_canRender) {
    return;
  }

  _redrawScheduled = false;
  _drawing = true;
  _app->renderFrame();
  _drawing = false;

  [[self openGLContext] flushBuffer];
}

- (void)reshape {
  [super reshape];

  NSRect frame = [self convertRectToBacking:[self bounds]];
  _app->resize(static_cast<float>(frame.size.width), static_cast<float>(frame.size.height));
  [self setNeedsDisplay:YES];
}

- (BOOL)acceptsFirstResponder {
  return YES;
}

@end

#endif  // defined OS_MACOSX
