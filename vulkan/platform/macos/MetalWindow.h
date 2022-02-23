#if defined PLATFORM_MACOS_USE_METAL
#import <MetalKit/MetalKit.h>

@interface MetalWindow : MTKView
- (void)setupApplicationWithWindow: (NSWindow*) window;
- (void)quit;
@end

#endif // defined PLATFORM_MACOS_USE_METAL
