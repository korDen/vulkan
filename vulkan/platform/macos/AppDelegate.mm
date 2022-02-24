#if defined OS_MACOSX

#include "vulkan/platform/macos/AppDelegate.h"
#include "vulkan/platform/macos/OpenGLWindow.h"
#include "vulkan/platform/macos/MetalWindow.h"

@implementation AppDelegate {
#if defined PLATFORM_MACOS_USE_OPENGL
  OpenGLWindow* _view;
#elif defined PLATFORM_MACOS_USE_METAL
  MetalWindow* _view;
#endif
}

- (id)init {
  if (self = [super init]) {
    NSRect windowRect = NSMakeRect(0.0f, 0.0f, 800.0f, 800.0f);
    int styleMask = NSWindowStyleMaskResizable | NSWindowStyleMaskMiniaturizable |
                    NSWindowStyleMaskClosable | NSWindowStyleMaskTitled;
    _window = [[NSWindow alloc] initWithContentRect:windowRect
                                          styleMask:styleMask
                                            backing:NSBackingStoreBuffered
                                              defer:NO];
    _window.title = @"Test";
    [NSApp activateIgnoringOtherApps:YES];

    // Create app menu and populate with quit command
    id appMenu = [[NSMenu alloc] init];
    id appName = [[NSProcessInfo processInfo] processName];
    id quitTitle = [@"Quit " stringByAppendingString:appName];
    id quitMenuItem = [[NSMenuItem alloc] initWithTitle:quitTitle
                                                 action:@selector(terminate:)
                                          keyEquivalent:@"q"];
    [appMenu addItem:quitMenuItem];

    // Add app menu to menu bar.
    id menuBar = [[NSMenu alloc] init];
    [NSApp setMainMenu:menuBar];
    id appMenuItem = [NSMenuItem new];
    [menuBar addItem:appMenuItem];
    [appMenuItem setSubmenu:appMenu];
  }
  return self;
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication*)sender {
  [_view quit];
  return NSTerminateNow;
}

static void handle_crash(int sig) {
  NSLog(@"crash: %@", [NSThread callStackSymbols]);
  exit(-1);
}

- (void)applicationWillFinishLaunching:(NSNotification*)notification {
  [_window makeKeyAndOrderFront:self];

  signal(SIGSEGV, handle_crash);

#if defined PLATFORM_MACOS_USE_OPENGL
  NSOpenGLPixelFormatAttribute attr[] = {NSOpenGLPFADoubleBuffer,
                                         NSOpenGLPFAAccelerated,
                                         NSOpenGLPFAColorSize,
                                         32,
                                         NSOpenGLPFADepthSize,
                                         16,
                                         0};
  NSOpenGLPixelFormat* format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attr];
  _view = [[OpenGLWindow alloc] initWithFrame:[_window frame] pixelFormat:format];
#elif defined PLATFORM_MACOS_USE_METAL
  _view = [[MetalWindow alloc] initWithFrame:[_window frame] device:MTLCreateSystemDefaultDevice()];
#endif
  [_view setupApplicationWithWindow: _window];
  [_view setWantsBestResolutionOpenGLSurface:YES];
  [_window setContentView:_view];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)theApplication {
  return YES;
}

@end

#endif  // defined OS_MACOSX
