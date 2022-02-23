#if defined OS_MACOSX

#include "vulkan/platform/macos/AppDelegate.h"

int main(int argc, char* argv[]) {
  @autoreleasepool {
    NSApplication* application = [NSApplication sharedApplication];

    AppDelegate* appDelegate = [[AppDelegate alloc] init];
    [application setDelegate:appDelegate];
    [application run];
  }
  return EXIT_SUCCESS;
}

#endif  // defined OS_MACOSX
