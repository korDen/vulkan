#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")

#define _AMD64_
#include <windef.h>
#include <winuser.h>
#include <stdio.h>

#include "vulkan/platform/app/Application.h"

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if (message == WM_DESTROY) {
		PostQuitMessage(0);
	}

	// LONG_PTR ptr = GetWindowLongPtrW(hWnd, GWL_USERDATA);
	// Window* window = (Window*)ptr;
	// if (window != nullptr) {
	// 	return window->_wndProc(hWnd, message, wParam, lParam);
	// }
	return DefWindowProcW(hWnd, message, wParam, lParam);
}

bool ApplicationController::createSurface(VkInstance instance, VkSurfaceKHR* surface) {
	return true;
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
		.style			= CS_HREDRAW | CS_VREDRAW,
		.lpfnWndProc	= &WndProc,
		.cbClsExtra		= 0,
		.cbWndExtra		= 0,
		.hInstance		= hInstance,
		.hIcon			= nullptr,
		.hCursor		= LoadCursor(nullptr, IDC_ARROW),
		.hbrBackground	= hBackgroundBrush,
		.lpszMenuName	= nullptr,
		.lpszClassName	= className,
		.hIconSm		= nullptr,
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
		return -1;
	}

	ApplicationController applicationController;
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
