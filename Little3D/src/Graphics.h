#ifndef __L3D_GRAPHICS_H__
#define __L3D_GRAPHICS_H__

namespace L3DGraphics{

	void MakeTranslateMatrix(L3DMath::Matrix& ret, float x, float y, float z){
		L3DMath::MatrixSetIdentity(ret);
		ret.m[3][0] = x;
		ret.m[3][1] = y;
		ret.m[3][2] = z;
	}

	void MakeScaleMatrix(L3DMath::Matrix& ret, float x, float y, float z){
		L3DMath::MatrixSetIdentity(ret);
		ret.m[0][0] = x;
		ret.m[1][1] = y;
		ret.m[2][2] = z;
	}

	void MakeRotateMatrix(L3DMath::Matrix& ret, float x, float y, float z, float theta){
		float cost = cos(theta);
		float sint = sin(theta);
		L3DMath::Vector axi;
		axi.x = x;
		axi.y = y;
		axi.z = z;
		L3DMath::VectorSetNormalized(axi);
		L3DMath::MatrixSetIdentity(ret);
		float one_sub_cost = 1.0f - cost;
		ret.m[0][0] = axi.x * axi.x * one_sub_cost + cost;
		ret.m[0][1] = axi.x * axi.y * one_sub_cost + axi.z * sint;
		ret.m[0][2] = axi.x * axi.z * one_sub_cost - axi.y * sint;
		ret.m[1][0] = axi.y * axi.x * one_sub_cost - axi.z * sint;
		ret.m[1][1] = axi.y * axi.y * one_sub_cost + cost;
		ret.m[1][2] = axi.y * axi.z * one_sub_cost + axi.x * sint;
		ret.m[2][0] = axi.z * axi.x * one_sub_cost + axi.y * sint;
		ret.m[2][1] = axi.z * axi.y * one_sub_cost - axi.x * sint;
		ret.m[2][2] = axi.z * axi.z * one_sub_cost + cost;
		ret.m[3][3] = 1.0f;
	}

	void MakeCameraViewMatrix(L3DMath::Matrix& ret, const L3DMath::Vector& eye, const L3DMath::Vector& look_at, const L3DMath::Vector& up){
		L3DMath::Vector cam_z;
		L3DMath::VectorSub(cam_z, look_at, eye);
		L3DMath::VectorSetNormalized(cam_z);
		L3DMath::Vector cam_x;
		L3DMath::VectorCross(cam_x, up, cam_z);
		L3DMath::VectorSetNormalized(cam_x);
		L3DMath::Vector cam_y;
		L3DMath::VectorCross(cam_y, cam_z, cam_x);
		ret.m[0][0] = cam_x.x;
		ret.m[0][1] = cam_y.x;
		ret.m[0][2] = cam_z.x;
		ret.m[1][0] = cam_x.y;
		ret.m[1][1] = cam_y.y;
		ret.m[1][2] = cam_z.y;
		ret.m[2][0] = cam_x.z;
		ret.m[2][1] = cam_y.z;
		ret.m[2][2] = cam_z.z;
		ret.m[3][0] = -L3DMath::VectorDot(cam_x, eye);
		ret.m[3][1] = -L3DMath::VectorDot(cam_y, eye);
		ret.m[3][2] = -L3DMath::VectorDot(cam_z, eye);
		ret.m[3][3] = 1.0f;
		ret.m[0][3] = ret.m[1][3] = ret.m[2][3] = 0.0f;
	}

	void MakePerspectiveMatrixLH(L3DMath::Matrix& ret, float width, float height, float near_c, float far_c){
		L3DMath::MatrixSetZero(ret);
		ret.m[0][0] = 2.0f * near_c / width;
		ret.m[1][1] = 2.0f * near_c / height;
		ret.m[2][2] = far_c / (far_c - near_c);
		ret.m[3][2] = -1.0f * near_c * far_c / (far_c - near_c);
		ret.m[2][3] = 1.0f;
	}

	void MakePerspectiveMatrixFOVLH(L3DMath::Matrix& ret, float fovh, float aspect, float near_c, float far_c){
		L3DMath::MatrixSetZero(ret);
		float r = 1.0f / tan(fovh * 0.5f);
		ret.m[0][0] = r;
		ret.m[1][1] = r * aspect;
		ret.m[2][2] = far_c / (far_c - near_c);
		ret.m[3][2] = -1.0f * near_c * far_c / (far_c - near_c);
		ret.m[2][3] = 1.0f;
	}

	bool IsClipedInCVV(const L3DMath::Vector& a){
		return a.x < -a.w || a.x > a.w || a.y < -a.w || a.y > a.w || a.z < 0.0f || a.z > a.w;
	}

	//a, b, c is counter-clockwise in screen-space , screen is upside-down£¬so it's less-than zero is counter-clockwise.
	bool IsBackfaceInScreen(const L3DMath::Vector& a, const L3DMath::Vector& b, const L3DMath::Vector& c){
		L3DMath::Vector ab;
		L3DMath::VectorSub(ab, b, a);
		L3DMath::Vector bc;
		L3DMath::VectorSub(bc, c, b);
		return (ab.x * bc.y - ab.y * bc.x) < 0;
	}

	void ProjectiveToScreen(L3DMath::Vector& ret, const L3DMath::Vector& a, int width, int height) {
		float rhw = 1.0f / a.w;
		ret.x = (1.0f + a.x * rhw) * width * 0.5f;
		ret.y = (1.0f - a.y * rhw) * height * 0.5f;
		ret.z = a.z;
		ret.w = a.w;
	}

	/*class Color{
	public:
		int r;
		int g;
		int b;
	};*/

	class TexUV{
	public:
		float u;
		float v;
	};

	class Vertex{
	public:
		L3DMath::Vector pos;
		float c_r, c_g, c_b;
		TexUV tex;
		float rhw;
	};

	class Triangle{
	public:
		int v[3];
	};

	class Mesh{
	public:
		int vertex_cnt;
		Vertex* v_list;
		int triangle_cnt;
		Triangle* t_list;		
	};

	/*class ScanLine{
	public:
		L3DMath::Vector v_start;
		L3DMath::Vector step_info;
		int screen_start_x;
		int screen_start_y;
		int screen_line_width;
	};*/

	class Transform{
	public:
		L3DMath::Matrix position;
		L3DMath::Matrix rotation;
		L3DMath::Matrix scale;
		L3DMath::Matrix view;
		L3DMath::Matrix perspective;
		L3DMath::Matrix transform;
	};

	void UpdateTransform(Transform& t){
		L3DMath::MatrixMul(t.transform, t.position, t.rotation);
		L3DMath::MatrixMul(t.transform, t.transform, t.scale);
		L3DMath::MatrixMul(t.transform, t.transform, t.view);
		L3DMath::MatrixMul(t.transform, t.transform, t.perspective);
	}

	void TransformVector(L3DMath::Vector& ret, const L3DMath::Vector& a, const Transform& t){
		L3DMath::VectorMulMatrix(ret, a, t.transform);
	}

	void VertexInterp(Vertex& ret, const Vertex& a, const Vertex& b, float t){
		L3DMath::VectorInterp(ret.pos, a.pos, b.pos, t);
		ret.c_r = L3DMath::FloatInterp(a.c_r, b.c_r, t);
		ret.c_g = L3DMath::FloatInterp(a.c_g, b.c_g, t);
		ret.c_b = L3DMath::FloatInterp(a.c_b, b.c_b, t);
		ret.tex.u = L3DMath::FloatInterp(a.tex.u, b.tex.u, t);
		ret.tex.v = L3DMath::FloatInterp(a.tex.v, b.tex.v, t);
		ret.rhw = L3DMath::FloatInterp(a.rhw, b.rhw, t);
	}

	void VertexDivide(Vertex& step, const Vertex& begin, const Vertex& end){
		float w = end.pos.x - begin.pos.x;
		if (w != 0.0f){
			float i_w = 1.0f / w;
			L3DMath::VectorSub(step.pos, end.pos, begin.pos);
			L3DMath::VectorScale(step.pos, step.pos, i_w);
			step.c_r = (end.c_r - begin.c_r) * i_w;
			step.c_g = (end.c_g - begin.c_g) * i_w;
			step.c_b = (end.c_b - begin.c_b) * i_w;
			step.rhw = (end.rhw - begin.rhw) * i_w;
			step.tex.u = (end.tex.u - begin.tex.u) * i_w;
			step.tex.v = (end.tex.v - begin.tex.v) * i_w;
		}
	}

	void VertexAdd(Vertex& ret, const Vertex& a, const Vertex& b){
		L3DMath::VectorAdd(ret.pos, a.pos, b.pos);
		ret.c_r = a.c_r + b.c_r;
		ret.c_g = a.c_g + b.c_g;
		ret.c_b = a.c_b + b.c_b;
		ret.rhw = a.rhw + b.rhw;
		ret.tex.u = a.tex.u + b.tex.u;
		ret.tex.v = a.tex.v + b.tex.v;
	}

	class Texture{
	public:
		int w, h;
		float *r, *g, *b, *a;
		float **row_r, **row_g, **row_b, **row_a;
	};

	int TextureColor(const Texture& t, int x, int y){
		int ret = (255 << 16) | 255;
		if (x >= 0 && x < t.w && y >= 0 && y < t.h){
			ret = ((int(t.row_r[y][x] + 0.5f)) << 16) | ((int(t.row_g[y][x] + 0.5f)) << 8) | (int(t.row_b[y][x] + 0.5f));
		}
		return ret;
	}
}
#endif