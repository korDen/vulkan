#if defined PLATFORM_MACOS_USE_OPENGL
#define GL_SILENCE_DEPRECATION
#import <AppKit/NSOpenGLView.h>

@interface OpenGLWindow : NSOpenGLView
- (void)setupApplicationWithWindow: (NSWindow*) window;
- (void)quit;
@end
#endif // defined PLATFORM_MACOS_USE_OPENGL
