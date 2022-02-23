#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")

#define _AMD64_
#include <windef.h>
#include <winuser.h>
#include <stdio.h>

#include "vulkan/platform/app/Application.h"

HINSTANCE 

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
  if (message == WM_DESTROY) {
    PostQuitMessage(0);
  }

  // LONG_PTR ptr = GetWindowLongPtrW(hWnd, GWL_USERDATA);
  // Window* window = (Window*)ptr;
  // if (window != nullptr) {
  //  return window->_wndProc(hWnd, message, wParam, lParam);
  // }
  return DefWindowProcW(hWnd, message, wParam, lParam);
}

typedef VkFlags VkWin32SurfaceCreateFlagsKHR;
typedef struct VkWin32SurfaceCreateInfoKHR {
    VkStructureType                 sType;
    const void*                     pNext;
    VkWin32SurfaceCreateFlagsKHR    flags;
    HINSTANCE                       hinstance;
    HWND                            hwnd;
} VkWin32SurfaceCreateInfoKHR;

class ApplicationControllerImpl : ApplicationController {
public:
  ApplicationControllerImpl(HINSTANCE hInstance, HWND hWnd) : _hInstance(hInstance), _hWnd(hWnd) {

  }

private:
  HINSTANCE _hInstance;
  HWND _hWnd;

  friend class ApplicationController;
}

bool ApplicationController::createSurface(VkInstance instance, VkSurfaceKHR* surface) {
  PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR) vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR");
  if (vkCreateWin32SurfaceKHR == nullptr) {
    return false;
  }

  ApplicationControllerImpl* impl = (ApplicationControllerImpl*) this;

  VkWin32SurfaceCreateInfoKHR sci {
    .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
    .hinstance = impl->_hInstance,
    .hwnd = impl->_hWnd,
  };

  return vkCreateWin32SurfaceKHR(instance, &sci, nullptr, surface) == VK_SUCCESS;
}

int WINAPI WinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR     lpCmdLine,
  int       nShowCmd
) {
  const wchar_t* className = L"Application";
  const wchar_t* caption = L"Application Title";
  HBRUSH hBackgroundBrush = (HBRUSH) COLOR_WINDOW;
  WNDCLASSEXW wcex {
    .cbSize = sizeof(WNDCLASSEXW),
    .style      = CS_HREDRAW | CS_VREDRAW,
    .lpfnWndProc  = &WndProc,
    .cbClsExtra   = 0,
    .cbWndExtra   = 0,
    .hInstance    = hInstance,
    .hIcon      = nullptr,
    .hCursor    = LoadCursor(nullptr, IDC_ARROW),
    .hbrBackground  = hBackgroundBrush,
    .lpszMenuName = nullptr,
    .lpszClassName  = className,
    .hIconSm    = nullptr,
  };
  
  if (RegisterClassExW(&wcex) == 0) {
    fprintf(stderr, "Failed to register window class.\n");
    return -1;
  }

  int style = WS_CAPTION | WS_SYSMENU | WS_THICKFRAME; 
  int styleEx = 0;

  HWND hWnd = CreateWindowExW(styleEx, className, caption, style,
    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, hInstance, nullptr);

  if (hWnd == nullptr) {
    fprintf(stderr, "Failed to create window.\n");
    return -1;
  }

  ApplicationControllerImpl applicationController = ApplicationControllerImpl(hInstance, hWnd);
  Application* app = createApplication();
  app->init(&applicationController);

  bool canRender = app->initGraphics();
  ShowWindow(hWnd, nShowCmd);

  MSG msg;
  while (GetMessage(&msg, nullptr, 0, 0)) {
      DispatchMessage(&msg);
  }

  if (canRender) {
    app->termGraphics();
  }

  app->term();
  delete app;

  return 0;
}
