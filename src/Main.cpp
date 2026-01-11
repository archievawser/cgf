#include <iomanip>
#include <iostream>

#include "core/Game.h"

class Peener : public Game 
{
public:
	void Tick() override
	{

	}

	void Draw() override
	{
		
	}
};

Game* appRef = nullptr;

LRESULT CALLBACK MessageProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
#if defined(_DEBUG) || defined(DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	appRef = new Peener;

	std::wstring title = L"Hello, world!";

	WNDCLASSEX wcex = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, MessageProc,
					   0L, 0L, hInstance, NULL, NULL, NULL, NULL, L"SampleApp", NULL };
	RegisterClassEx(&wcex);

	// Create a window
	LONG windowWidth = 1280;
	LONG windowHeight = 1024;
	RECT rc = {0, 0, windowWidth, windowHeight};
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	HWND wnd = CreateWindow(L"SampleApp", title.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
	
	if (!wnd)
	{
		MessageBox(NULL, L"Cannot create window", L"Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	ShowWindow(wnd, nShowCmd);
	UpdateWindow(wnd);

	if (!appRef->InitializeDiligent(wnd))
		return -1;

	appRef->CreateResources();

	// Main message loop
	MSG msg = {0};

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			appRef->Render();
		}
	}

	delete appRef;

	return (int)msg.wParam;
}

// Called every time the NativeNativeAppBase receives a message
LRESULT CALLBACK MessageProc(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		BeginPaint(wnd, &ps);
		EndPaint(wnd, &ps);
		return 0;
	}
	case WM_SIZE: // Window size has been changed
		if (appRef)
		{
			appRef->WindowResize(LOWORD(lParam), HIWORD(lParam));
		}
		return 0;

	case WM_CHAR:
		if (wParam == VK_ESCAPE)
			PostQuitMessage(0);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_GETMINMAXINFO:
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;

		lpMMI->ptMinTrackSize.x = 320;
		lpMMI->ptMinTrackSize.y = 320;
		return 0;
	}

	default:
		return DefWindowProc(wnd, message, wParam, lParam);
	}
}
