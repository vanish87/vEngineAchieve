//
//Eggache implementing of template methods.
//The implementation of template class is in Math.hpp file.
//The rest of method implementation is in Math.cpp file.

#ifndef MATH_H_
#define MATH_H_

#pragma once

#include "Common/Header/CommonPreDec.h"

namespace vEngine
{
	//
	namespace Math
	{
		//constant
		float const PI = 3.141592653f;
		float const E  = 2.718281828f;

		template <typename T>
		bool IsNAN(T& x);

		template <typename T>
		bool IsINF(T& x);

		float Abs(float num);
		bool IsFloatEqual(float& lhs, float& rhs);
		bool IsFloatEqual(const float& lhs, const float& rhs);

		bool IsFloatEqual(float& lhs, float& rhs, float epsilon);

		template <typename T>
		Matrix<T> Multiply(const Matrix<T> & lhs, const Matrix<T> & rhs);

		template <typename T>
		Matrix2D<T> Multiply(const Matrix2D<T> & lhs, const Matrix2D<T> & rhs);

		float Sqrt(float x);
		float Sin(float x);
		float Cos(float x);
		float ArcCos( float x );
		float Tan(float x);
		float ArcTan( float x );
		float Cot(float x);
		float InvSqrt(float number);
		
		float Ln(float x);
		float Pow(float base, float exp);

		template <typename T>
		T Min(T& lhs, T& rhs);
		template <typename T>
		T Max(T& lhs, T& rhs);

		template <typename T>
		T Dot(Vec2<T> const & lhs, Vec2<T> const & rhs);

		template <typename T>
		T Dot(Vec3<T> const & lhs, Vec3<T> const & rhs);

		template <typename T>
		Matrix2D<T> OuterProduct(Vec2<T> const & lhs, Vec2<T> const & rhs);

		template <typename T> 
		Vec3<T> Cross(Vec3<T> const & lhs, Vec3<T> const & rhs);

		template <typename T> 
		T Normalize(T const & rhs);

		template <typename T> 
		Vec3<T> TransformPoint(const Vec3<T> & lhs, const Matrix<T> & rhs);
		template <typename T>
		Vec3<T> TransformVector(const Vec3<T> & lhs, const Matrix<T> & rhs);

		template <typename T>
		Vec4<T> Transform(const Vec4<T> & lhs, const Matrix<T> & rhs);


		template <typename T>
		Matrix<T> LookAtLH(const Vec3<T> & eye, const Vec3<T> & at, const Vec3<T> & up);

		template <typename T>
		Matrix<T> PerspectiveFovLH(const T fovy, const T aspect, const T zn,const T zf);

		template <typename T>
		void Identity(Matrix<T> & lhs);

		template <typename T>
		Matrix<T>  Inverse(Matrix<T> & lhs);

		template <typename T>
		Matrix2D<T>  Inverse(Matrix2D<T> & lhs);

		template <typename T>
		Matrix<T>  Transpose(Matrix<T> & lhs);

		template <typename T>
		Matrix<T>  InverTranspose(Matrix<T> & lhs);

		template <typename T>
		void XRotation(Matrix<T> & lhs, float theta);

		template <typename T>
		void YRotation(Matrix<T> & lhs, float theta);

		template <typename T>
		void ZRotation(Matrix<T> & lhs, float theta);

		template <typename T>
		void RotationAxis(Matrix<T> & lhs, Vec3<T> & axis, float theta);

		template <typename T>
		void Translate(Matrix<T> & lhs, float x, float y, float z);

		template <typename T>
		void Scale(Matrix<T> & lhs, float scale);

		template <typename T>
		Vec3<T> Max(Vec3<T> & lhs, Vec3<T> & rhs);

		template <typename T>
		Vec3<T> Min(Vec3<T> & lhs, Vec3<T> & rhs);

		template <typename T>
		T Clamp(const T& value, const T& min,const T& max );

		bool IntersectRayAABB(Ray* ray, AABBox* aabb);
		bool IntersectRayTriangle(Ray* ray, float3 a, float3 b, float3 c, float & t);
		bool GetQuadraticRoot(float2& Roots, const float a, const float b, const float c);

		float3 GetBSpline(const float3 value);
		float3 GetBSplineDerivative(const float3 value);

		//https://github.com/Washington-University/workbench/blob/master/src/Nifti/Matrix4x4.cxx
		void GetSVD(float4x4 A, float U[3][3], float D[3][3], float Vt[3][3]);

		void GetPolarDecomposition2D(float2x2 A, float2x2& R, float2x2& S);

		//implementation of this paper
		//http://www.math.ucla.edu/~fuchuyuan/svd/paper.pdf
		//
		//in the end of this function, I TRANSPOSE V so it will return V-Transpose instead of V
		//this is different with original paper, which returns V
		void GetSVD2D(float2x2 A, float2x2& U, float2& D, float2x2& Vt);
		void GetSVD3D(float3x3 A, float3x3& U, float3& D, float3x3& Vt);
	}

}

#include "Common/Header/Math.hpp"
#endif