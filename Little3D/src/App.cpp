#include "App.h"
#include <tchar.h>
#include "Math.h"
#include "Graphics.h"

namespace L3DApp{

	static const TCHAR* CLASS_NAME = _T("L3DWND");
	static const TCHAR* TITLE = _T("Little3D");
	static const int WIN_W = 640;
	static const int WIN_H = 480;
	static const int FPS = 60;
	static const int WAIT_FOR_FPS = 1000 / FPS;
	static const int SPACE_LINE_COLOR = 0;

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
			, 0, 0, WIN_W, WIN_H, 0, 0, window_class.hInstance, 0);
		if (0 == m_window_hnd){
			return 3;
		}

		HDC origin_dc = GetDC(m_window_hnd);
		m_offscreen_dc = CreateCompatibleDC(origin_dc);
		ReleaseDC(m_window_hnd, origin_dc);
		
		BITMAPINFOHEADER bmp_header;
		bmp_header.biSize = sizeof(BITMAPINFOHEADER);
		bmp_header.biWidth = WIN_W;
		bmp_header.biHeight = -WIN_H;
		bmp_header.biPlanes = 1;
		bmp_header.biBitCount = 32;
		bmp_header.biCompression = BI_RGB;
		bmp_header.biSizeImage = 0;
		bmp_header.biXPelsPerMeter = 0;
		bmp_header.biYPelsPerMeter = 0;
		bmp_header.biClrUsed = 0;
		bmp_header.biClrImportant = 0;

		BITMAPINFO bmp_info;
		memset(&bmp_info, 0, sizeof(BITMAPINFO));
		bmp_info.bmiHeader = bmp_header;
		void* section_ptr = 0;
		m_offscreen_bmp = CreateDIBSection(m_offscreen_dc, &bmp_info, DIB_RGB_COLORS, &section_ptr, 0, 0);
		if (0 == m_offscreen_bmp){
			return 4;
		}
		HBITMAP origin_bmp = (HBITMAP)SelectObject(m_offscreen_dc, m_offscreen_bmp);
		DeleteObject(origin_bmp);
		m_offscreen_framebuffer = (unsigned char*)section_ptr;
		InitDevice();
		CleanBuffer();

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

	void App::SwapBuffer(){
		HDC origin_dc = GetDC(m_window_hnd);
		BitBlt(origin_dc, 0, 0, WIN_W, WIN_H, m_offscreen_dc, 0, 0, SRCCOPY);
		ReleaseDC(m_window_hnd, origin_dc);
	}

	void App::CleanBuffer(){
		int* p = m_soft_device->frame_buffer;
		float* z = m_soft_device->z_buffer;
		for (int i = 0; i < WIN_H; ++i){
			int color_r = 128 * (WIN_H - 1 - i) / (WIN_H - 1);
			int color_g = 168 * (WIN_H - 1 - i) / (WIN_H - 1);
			int color_b = 208 * (WIN_H - 1 - i) / (WIN_H - 1);
			for (int j = 0; j < WIN_W; ++j){
				*p = (color_r << 16) | (color_g << 8) | color_b;
				++p;
				*z = 0.0f;
				++z;
			}
		}
	}

	void App::MainLoop(){
		while (KEY_MAP[VK_ESCAPE] == 0){
			WinMsg();
			CleanBuffer();
			SwapBuffer();
			Sleep(WAIT_FOR_FPS);
		}
	}

	void App::Exit(){
		if (m_offscreen_dc){
			DeleteDC(m_offscreen_dc);
			m_offscreen_dc = 0;
		}
		m_offscreen_bmp = 0;
		if (m_window_hnd){
			CloseWindow(m_window_hnd);
			m_window_hnd = 0;
		}
		ReleaseDevice();
	}

	void App::DrawPixel(L3DGraphics::Device& device, int x, int y, int color){
		device.row_idx_of_frame_buffer[y][x] = color;
	}

	void App::InitDevice(){
		m_soft_device = new L3DGraphics::Device;
		m_soft_device->frame_buffer = (int*)m_offscreen_framebuffer;
		m_soft_device->z_buffer = (float*)malloc(sizeof(float)* WIN_H * WIN_W);
		m_soft_device->row_idx_of_frame_buffer = (int**)malloc(sizeof(int*) * WIN_H);
		m_soft_device->row_idx_of_z_buffer = (float**)malloc(sizeof(float*)* WIN_H);
		for (int i = 0; i < WIN_H; ++i){
			m_soft_device->row_idx_of_frame_buffer[i] = m_soft_device->frame_buffer + i * WIN_W * sizeof(int);
			m_soft_device->row_idx_of_z_buffer[i] = m_soft_device->z_buffer + i * WIN_W * sizeof(float);
		}
		m_soft_device->render_state = L3DGraphics::FRAME;
		m_soft_device->frame_color = SPACE_LINE_COLOR;
	}

	void App::ReleaseDevice(){
		free(m_soft_device->row_idx_of_frame_buffer);
		m_soft_device->row_idx_of_frame_buffer = 0;
		free(m_soft_device->row_idx_of_z_buffer);
		m_soft_device->row_idx_of_z_buffer = 0;
		free(m_soft_device->z_buffer);
		m_soft_device->z_buffer = 0;
		m_soft_device->frame_buffer = 0;
		delete m_soft_device;
		m_soft_device = 0;
	}
	
}//namespace L3DApp