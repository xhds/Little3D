#ifndef __L3D_MATH_H__
#define __L3D_MATH_H__
#include <memory>
#include <math.h>

namespace L3DMath {
	class Vector {
	public:
		float x, y, z, w;
		Vector():x(0.0f), y(0.0f), z(0.0f), w(1.0f){}
	};

	class Matrix {
	public:
		float m[4][4];
		Matrix(){ memset(m, 0, 16 * sizeof(float));}
	};

	Vector VectorAdd(const Vector& a, const Vector& b){
		Vector ret;
		ret.x = a.x + b.x;
		ret.y = a.y + b.y;
		ret.z = a.z + b.z;
		return ret;
	}

	Vector VectorSub(const Vector& a, const Vector& b){
		Vector ret;
		ret.x = a.x - b.x;
		ret.y = a.y - b.y;
		ret.z = a.z - b.z;
		return ret;
	}

	float VectorMod(const Vector& a){
		return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
	}

	void VectorUnit(Vector& a){
		a.x = 1.0f;
		a.y = 1.0f;
		a.z = 1.0f;
		a.w = 1.0f;
	}

	void VectorSetNormalized(Vector& a){
		float inv_mod = 1.0f / VectorMod(a);
		a.x *= inv_mod;
		a.y *= inv_mod;
		a.z *= inv_mod;
	}

	Vector VectorGetNormalized(const Vector& a){
		float inv_mod = 1.0f / VectorMod(a);
		Vector ret;
		ret.x = a.x * inv_mod;
		ret.y = a.y * inv_mod;
		ret.z = a.z * inv_mod;
		ret.w = a.w;
		return ret;
	}

	float VectorDot(const Vector& a, const Vector& b){
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	Vector VectorCross(const Vector& a, const Vector& b){
		Vector ret;
		ret.x = a.y * b.z - a.z * b.y;
		ret.y = a.z * b.x - a.x * b.z;
		ret.z = a.x * b.y - a.y * b.x;
		return ret;
	}

	float FloatInterp(float a, float b, float t){
		return a * t + b * (1.0f - t);
	}

	Vector VectorInterp(const Vector& a, const Vector& b, float t){
		Vector ret;
		ret.x = FloatInterp(a.x, b.x, t);
		ret.y = FloatInterp(a.y, b.y, t);
		ret.z = FloatInterp(a.z, b.z, t);
		ret.w = 1.0f;
		return ret;
	}

	Matrix MatrixMul(const Matrix& a, const Matrix& b){
		Matrix ret;
		for (int row = 0; row < 4; ++row){
			for (int col = 0; col < 4; ++i) {
				ret[row][col] = a[row][0] * b[0][col] + a[row][1] * b[1][col] + a[row][2] * b[2][col] + a[row][3] * b[3][col];
			}
		}
		return ret;
	}

	Vector VectorMulMatrix(const Vector& a, const Matrix& b){
		Vector ret;
		ret.x = a.x * b[0][0] + a.y * b[1][0] + a.z * b[2][0] + a.w * b[3][0];
		ret.y = a.x * b[0][1] + a.y * b[1][1] + a.z * b[2][1] + a.w * b[3][1];
		ret.z = a.x * b[0][2] + a.y * b[1][2] + a.z * b[2][2] + a.w * b[3][2];
		ret.w = a.x * b[0][3] + a.y * b[1][3] + a.z * b[2][3] + a.w * b[3][3];
		return ret;
	}

	void MatrixSetIdentity(Matrix& a){
		a[0][0] = a[1][1] = a[2][2] = a[3][3] = 1.0f;
		a[0][1] = a[0][2] = a[0][3] = 0.0f;
		a[1][0] = a[1][2] = a[1][3] = 0.0f;
		a[2][0] = a[2][1] = a[2][3] = 0.0f;
		a[3][0] = a[3][1] = a[3][2] = 0.0f;
	}

	void MatrixSetZero(Matrix& a){
		a[0][0] = a[0][1] = a[0][2] = a[0][3] = 0.0f;
		a[1][0] = a[1][1] = a[1][2] = a[1][3] = 0.0f;
		a[2][0] = a[2][1] = a[2][2] = a[2][3] = 0.0f;
		a[3][0] = a[3][1] = a[3][2] = a[3][3] = 0.0f;
	}
}

#endif