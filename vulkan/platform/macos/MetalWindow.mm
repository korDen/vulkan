#if defined OS_MACOSX
#if defined PLATFORM_MACOS_USE_METAL

#import <QuartzCore/QuartzCore.h>
#include <AppKit/NSApplication.h>

#include "vulkan/platform/app/Application.h"
#include "vulkan/platform/macos/MetalWindow.h"
#include "vulkan/platform/macos/ApplicationControllerImpl.h"

@implementation MetalWindow {
  ApplicationControllerImpl _applicationControllerImpl;
}

- (void)quit {
  _applicationControllerImpl.cleanup();
}

- (void)setupApplicationWithWindow: (NSWindow*) window {
  _applicationControllerImpl.init(window, self);
}

- (void)drawRect:(NSRect)dirtyRect {
  [super drawRect: dirtyRect];
  if (_applicationControllerImpl.renderFrame()) {
    // TODO?
  }
}

- (BOOL)acceptsFirstResponder {
  return YES;
}

@end

#endif // defined PLATFORM_MACOS_USE_METAL
#endif // defined OS_MACOSX
