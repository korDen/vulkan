#if defined OS_MACOSX
#if defined PLATFORM_MACOS_USE_OPENGL

#import <QuartzCore/QuartzCore.h>
#include <AppKit/NSApplication.h>

#include "vulkan/platform/app/Application.h"
#include "vulkan/platform/macos/OpenGLWindow.h"
#include "vulkan/platform/macos/ApplicationControllerImpl.h"

@implementation OpenGLWindow {
  ApplicationControllerImpl _applicationControllerImpl;
}

- (void)quit {
  _applicationControllerImpl.cleanup();
}

- (void)setupApplicationWithWindow: (NSWindow*) window {
  _applicationControllerImpl.init(window, self);
}

- (void)prepareOpenGL {
  [super prepareOpenGL];

  // Enables double buffering.
  GLint swapInterval = 1;
  [[self openGLContext] setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];

  // Enables hidpi rendering.
  [self setWantsBestResolutionOpenGLSurface:YES];

  _applicationControllerImpl.prepareGraphics();
}

- (void)drawRect:(NSRect)dirtyRect {
  if (_applicationControllerImpl.renderFrame()) {
    [[self openGLContext] flushBuffer];
  }
}

- (void)reshape {
  [super reshape];

  NSRect frame = [self convertRectToBacking:[self bounds]];
  _applicationControllerImpl.resize(frame.size.width, frame.size.height);
  [self setNeedsDisplay:YES];
}

- (BOOL)acceptsFirstResponder {
  return YES;
}

@end

#endif // defined PLATFORM_MACOS_USE_OPENGL
#endif // defined OS_MACOSX
