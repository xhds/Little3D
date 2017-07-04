#ifndef __L3D_GRAPHICS_H__
#define __L3D_GRAPHICS_H__
#include "Math.h"

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
		L3DMath::MatrixSetZero(ret);
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

	void MakePerspectiveMatrixLH(L3DMath::Matrix& ret, float width, float height, float near, float far){
		L3DMath::MatrixSetZero(ret);
		ret.m[0][0] = 2.0f * near / width;
		ret.m[1][1] = 2.0f * near / height;
		ret.m[2][2] = far / (far - near);
		ret.m[3][2] = -1.0f * near * far / (far - near);
		ret.m[2][3] = 1.0f;
	}

	void MakePerspectiveMatrixFOVLH(L3DMath::Matrix& ret, float fovh, float aspect, float near, float far){
		L3DMath::MatrixSetZero(ret);
		float r = 1.0f / tan(fovh * 0.5f);
		ret.m[0][0] = r;
		ret.m[1][1] = r * aspect;
		ret.m[2][2] = far / (far - near);
		ret.m[3][2] = -1.0f * near * far / (far - near);
		ret.m[2][3] = 1.0f;
	}
}
#endif