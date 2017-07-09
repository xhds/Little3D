#ifndef L3D_APP_H_
#define L3D_APP_H_

#include <Windows.h>

namespace L3DGraphics{
	class Mesh;
	class Transform;
	class Vertex;
	class Texture;
}

namespace L3DApp{

	enum RENDER_STATE{
		RS_NULL = 0,
		RS_FRAME = 1,
		RS_COLOR = 2,
		RS_TEXTURE = 4
	};

	class Device{
	public:
		int* frame_buffer;
		float* z_buffer;

		int** row_idx_of_frame_buffer;
		float** row_idx_of_z_buffer;

		RENDER_STATE render_state;
		int frame_color;
	};

	class GameObject{
	public:
		L3DGraphics::Transform* transform;
		L3DGraphics::Mesh* mesh_renderer;
		const L3DGraphics::Mesh* mesh_filter;
		const L3DGraphics::Texture* tex;
	};

	class App{
	private:
		App(){}
		App(const App&);
		App& operator=(const App&);

		HWND m_window_hnd = 0;
		
		HDC m_offscreen_dc = 0;
		HBITMAP m_offscreen_bmp = 0;
		unsigned char* m_offscreen_framebuffer = 0;

		Device m_soft_device;
		GameObject m_game_obj;

		L3DGraphics::Mesh* mesh_res;
		L3DGraphics::Texture* texture_res;
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
		void CleanBuffer(L3DGraphics::Transform* trans = 0);
		void SwapBuffer();
		
		void InitDevice();
		void ReleaseDevice();

		void DrawPixel(int x, int y, int color);
		void DrawLine(int x1, int y1, int x2, int y2, int color);
		void DrawGameObject(const GameObject& obj);

		void DrawStandardTriangle(const L3DGraphics::Vertex& peak, const L3DGraphics::Vertex& left, const L3DGraphics::Vertex& right
			, int peak_y, int line_y, RENDER_STATE rs, const L3DGraphics::Texture* ts = 0);
		void DrawTriangle(const L3DGraphics::Vertex& v0, const L3DGraphics::Vertex& v1, const L3DGraphics::Vertex& v2
			, int s0_x, int s0_y, int s1_x, int s1_y, int s2_x, int s2_y, RENDER_STATE rs, const L3DGraphics::Texture* ts = 0);

		void InitGameObject();
		void ReleaseGameObject();
	};

}//namespace L3DApp

#endif