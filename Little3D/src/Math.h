#ifndef __L3D_MATH_H__
#define __L3D_MATH_H__
#include <memory>
#include <math.h>

namespace L3DMath {

	inline float FloatClamp(float t, float min, float max){
		return t < min ? min : (t > max ? max : t);
	}

	struct Vector {
		float x, y, z, w;
	};

	class Matrix {
	public:
		float m[4][4];
		Matrix(){ memset(m, 0, 16 * sizeof(float));}
	};

	inline void VectorAdd(Vector& ret, const Vector& a, const Vector& b){
		ret.x = a.x + b.x;
		ret.y = a.y + b.y;
		ret.z = a.z + b.z;
	}

	inline void VectorSub(Vector& ret, const Vector& a, const Vector& b){
		ret.x = a.x - b.x;
		ret.y = a.y - b.y;
		ret.z = a.z - b.z;
	}

	inline void VectorScale(Vector& ret, const Vector& a, float scale){
		ret.x = a.x * scale;
		ret.y = a.y * scale;
		ret.z = a.z * scale;
	}

	inline float VectorMod(const Vector& a){
		return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
	}

	inline void VectorSetNormalized(Vector& a){
		float mod = VectorMod(a);
		if (mod != 0.0f){
			float inv_mod = 1.0f / mod;
			a.x *= inv_mod;
			a.y *= inv_mod;
			a.z *= inv_mod;
		}
	}

	inline float VectorDot(const Vector& a, const Vector& b){
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	inline void VectorCross(Vector& ret, const Vector& a, const Vector& b){
		ret.x = a.y * b.z - a.z * b.y;
		ret.y = a.z * b.x - a.x * b.z;
		ret.z = a.x * b.y - a.y * b.x;
	}

	inline float FloatInterp(float a, float b, float t){
		return a + (b - a) * t;
	}

	inline void VectorInterp(Vector& ret, const Vector& a, const Vector& b, float t){
		ret.x = FloatInterp(a.x, b.x, t);
		ret.y = FloatInterp(a.y, b.y, t);
		ret.z = FloatInterp(a.z, b.z, t);
		ret.w = 1.0f;
	}

	inline void MatrixMul(Matrix& ret, const Matrix& a, const Matrix& b){
		for (int row = 0; row < 4; ++row){
			for (int col = 0; col < 4; ++col) {
				ret.m[row][col] = a.m[row][0] * b.m[0][col] + a.m[row][1] * b.m[1][col] + a.m[row][2] * b.m[2][col] + a.m[row][3] * b.m[3][col];
			}
		}		
	}

	inline void VectorMulMatrix(Vector& ret, const Vector& a, const Matrix& b){
		ret.x = a.x * b.m[0][0] + a.y * b.m[1][0] + a.z * b.m[2][0] + a.w * b.m[3][0];
		ret.y = a.x * b.m[0][1] + a.y * b.m[1][1] + a.z * b.m[2][1] + a.w * b.m[3][1];
		ret.z = a.x * b.m[0][2] + a.y * b.m[1][2] + a.z * b.m[2][2] + a.w * b.m[3][2];
		ret.w = a.x * b.m[0][3] + a.y * b.m[1][3] + a.z * b.m[2][3] + a.w * b.m[3][3];
	}

	inline void MatrixSetIdentity(Matrix& a){
		a.m[0][0] = a.m[1][1] = a.m[2][2] = a.m[3][3] = 1.0f;
		a.m[0][1] = a.m[0][2] = a.m[0][3] = 0.0f;
		a.m[1][0] = a.m[1][2] = a.m[1][3] = 0.0f;
		a.m[2][0] = a.m[2][1] = a.m[2][3] = 0.0f;
		a.m[3][0] = a.m[3][1] = a.m[3][2] = 0.0f;
	}

	inline void MatrixSetZero(Matrix& a){
		a.m[0][0] = a.m[0][1] = a.m[0][2] = a.m[0][3] = 0.0f;
		a.m[1][0] = a.m[1][1] = a.m[1][2] = a.m[1][3] = 0.0f;
		a.m[2][0] = a.m[2][1] = a.m[2][2] = a.m[2][3] = 0.0f;
		a.m[3][0] = a.m[3][1] = a.m[3][2] = a.m[3][3] = 0.0f;
	}
}

#endif