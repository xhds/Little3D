#include "App.h"
#include <tchar.h>
namespace L3DApp{

	static const TCHAR* CLASS_NAME = _T("L3DWND");
	static const TCHAR* TITLE = _T("Little3D");
	static const int WINDOW_WIDTH = 640;
	static const int WINDOW_HEIGHT = 480;

	static int KEY_MAP[512];

	int App::Init(){
		if (m_window_hnd != 0){
			return 1;
		}

		memset(KEY_MAP, 0, sizeof(int));

		WNDCLASS window_class;
		memset(&window_class, 0, sizeof(WNDCLASS));
		window_class.style = CS_BYTEALIGNCLIENT;
		window_class.lpfnWndProc = (WNDPROC)WindowProc;
		window_class.hInstance = GetModuleHandle(0);
		window_class.lpszClassName = CLASS_NAME;
		window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		if (!RegisterClass(&window_class)) {
			return 2;
		}

		m_window_hnd = CreateWindow(CLASS_NAME, TITLE
			, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX
			, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, window_class.hInstance, 0);
		if (0 == m_window_hnd){
			return 3;
		}

		SetForegroundWindow(m_window_hnd);
		ShowWindow(m_window_hnd, SW_NORMAL);

		return 0;
	}

	LRESULT App::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam){
		switch (msg)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_KEYDOWN:
			KEY_MAP[wparam & 511] = 1;
			break;
		case WM_KEYUP:
			KEY_MAP[wparam & 511] = 0;
			break;
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return 0;
	}

	void App::WinMsg(){
		MSG msg;
		while (true)
		{
			if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
				break;
			if (!GetMessage(&msg, NULL, 0, 0))
				break;
			DispatchMessage(&msg);
		}
	}

	void App::MainLoop(){
		while (KEY_MAP[VK_ESCAPE] == 0)
		{
			WinMsg();
		}
	}
}//namespace L3DApp