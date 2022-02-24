#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")

#define _AMD64_
#include <windef.h>
#include <winuser.h>
#include <stdio.h>
#include <fileapi.h>
#include <handleapi.h>

#include "vulkan/platform/app/Application.h"

#include "tools/cpp/runfiles/runfiles.h"
using bazel::tools::cpp::runfiles::Runfiles;

Runfiles* runfiles;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
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
typedef VkResult (VKAPI_PTR *PFN_vkCreateWin32SurfaceKHR)(VkInstance instance, const VkWin32SurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface);

class ApplicationControllerImpl : public ApplicationController {
public:
  ApplicationControllerImpl(HINSTANCE hInstance, HWND hWnd) : _hInstance(hInstance), _hWnd(hWnd) {}

private:
  HINSTANCE _hInstance;
  HWND _hWnd;

  friend class ApplicationController;
};

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

void* ApplicationController::readFile(const char* fileName, uint64_t* size) {
  std::string path = "vulkan/vulkan/";
  const char* fullPath = runfiles->Rlocation(path + fileName).c_str();

  HANDLE fd = CreateFileA(fullPath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (fd == INVALID_HANDLE_VALUE) {
    return nullptr;
  }

  DWORD fileSizeHigh;
  DWORD fileSize = GetFileSize(fd, &fileSizeHigh);
  if (fileSizeHigh != 0) {
    // file is too big.
    return nullptr;
  }

  void* data = malloc(fileSize);
  DWORD bytesRead;
  BOOL success = ReadFile(fd, data, fileSize, &bytesRead, nullptr);
  CloseHandle(fd);

  if (!success || bytesRead != fileSize) {
    free(data);
    return nullptr;
  }

  /*
  FILE* f = fopen(fullPath, "rb");
  fseek(f, 0, SEEK_END);
  int fileSize = ftell(f);
  fseek(f, 0, SEEK_SET);

  void* data = malloc(fileSize);
  auto bytesRead = fread(data, 1, fileSize, f);
  if (bytesRead != fileSize) {
    printf("bytesRead: %zd != fileSize: %d\n", bytesRead, fileSize);
    return nullptr;
  }
  
  printf("bytesRead: %zd == fileSize: %d\n", bytesRead, fileSize);
  */

  *size = fileSize;
  return data;
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

  char executablePath[MAX_PATH];
  GetModuleFileNameA(nullptr, executablePath, sizeof(executablePath));
  runfiles = Runfiles::Create(executablePath);

  ApplicationControllerImpl applicationController = ApplicationControllerImpl(hInstance, hWnd);
  Application* app = createApplication();
  app->init(&applicationController);

  bool canRender = app->initGraphics();
  ShowWindow(hWnd, nShowCmd);

  while (true) {
    MSG msg;

    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT) {
        // TODO: break.
      } else {
        TranslateMessage(&msg);
        DispatchMessage(&msg); 
      }
    }

    if (canRender) {
      app->renderFrame();
    }
  }

  if (canRender) {
    app->termGraphics();
  }

  app->term();
  delete app;

  return 0;
}
