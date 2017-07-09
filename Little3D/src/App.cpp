#include "App.h"
#include <tchar.h>
#include <iostream>
#include "Math.h"
#include "Graphics.h"

using namespace L3DGraphics;
using namespace L3DMath;
namespace L3DApp{

	static const TCHAR* CLASS_NAME = _T("L3DWND");
	static const TCHAR* TITLE = _T("Little3D");
	static const int WIN_W = 800;
	static const int WIN_H = 600;
	static const int FPS = 30;
	static const int WAIT_FOR_FPS = 1000 / FPS;
	static const int SPACE_LINE_COLOR = 0;
	static const int CAM_NEAR = 1;
	static const int CAM_FAR = 500;

	static const int RED = 255 << 16;
	static const int GREEN = 255 << 8;
	static const int BLUE = 255;
	static const int BLACK = 0;
	static const float COL_MAX = 255.0f;
	static const float COL_MIN = 0.0f;
	static const int ERROR_COL = RED | BLUE;

	/*static const L3DMath::Vector LINES[2 * 10] = {
		{ -10.0f, 0.0f, 10.0f, 1.0f },
		{ 10.0f, 0.0f, 10.0f, 1.0f },
		{ -10.0f, 0.0f, 5.0f, 1.0f },
		{ 10.0f, 0.0f, 5.0f, 1.0f },
		{ -10.0f, 0.0f, 0.0f, 1.0f },
		{ 10.0f, 0.0f, 0.0f, 1.0f },
		{ -10.0f, 0.0f, -5.0f, 1.0f },
		{ 10.0f, 0.0f, -5.0f, 1.0f },
		{ -10.0f, 0.0f, -10.0f, 1.0f },
		{ 10.0f, 0.0f, -10.0f, 1.0f },
		{ -10.0f, 0.0f, 10.0f, 1.0f },
		{ -10.0f, 0.0f, -10.0f, 1.0f },
		{ -5.0f, 0.0f, 10.0f, 1.0f },
		{ -5.0f, 0.0f, -10.0f, 1.0f },
		{ 0.0f, 0.0f, 10.0f, 1.0f },
		{ 0.0f, 0.0f, -10.0f, 1.0f },
		{ 5.0f, 0.0f, 10.0f, 1.0f },
		{ 5.0f, 0.0f, -10.0f, 1.0f },
		{ 10.0f, 0.0f, 10.0f, 1.0f },
		{ 10.0f, 0.0f, -10.0f, 1.0f }
	};

	static const int LINE_COLOR[10] = {
		BLUE, GREEN, RED, GREEN, BLUE,
		BLUE, GREEN, RED, GREEN, BLUE
	};*/

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

	void App::CleanBuffer(Transform* trans){
		int* p = m_soft_device.frame_buffer;
		float* z = m_soft_device.z_buffer;
		for (int i = 0; i < WIN_H; ++i){
			int color_r = 128 * (WIN_H - 1 - i) / (WIN_H - 1);
			int color_g = 168 * (WIN_H - 1 - i) / (WIN_H - 1);
			int color_b = 208 * (WIN_H - 1 - i) / (WIN_H - 1);
			for (int j = 0; j < WIN_W; ++j){
				DrawPixel(j, i, (color_r << 16) | (color_g << 8) | color_b);
				*z = 0.0f;
				++z;
			}
		}

		/*if (trans){
			for (int i = 0; i < 10; ++i){
				L3DMath::Vector red_b = LINES[2 * i];
				L3DMath::Vector red_e = LINES[2 * i + 1];
				TransformVector(red_b, red_b, *trans);
				TransformVector(red_e, red_e, *trans);
				ProjectiveToScreen(red_b, red_b, WIN_W, WIN_H);
				ProjectiveToScreen(red_e, red_e, WIN_W, WIN_H);
				DrawLine(int(red_b.x + 0.5f), int(red_b.y + 0.5f), int(red_e.x + 0.5f), int(red_e.y + 0.5f), LINE_COLOR[i]);
			}
		}*/
	}

	void App::MainLoop(){
		InitGameObject();
		bool log_dirty = true;
		float cam_x = 0, cam_y = 0, cam_z = 0, obj_r_y = 0;
		while (KEY_MAP[VK_ESCAPE] == 0){
			WinMsg();

			if (KEY_MAP['E'] == 1){
				cam_y += 0.05f;
				log_dirty = true;
			}
			if (KEY_MAP['Q'] == 1){
				cam_y -= 0.05f;
				log_dirty = true;
			}
			if (KEY_MAP['W'] == 1){
				cam_z += 0.05f;
				log_dirty = true;
			}
			if (KEY_MAP['S'] == 1){
				cam_z -= 0.05f;
				log_dirty = true;
			}
			if (KEY_MAP['A'] == 1){
				cam_x -= 0.05f;
				log_dirty = true;
			}
			if (KEY_MAP['D'] == 1){
				cam_x += 0.05f;
				log_dirty = true;
			}
			if (KEY_MAP['R'] == 1){
				cam_x = cam_y = cam_z = 0.0f;
				log_dirty = true;
			}
			if (KEY_MAP[VK_RIGHT] == 1){
				obj_r_y -= 0.05f;
			}
			if (KEY_MAP[VK_LEFT] == 1){
				obj_r_y += 0.05f;
			}
			if (KEY_MAP['F'] == 1){
				m_soft_device.render_state = RS_NULL;
				m_soft_device.render_state = RENDER_STATE(m_soft_device.render_state | RS_FRAME);
			}
			if (KEY_MAP['T'] == 1){
				m_soft_device.render_state = RS_NULL;
				m_soft_device.render_state = RENDER_STATE(m_soft_device.render_state | RS_TEXTURE);
			}
			if (KEY_MAP['C'] == 1){
				m_soft_device.render_state = RS_NULL;
				m_soft_device.render_state = RENDER_STATE(m_soft_device.render_state | RS_COLOR);
			}

			MakeTranslateMatrix(m_game_obj.transform->position, 0.0f, 0.0f, 0.0f);
			MakeRotateMatrix(m_game_obj.transform->rotation, 0.0f, 1.0f, 0.0f, obj_r_y);
			MakeScaleMatrix(m_game_obj.transform->scale, 1.0f, 1.0f, 1.0f);
			L3DMath::Vector eye = { 0.0f + cam_x, 0.0f + cam_y, -5.5f + cam_z, 1.0f};
			L3DMath::Vector look_at = { 0.0f, 0.0f, 0.0f, 1.0f };
			L3DMath::Vector up = { 0.0f, 1.0f, 0.0f, 1.0f};
			MakeCameraViewMatrix(m_game_obj.transform->view, eye, look_at, up);
			MakePerspectiveMatrixFOVLH(m_game_obj.transform->perspective, 3.1415926f * 0.5f, (float)WIN_W / (float)WIN_H, (float)CAM_NEAR, (float)CAM_FAR);
			UpdateTransform(*m_game_obj.transform);
			CleanBuffer(m_game_obj.transform);
			DrawGameObject(m_game_obj);
			SwapBuffer();
			if (log_dirty){
				log_dirty = false;
				std::cout << "===" << std::endl;
				std::cout << "cam eye:[" << eye.x << "," << eye.y << "," << eye.z << "]" << std::endl;
			}
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
		ReleaseGameObject();
	}

	void App::DrawPixel(int x, int y, int color){
		if (x < 0 || x >= WIN_W || y < 0 || y >= WIN_H)
			return;
		m_soft_device.row_idx_of_frame_buffer[y][x] = color;
	}

	void App::InitDevice(){
		m_soft_device.frame_buffer = (int*)m_offscreen_framebuffer;
		m_soft_device.z_buffer = (float*)malloc(sizeof(float)* WIN_H * WIN_W);
		m_soft_device.row_idx_of_frame_buffer = (int**)malloc(sizeof(int*) * WIN_H);
		m_soft_device.row_idx_of_z_buffer = (float**)malloc(sizeof(float*)* WIN_H);
		for (int i = 0; i < WIN_H; ++i){
			m_soft_device.row_idx_of_frame_buffer[i] = m_soft_device.frame_buffer + i * WIN_W;
			m_soft_device.row_idx_of_z_buffer[i] = m_soft_device.z_buffer + i * WIN_W;
		}
		m_soft_device.render_state = RS_FRAME;
		m_soft_device.frame_color = SPACE_LINE_COLOR;
	}

	void App::ReleaseDevice(){
		free(m_soft_device.row_idx_of_frame_buffer);
		m_soft_device.row_idx_of_frame_buffer = 0;
		free(m_soft_device.row_idx_of_z_buffer);
		m_soft_device.row_idx_of_z_buffer = 0;
		free(m_soft_device.z_buffer);
		m_soft_device.z_buffer = 0;
		m_soft_device.frame_buffer = 0;
	}

	void App::DrawLine(int x1, int y1, int x2, int y2, int color){
		int dx = x1 - x2;
		int dy = y1 - y2;
		if (dx == 0){
			int diff = dy > 0 ? -1 : 1;
			y2 = diff > 0 ? y2 + 1 : y2 - 1;
			for (int i = y1; i != y2; i += diff){
				DrawPixel(x1, i, color);
			}
		}
		else if (dy == 0)
		{
			int diff = dx > 0 ? -1 : 1;
			x2 = diff > 0 ? x2 + 1 : x2 - 1;
			for (int i = x1; i != x2; i += diff){
				DrawPixel(i, y1, color);
			}
		}
		else{
			int tan_theta = abs(dy / dx);
			if (tan_theta < 1){
				int diff = dx > 0 ? -1 : 1;
				int diff_j = dy > 0 ? -1 : 1;
				int r = 0, dr = abs(dy), rx = abs(dx), j = y1;
				x2 = diff > 0 ? x2 + 1 : x2 - 1;
				for (int i = x1; i != x2; i += diff){
					DrawPixel(i, j, color);
					r += dr;
					if (r >= rx) {
						r -= rx;
						j += diff_j;
					}
				}
			}
			else {
				int diff = dy > 0 ? -1 : 1;
				int diff_j = dx > 0 ? -1 : 1;
				int r = 0, dr = abs(dx), ry = abs(dy), j = x1;
				y2 = diff > 0 ? y2 + 1 : y2 - 1;
				for (int i = y1; i != y2; i += diff){
					DrawPixel(j, i, color);
					r += dr;
					if (r >= ry) {
						r -= ry;
						j += diff_j;
					}
				}
			}
		}
	}
	
	void App::DrawGameObject(const GameObject& obj){
		for (int i = 0; i < obj.mesh_renderer->vertex_cnt; ++i){
			TransformVector(obj.mesh_renderer->v_list[i].pos, obj.mesh_filter->v_list[i].pos, *obj.transform);	
		}
		for (int i = 0; i < obj.mesh_renderer->triangle_cnt; ++i){
			const Triangle& t = obj.mesh_renderer->t_list[i];
			const Vertex& v0 = obj.mesh_renderer->v_list[t.v[0]];
			const Vertex& v1 = obj.mesh_renderer->v_list[t.v[1]];
			const Vertex& v2 = obj.mesh_renderer->v_list[t.v[2]];
			if (!IsClipedInCVV(v0.pos) && !IsClipedInCVV(v1.pos) && !IsClipedInCVV(v2.pos)){
				L3DMath::Vector s0, s1, s2;  //screen float
				ProjectiveToScreen(s0, v0.pos, WIN_W, WIN_H);
				ProjectiveToScreen(s1, v1.pos, WIN_W, WIN_H);
				ProjectiveToScreen(s2, v2.pos, WIN_W, WIN_H);
				if (IsBackfaceInScreen(s0, s1, s2)){  //backface-culling
					continue;
				}
				int v0x = int(s0.x + 0.5f), v1x = int(s1.x + 0.5f), v2x = int(s2.x + 0.5f);  //screen buffer index
				int v0y = int(s0.y + 0.5f), v1y = int(s1.y + 0.5f), v2y = int(s2.y + 0.5f);  //screen buffer index
				if (m_soft_device.render_state & RS_TEXTURE || m_soft_device.render_state & RS_COLOR) {
					Vertex rs0 = v0, rs1 = v1, rs2 = v2;
					rs0.pos = s0, rs1.pos = s1, rs2.pos = s2;
					rs0.rhw = 1.0f / rs0.pos.w;
					rs1.rhw = 1.0f / rs1.pos.w;
					rs2.rhw = 1.0f / rs2.pos.w;
					rs0.tex.u *= rs0.rhw;
					rs0.tex.v *= rs0.rhw;
					rs1.tex.u *= rs1.rhw;
					rs1.tex.v *= rs1.rhw;
					rs2.tex.u *= rs2.rhw;
					rs2.tex.v *= rs2.rhw;
					DrawTriangle(rs0, rs1, rs2, v0x, v0y, v1x, v1y, v2x, v2y, m_soft_device.render_state, texture_res);
				}
				if (m_soft_device.render_state & RS_FRAME){
					DrawLine(v0x, v0y, v1x, v1y, m_soft_device.frame_color);
					DrawLine(v1x, v1y, v2x, v2y, m_soft_device.frame_color);
					DrawLine(v2x, v2y, v0x, v0y, m_soft_device.frame_color);
				}
			}
		}
	}

	void App::InitGameObject(){
		texture_res = new Texture;
		texture_res->w = texture_res->h = 256;
		int tex_size = texture_res->w * texture_res->h;
		texture_res->r = new float[tex_size];
		texture_res->g = new float[tex_size];
		texture_res->b = new float[tex_size];
		texture_res->a = new float[tex_size];
		texture_res->row_r = new float*[texture_res->h];
		texture_res->row_g = new float*[texture_res->h];
		texture_res->row_b = new float*[texture_res->h];
		texture_res->row_a = new float*[texture_res->h];
		for (int i = 0; i < texture_res->h; ++i){
			texture_res->row_r[i] = texture_res->r + i * texture_res->w;
			texture_res->row_g[i] = texture_res->g + i * texture_res->w;
			texture_res->row_b[i] = texture_res->b + i * texture_res->w;
			texture_res->row_a[i] = texture_res->a + i * texture_res->w;
		}
		for (int y = 0; y < texture_res->h; ++y){
			for (int x = 0; x < texture_res->w; ++x){
				if ((y / 32) % 2 == 0 && (x / 32) % 2 == 0){
					texture_res->row_r[y][x] = 25.0f;
					texture_res->row_g[y][x] = 25.0f;
					texture_res->row_b[y][x] = 25.0f;
				}
				else if ((y / 32) % 2 == 1 && (x / 32) % 2 == 1){
					texture_res->row_r[y][x] = 25.0f;
					texture_res->row_g[y][x] = 25.0f;
					texture_res->row_b[y][x] = 25.0f;
				}
				else {
					texture_res->row_r[y][x] = 255.0f;
					texture_res->row_g[y][x] = 255.0f;
					texture_res->row_b[y][x] = 255.0f;
				}
				texture_res->row_a[y][x] = 1.0f;
			}
		}

		int vc = 24;
		Vertex mesh_data[24] = {
			{ { -1.0f, 1.0f, -1.0f, 1.0f }, 255.0f, 0.0f, 0.0f, { 0.0f, 0.0f }, 1.0f },
			{ { 1.0f, 1.0f, -1.0f, 1.0f }, 0.0f, 255.0f, 0.0f, { 1.0f, 0.0f }, 1.0f },
			{ { 1.0f, -1.0f, -1.0f, 1.0f }, 0.0f, 0.0f, 255.0f , { 1.0f, 1.0f }, 1.0f },
			{ { -1.0f, -1.0f, -1.0f, 1.0f }, 0.0f, 0.0f, 0.0f , { 0.0f, 1.0f }, 1.0f },

			{ { 1.0f, 1.0f, -1.0f, 1.0f },  255.0f, 0.0f, 0.0f , { 0.0f, 0.0f }, 1.0f },
			{ { 1.0f, 1.0f, 1.0f, 1.0f },  0.0f, 255.0f, 0.0f , { 1.0f, 0.0f }, 1.0f },
			{ { 1.0f, -1.0f, 1.0f, 1.0f },  0.0f, 0.0f, 255.0f , { 1.0f, 1.0f }, 1.0f },
			{ { 1.0f, -1.0f, -1.0f, 1.0f },  0.0f, 0.0f, 0.0f , { 0.0f, 1.0f }, 1.0f },

			{ { 1.0f, 1.0f, 1.0f, 1.0f }, 255.0f, 0.0f, 0.0f, { 0.0f, 0.0f }, 1.0f },
			{ { -1.0f, 1.0f, 1.0f, 1.0f }, 0.0f, 255.0f, 0.0f, { 1.0f, 0.0f }, 1.0f },
			{ { -1.0f, -1.0f, 1.0f, 1.0f }, 0.0f, 0.0f, 255.0f, { 1.0f, 1.0f }, 1.0f },
			{ { 1.0f, -1.0f, 1.0f, 1.0f }, 0.0f, 0.0f, 0.0f, { 0.0f, 1.0f }, 1.0f },

			{ { -1.0f, 1.0f, 1.0f, 1.0f }, 255.0f, 0.0f, 0.0f, { 0.0f, 0.0f }, 1.0f },
			{ { -1.0f, 1.0f, -1.0f, 1.0f }, 0.0f, 255.0f, 0.0f, { 1.0f, 0.0f }, 1.0f },
			{ { -1.0f, -1.0f, -1.0f, 1.0f }, 0.0f, 0.0f, 255.0f, { 1.0f, 1.0f }, 1.0f },
			{ { -1.0f, -1.0f, 1.0f, 1.0f }, 0.0f, 0.0f, 0.0f, { 0.0f, 1.0f }, 1.0f },

			{ { -1.0f, 1.0f, 1.0f, 1.0f }, 255.0f, 0.0f, 0.0f, { 0.0f, 0.0f }, 1.0f },
			{ { 1.0f, 1.0f, 1.0f, 1.0f }, 0.0f, 255.0f, 0.0f, { 1.0f, 0.0f }, 1.0f },
			{ { 1.0f, 1.0f, -1.0f, 1.0f }, 0.0f, 0.0f, 255.0f, { 1.0f, 1.0f }, 1.0f },
			{ { -1.0f, 1.0f, -1.0f, 1.0f }, 0.0f, 0.0f, 0.0f, { 0.0f, 1.0f }, 1.0f },

			{ { -1.0f, -1.0f, -1.0f, 1.0f }, 255.0f, 0.0f, 0.0f, { 0.0f, 0.0f }, 1.0f },
			{ { 1.0f, -1.0f, -1.0f, 1.0f }, 0.0f, 255.0f, 0.0f, { 1.0f, 0.0f }, 1.0f },
			{ { 1.0f, -1.0f, 1.0f, 1.0f }, 0.0f, 0.0f, 255.0f, { 1.0f, 1.0f }, 1.0f },
			{ { -1.0f, -1.0f, 1.0f, 1.0f }, 0.0f, 0.0f, 0.0f, { 0.0f, 1.0f }, 1.0f }
		};
		int tc = 12;
		Triangle tri_data[12] {
			{ { 0, 1, 2 } },
			{ { 0, 2, 3 } },

			{ { 4, 5, 6 } },
			{ { 4, 6, 7 } },

			{ { 8, 9, 10 } },
			{ { 8, 10, 11 } },

			{ { 12, 13, 14 } },
			{ { 12, 14, 15 } },

			{ { 16, 17, 18 } },
			{ { 16, 18, 19 } },

			{ { 20, 21, 22 } },
			{ { 20, 22, 23 } }
		};

		/*int vc = 3;
		Vertex mesh_data[3] = {
			{ { -1.0f, 1.0f, -1.0f, 1.0f }, 0, { 0.0f, 0.0f }, 1.0f },
			{ { 1.0f, 1.0f, -1.0f, 1.0f }, 0, { 1.0f, 0.0f }, 1.0f },
			{ { 1.0f, -1.0f, -1.0f, 1.0f }, 0, { 1.0f, 1.0f }, 1.0f }
		};
		int tc = 1;
		Triangle tri_data[1] {
			{ { 0, 1, 2 } }
		};*/
	
		mesh_res = new Mesh;
		mesh_res->vertex_cnt = vc;
		mesh_res->v_list = new Vertex[mesh_res->vertex_cnt];
		for (int i = 0; i < mesh_res->vertex_cnt; ++i){
			mesh_res->v_list[i] = mesh_data[i];
		}
		mesh_res->triangle_cnt = tc;
		mesh_res->t_list = new Triangle[mesh_res->triangle_cnt];
		for (int i = 0; i < mesh_res->triangle_cnt; ++i){
			mesh_res->t_list[i] = tri_data[i];
		}

		m_game_obj.mesh_renderer = new Mesh;
		m_game_obj.mesh_renderer->vertex_cnt = mesh_res->vertex_cnt;
		m_game_obj.mesh_renderer->v_list = new Vertex[m_game_obj.mesh_renderer->vertex_cnt];
		for (int i = 0; i < mesh_res->vertex_cnt; ++i){
			m_game_obj.mesh_renderer->v_list[i] = mesh_res->v_list[i];
		}

		m_game_obj.mesh_renderer->triangle_cnt = mesh_res->triangle_cnt;
		m_game_obj.mesh_renderer->t_list = new Triangle[m_game_obj.mesh_renderer->triangle_cnt];
		for (int i = 0; i < mesh_res->triangle_cnt; ++i){
			m_game_obj.mesh_renderer->t_list[i] = mesh_res->t_list[i];
		}

		m_game_obj.transform = new Transform;
		m_game_obj.mesh_filter = mesh_res;
		UpdateTransform(*m_game_obj.transform);
	}

	void App::ReleaseGameObject(){

		delete[] texture_res->row_a;
		texture_res->row_a = 0;
		delete[] texture_res->row_r;
		texture_res->row_r = 0;
		delete[] texture_res->row_g;
		texture_res->row_g = 0;
		delete[] texture_res->row_b;
		texture_res->row_b = 0;
		delete[] texture_res->a;
		texture_res->a = 0;
		delete[] texture_res->r;
		texture_res->r = 0;
		delete[] texture_res->g;
		texture_res->g = 0;
		delete[] texture_res->b;
		texture_res->b = 0;
		delete texture_res;
		texture_res = 0;

		delete[] mesh_res->v_list;
		mesh_res->v_list = 0;
		mesh_res->vertex_cnt = 0;
		delete[] mesh_res->t_list;
		mesh_res->t_list = 0;
		mesh_res->vertex_cnt = 0;
		delete mesh_res;
		mesh_res = 0;

		delete[] m_game_obj.mesh_renderer->v_list;
		m_game_obj.mesh_renderer->v_list = 0;
		m_game_obj.mesh_renderer->vertex_cnt = 0;
		delete[] m_game_obj.mesh_renderer->t_list;
		m_game_obj.mesh_renderer->t_list = 0;
		m_game_obj.mesh_renderer->vertex_cnt = 0;
		delete m_game_obj.mesh_renderer;
		m_game_obj.mesh_renderer = 0;

		delete m_game_obj.transform;
		m_game_obj.transform = 0;

		m_game_obj.mesh_filter = 0;
	}

	void App::DrawTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2
		, int s0_x, int s0_y, int s1_x, int s1_y, int s2_x, int s2_y, RENDER_STATE rs, const Texture* ts){
		const Vertex *p0 = &v0, *p1 = &v1, *p2 = &v2;
		if (s0_y > s1_y){
			int temp = s0_y;
			s0_y = s1_y;
			s1_y = temp;
			temp = s0_x;
			s0_x = s1_x;
			s1_x = temp;
			const Vertex *pt = p0;
			p0 = p1;
			p1 = pt;
		}
		if (s0_y > s2_y){
			int temp = s0_y;
			s0_y = s2_y;
			s2_y = temp;
			temp = s0_x;
			s0_x = s2_x;
			s2_x = temp;
			const Vertex *pt = p0;
			p0 = p2;
			p2 = pt;
		}
		if (s1_y > s2_y){
			int temp = s1_y;
			s1_y = s2_y;
			s2_y = temp;
			temp = s1_x;
			s1_x = s2_x;
			s2_x = temp;
			const Vertex *pt = p1;
			p1 = p2;
			p2 = pt;
		}
		if (s0_y == s1_y){
			if (s0_x < s1_x) {
				DrawStandardTriangle(*p2, *p0, *p1, s2_y, s0_y, rs, ts);
			}
			else {
				DrawStandardTriangle(*p2, *p1, *p0, s2_y, s0_y, rs, ts);
			}
		}
		else if (s1_y == s2_y){
			if (s1_x < s2_x) {
				DrawStandardTriangle(*p0, *p1, *p2, s0_y, s2_y, rs, ts);
			}
			else {
				DrawStandardTriangle(*p0, *p2, *p1, s0_y, s2_y, rs, ts);
			}
		}
		else {
			float t = (p2->pos.y - p1->pos.y) / (p2->pos.y - p0->pos.y);
			t = FloatClamp(t, 0.0f, 1.0f);
			Vertex mid;
			VertexInterp(mid, *p2, *p0, t);
			if (mid.pos.x < p1->pos.x){
				DrawStandardTriangle(*p0, mid, *p1, s0_y, s1_y, rs, ts);
				DrawStandardTriangle(*p2, mid, *p1, s2_y, s1_y, rs, ts);
			}
			else {
				DrawStandardTriangle(*p0, *p1, mid, s0_y, s1_y, rs, ts);
				DrawStandardTriangle(*p2, *p1, mid, s2_y, s1_y, rs, ts);
			}			
		}
	}

	void App::DrawStandardTriangle(const Vertex& peak, const Vertex& left, const Vertex& right
		, int peak_y, int line_y, RENDER_STATE rs, const Texture* ts){
		if (peak_y == line_y || peak_y < 0 || peak_y >= WIN_H){
			return;
		}
		int diff = 0;
		if (peak_y > line_y){
			diff = -1;
			--line_y;
		}
		else {
			diff = +1;
			++line_y;
		}
		float dp = peak.pos.y - left.pos.y;
		for (int index_y = peak_y; index_y != line_y; index_y += diff){
			float screen_y = index_y + 0.5f;
			float t = abs((peak.pos.y - screen_y) / dp);
			t = FloatClamp(t, 0.0f, 1.0f);
			Vertex scan_left, scan_right, step;
			VertexInterp(scan_left, peak, left, t);
			VertexInterp(scan_right, peak, right, t);
			VertexDivide(step, scan_left, scan_right);
			int x_begin = int(scan_left.pos.x + 0.5f);
			int x_end = int(scan_right.pos.x + 0.5f);
			if (x_begin < 0 || x_begin >= WIN_W || x_end < 0 || x_end >= WIN_W || x_begin > x_end) {
				continue;
			}
			for (int index_x = x_begin; index_x <= x_end; ++index_x){
				if (scan_left.rhw >= m_soft_device.row_idx_of_z_buffer[index_y][index_x]){
					m_soft_device.row_idx_of_z_buffer[index_y][index_x] = scan_left.rhw;
					int col = ERROR_COL;
					if (rs & RS_TEXTURE && ts != 0){
						float w = 1.0f / scan_left.rhw;
						float tx = ts->w * scan_left.tex.u * w;
						float ty = ts->h * scan_left.tex.v * w;
						col = TextureColor(*ts, int(tx + 0.5f), int(ty + 0.5f));
					}
					if (rs & RS_COLOR){
						int r = int(FloatClamp(scan_left.c_r, COL_MIN, COL_MAX) + 0.5f);
						int g = int(FloatClamp(scan_left.c_g, COL_MIN, COL_MAX) + 0.5f);
						int b = int(FloatClamp(scan_left.c_b, COL_MIN, COL_MAX) + 0.5f);
						col = (r << 16) | (g << 8) | b;
					}
					DrawPixel(index_x, index_y, col);
				}
				VertexAdd(scan_left, scan_left, step);
			}
		}
	}
}//namespace L3DApp