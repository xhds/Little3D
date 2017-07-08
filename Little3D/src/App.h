#ifndef L3D_APP_H_
#define L3D_APP_H_

#include <Windows.h>

namespace L3DGraphics{
	class Device;
}

class xmen;

namespace L3DApp{

	class App{
	private:
		App(){}
		App(const App&);
		App& operator=(const App&);

		HWND m_window_hnd = 0;
		
		HDC m_offscreen_dc = 0;
		HBITMAP m_offscreen_bmp = 0;
		unsigned char* m_offscreen_framebuffer = 0;

		L3DGraphics::Device* m_soft_device;

	public:
		static App& GetInstance(){
			static App instance;
			return instance;
		}

		int Init();

		void MainLoop();

		void Exit();

		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	private:
		void WinMsg();
		void CleanBuffer();
		void SwapBuffer();
		
		void InitDevice();
		void ReleaseDevice();

		void DrawPixel(L3DGraphics::Device& device, int x, int y, int color);
	};

}//namespace L3DApp

#endif