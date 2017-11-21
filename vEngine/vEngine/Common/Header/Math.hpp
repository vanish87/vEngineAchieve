#ifndef MATH_HPP_
#define MATH_HPP_

#pragma once
#include <algorithm>
#include <random>

namespace vEngine
{
	namespace Math
	{
		template <typename T>
		bool IsNAN(T& x)
		{
			return (std::isnan)(x);
		}

		template <typename T>
		bool IsINF(T& x)
		{
			return (std::isinf)(x);
		}				

		bool IsFloatEqual(float& lhs, float& rhs);

		template <typename T>
		T Clamp(const T& value, const T& min,const T& max )
		{
			return value < min ? min : value > max ? max : value;
		}
		template <typename T>
		T Min(const T& lhs,const T& rhs)
		{
			return (std::min)(lhs,rhs);
		}
		template <typename T>
		T Max(const T& lhs,const T& rhs)
		{
			return (std::max)(lhs,rhs);
		}
		template <typename T>
		Matrix<T>	Multiply( const Matrix<T> & lhs, const Matrix<T> & rhs )
		{
			CHECK_ASSERT( lhs.row() == rhs.col() );
			Matrix<T> ret;
			size_t dim = 4;

			for (int row = 0 ; row < dim ; row++)
				for (int col = 0 ; col < dim ; col++) 
				{
					ret[row][col] = 0;
					for (int i = 0 ; i < dim ; i++)
						ret[row][col] += lhs[row][i] * rhs[i][col];
				}
			return ret;
		}

		template <typename T>
		Matrix2D<T>	Multiply(const Matrix2D<T> & lhs, const Matrix2D<T> & rhs)
		{
			CHECK_ASSERT(lhs.row() == rhs.col());
			Matrix2D<T> ret;
			size_t dim = 2;

			for (int row = 0; row < dim; row++)
				for (int col = 0; col < dim; col++)
				{
					ret[row][col] = 0;
					for (int i = 0; i < dim; i++)
						ret[row][col] += lhs[row][i] * rhs[i][col];
				}
			return ret;
		}

		template <typename T>
		T Dot(Vec3<T> const & lhs, Vec3<T> const & rhs)
		{
			return T(lhs.x()*rhs.x() + lhs.y()*rhs.y() + lhs.z()* rhs.z());
		}

		template <typename T>
		Matrix2D<T> OuterProduct(Vec2<T> const & lhs, Vec2<T> const & rhs)
		{
			return Matrix2D<T>(lhs[0] * rhs[0], lhs[0] * rhs[1],
							   lhs[1] * rhs[0], lhs[1] * rhs[1]);
		}

		template <typename T>
		Vec3<T> Cross(Vec3<T> const & lhs, Vec3<T> const & rhs)
		{
			return Vec3<T>(lhs.y() * rhs.z() - lhs.z() * rhs.y(),
				lhs.z() * rhs.x() - lhs.x() * rhs.z(),
				lhs.x() * rhs.y() - lhs.y() * rhs.x());
		}


		template <typename T>
		T Normalize(T const & rhs)
		{
			return rhs * InvSqrt(Dot(rhs , rhs));
		}
		template <typename T>
		Vec3<T> TransformPoint(const Vec3<T> & lhs, const Matrix<T> & rhs)
		{
			Vec3<T> ret;
			ret.x() = lhs.x() * rhs[0][0] + lhs.y() * rhs[1][0] + lhs.z() * rhs[2][0] + 1 * rhs[3][0];
			ret.y() = lhs.x() * rhs[0][1] + lhs.y() * rhs[1][1] + lhs.z() * rhs[2][1] + 1 * rhs[3][1];
			ret.z() = lhs.x() * rhs[0][2] + lhs.y() * rhs[1][2] + lhs.z() * rhs[2][2] + 1 * rhs[3][2];
			T w		= lhs.x() * rhs[0][3] + lhs.y() * rhs[1][3] + lhs.z() * rhs[2][3] + 1 * rhs[3][3];
			ret.x() = ret.x() / w;
			ret.y() = ret.y() / w;
			ret.z() = ret.z() / w;
			return ret;
		}
		template <typename T>
		Vec3<T> TransformVector(const Vec3<T> & lhs, const Matrix<T> & rhs)
		{
			Vec3<T> ret;
			ret.x() = lhs.x() * rhs[0][0] + lhs.y() * rhs[1][0] + lhs.z() * rhs[2][0] ;
			ret.y() = lhs.x() * rhs[0][1] + lhs.y() * rhs[1][1] + lhs.z() * rhs[2][1] ;
			ret.z() = lhs.x() * rhs[0][2] + lhs.y() * rhs[1][2] + lhs.z() * rhs[2][2] ;
			return ret;
		}

		template <typename T>
		Vec4<T>	Transform( const Vec4<T> & lhs, const Matrix<T> & rhs )
		{
			Vec4<T> ret;
			ret.x() = lhs.x() * rhs[0][0] + lhs.y() * rhs[1][0] + lhs.z() * rhs[2][0] + lhs.w() * rhs[3][0];
			ret.y() = lhs.x() * rhs[0][1] + lhs.y() * rhs[1][1] + lhs.z() * rhs[2][1] + lhs.w() * rhs[3][1];
			ret.z() = lhs.x() * rhs[0][2] + lhs.y() * rhs[1][2] + lhs.z() * rhs[2][2] + lhs.w() * rhs[3][2];
			ret.w()	= lhs.x() * rhs[0][3] + lhs.y() * rhs[1][3] + lhs.z() * rhs[2][3] + lhs.w() * rhs[3][3];
			return ret;
		}
		template <typename T>
		Matrix<T> LookAtLH( const Vec3<T> & eye, const Vec3<T> & at, const Vec3<T> & up )
		{
			Vec3<T> zaxis = Normalize(at - eye);
			Vec3<T> xaxis = Normalize(Cross(up, zaxis));
			Vec3<T> yaxis = Cross(zaxis, xaxis);

			return Matrix<T>(
				xaxis.x(),           yaxis.x(),           zaxis.x(),           0,
				xaxis.y(),           yaxis.y(),           zaxis.y(),           0,
				xaxis.z(),           yaxis.z(),           zaxis.z(),           0,
				-Dot(xaxis, eye),  -Dot(yaxis, eye),  -Dot(zaxis, eye),		   1);
		}
		template <typename T>
		Matrix<T> PerspectiveFovLH(const T fovy, const T aspect, const T zn,const T zf)
		{
			float y_scale = Cot(fovy / 2);
			float x_scale = y_scale / aspect;
			float m33 = zf/(zf-zn);

			return Matrix<T>(x_scale, 0,	   0,		0,
							 0,		  y_scale, 0,		0,
							 0,		  0,	   m33,		1,
							 0,		  0,	   -zn*m33, 0 );
		}
		template <typename T>
		void Identity(Matrix<T> & lhs)
		{
				lhs =  Matrix<T>(1, 0, 0, 0,
								 0, 1, 0, 0,
								 0, 0, 1, 0,
								 0, 0, 0, 1 );
		}

		template <typename T>
		void Identity(Matrix2D<T> & lhs)
		{
			lhs = Matrix2D<T>(1, 0,
							  0, 1) ;
		}

		template <typename T>
		Matrix<T>  Inverse(Matrix<T> & rhs)
		{
			//from KlayGE
			T const _2132_2231(rhs(1, 0) * rhs(2, 1) - rhs(1, 1) * rhs(2, 0));
			T const _2133_2331(rhs(1, 0) * rhs(2, 2) - rhs(1, 2) * rhs(2, 0));
			T const _2134_2431(rhs(1, 0) * rhs(2, 3) - rhs(1, 3) * rhs(2, 0));
			T const _2142_2241(rhs(1, 0) * rhs(3, 1) - rhs(1, 1) * rhs(3, 0));
			T const _2143_2341(rhs(1, 0) * rhs(3, 2) - rhs(1, 2) * rhs(3, 0));
			T const _2144_2441(rhs(1, 0) * rhs(3, 3) - rhs(1, 3) * rhs(3, 0));
			T const _2233_2332(rhs(1, 1) * rhs(2, 2) - rhs(1, 2) * rhs(2, 1));
			T const _2234_2432(rhs(1, 1) * rhs(2, 3) - rhs(1, 3) * rhs(2, 1));
			T const _2243_2342(rhs(1, 1) * rhs(3, 2) - rhs(1, 2) * rhs(3, 1));
			T const _2244_2442(rhs(1, 1) * rhs(3, 3) - rhs(1, 3) * rhs(3, 1));
			T const _2334_2433(rhs(1, 2) * rhs(2, 3) - rhs(1, 3) * rhs(2, 2));
			T const _2344_2443(rhs(1, 2) * rhs(3, 3) - rhs(1, 3) * rhs(3, 2));
			T const _3142_3241(rhs(2, 0) * rhs(3, 1) - rhs(2, 1) * rhs(3, 0));
			T const _3143_3341(rhs(2, 0) * rhs(3, 2) - rhs(2, 2) * rhs(3, 0));
			T const _3144_3441(rhs(2, 0) * rhs(3, 3) - rhs(2, 3) * rhs(3, 0));
			T const _3243_3342(rhs(2, 1) * rhs(3, 2) - rhs(2, 2) * rhs(3, 1));
			T const _3244_3442(rhs(2, 1) * rhs(3, 3) - rhs(2, 3) * rhs(3, 1));
			T const _3344_3443(rhs(2, 2) * rhs(3, 3) - rhs(2, 3) * rhs(3, 2));

			T const det(determinant(rhs));
			if (!det==0)
			{
				T invDet(T(1) / det);

				return Matrix<T>(
					+invDet * (rhs(1, 1) * _3344_3443 - rhs(1, 2) * _3244_3442 + rhs(1, 3) * _3243_3342),
					-invDet * (rhs(0, 1) * _3344_3443 - rhs(0, 2) * _3244_3442 + rhs(0, 3) * _3243_3342),
					+invDet * (rhs(0, 1) * _2344_2443 - rhs(0, 2) * _2244_2442 + rhs(0, 3) * _2243_2342),
					-invDet * (rhs(0, 1) * _2334_2433 - rhs(0, 2) * _2234_2432 + rhs(0, 3) * _2233_2332),

					-invDet * (rhs(1, 0) * _3344_3443 - rhs(1, 2) * _3144_3441 + rhs(1, 3) * _3143_3341),
					+invDet * (rhs(0, 0) * _3344_3443 - rhs(0, 2) * _3144_3441 + rhs(0, 3) * _3143_3341),
					-invDet * (rhs(0, 0) * _2344_2443 - rhs(0, 2) * _2144_2441 + rhs(0, 3) * _2143_2341),
					+invDet * (rhs(0, 0) * _2334_2433 - rhs(0, 2) * _2134_2431 + rhs(0, 3) * _2133_2331),

					+invDet * (rhs(1, 0) * _3244_3442 - rhs(1, 1) * _3144_3441 + rhs(1, 3) * _3142_3241),
					-invDet * (rhs(0, 0) * _3244_3442 - rhs(0, 1) * _3144_3441 + rhs(0, 3) * _3142_3241),
					+invDet * (rhs(0, 0) * _2244_2442 - rhs(0, 1) * _2144_2441 + rhs(0, 3) * _2142_2241),
					-invDet * (rhs(0, 0) * _2234_2432 - rhs(0, 1) * _2134_2431 + rhs(0, 3) * _2132_2231),

					-invDet * (rhs(1, 0) * _3243_3342 - rhs(1, 1) * _3143_3341 + rhs(1, 2) * _3142_3241),
					+invDet * (rhs(0, 0) * _3243_3342 - rhs(0, 1) * _3143_3341 + rhs(0, 2) * _3142_3241),
					-invDet * (rhs(0, 0) * _2243_2342 - rhs(0, 1) * _2143_2341 + rhs(0, 2) * _2142_2241),
					+invDet * (rhs(0, 0) * _2233_2332 - rhs(0, 1) * _2133_2331 + rhs(0, 2) * _2132_2231));
			}
			else
			{
				return rhs;
			}
		}

		template <typename T>
		Matrix2D<T>  Inverse(Matrix2D<T> & rhs)
		{
			float det = determinant(rhs);
			CHECK_ASSERT(det != 0);
			return Matrix2D<T>(
				rhs[1][1] / det, -rhs[1][0] / det,
				-rhs[0][1] / det, rhs[0][0] / det);
		}

		template <typename T>
		T determinant(Matrix<T> const & rhs)
		{
			//from KlayGE
			T const _3142_3241(rhs(2, 0) * rhs(3, 1) - rhs(2, 1) * rhs(3, 0));
			T const _3143_3341(rhs(2, 0) * rhs(3, 2) - rhs(2, 2) * rhs(3, 0));
			T const _3144_3441(rhs(2, 0) * rhs(3, 3) - rhs(2, 3) * rhs(3, 0));
			T const _3243_3342(rhs(2, 1) * rhs(3, 2) - rhs(2, 2) * rhs(3, 1));
			T const _3244_3442(rhs(2, 1) * rhs(3, 3) - rhs(2, 3) * rhs(3, 1));
			T const _3344_3443(rhs(2, 2) * rhs(3, 3) - rhs(2, 3) * rhs(3, 2));

			return rhs(0, 0) * (rhs(1, 1) * _3344_3443 - rhs(1, 2) * _3244_3442 + rhs(1, 3) * _3243_3342)
				- rhs(0, 1) * (rhs(1, 0) * _3344_3443 - rhs(1, 2) * _3144_3441 + rhs(1, 3) * _3143_3341)
				+ rhs(0, 2) * (rhs(1, 0) * _3244_3442 - rhs(1, 1) * _3144_3441 + rhs(1, 3) * _3142_3241)
				- rhs(0, 3) * (rhs(1, 0) * _3243_3342 - rhs(1, 1) * _3143_3341 + rhs(1, 2) * _3142_3241);
		}

		template <typename T>
		T determinant(Matrix2D<T> const & rhs)
		{
			return rhs[0][0] * rhs[1][1] - rhs[0][1] * rhs[1][0];
		}

		template <typename T>
		Matrix<T>  Transpose(Matrix<T> & lhs)
		{
			return Matrix<T>(
				lhs(0, 0), lhs(1, 0), lhs(2, 0), lhs(3, 0),
				lhs(0, 1), lhs(1, 1), lhs(2, 1), lhs(3, 1),
				lhs(0, 2), lhs(1, 2), lhs(2, 2), lhs(3, 2),
				lhs(0, 3), lhs(1, 3), lhs(2, 3), lhs(3, 3));
		}
		template <typename T>
		Matrix2D<T>  Transpose(Matrix2D<T> & lhs)
		{
			return Matrix2D<T>(
				lhs(0, 0), lhs(1, 0), 
				lhs(0, 1), lhs(1, 1)
				);
		}

		template <typename T>
		Matrix<T>  InverTranspose(Matrix<T> & lhs)
		{
			return Transpose(Inverse(lhs));
		}

		template <typename T>
		void XRotation(Matrix<T> & lhs, float theta)
		{
			Identity(lhs);
			lhs[1][1] = Math::Cos(theta);
			lhs[1][2] = Math::Sin(theta);
			lhs[2][1] = -Math::Sin(theta);
			lhs[2][2] = Math::Cos(theta);
		}

		template <typename T>
		void YRotation(Matrix<T> & lhs, float theta)
		{
			Identity(lhs);
			lhs[0][0] = Math::Cos(theta);
			lhs[0][2] = -Math::Sin(theta);
			lhs[2][0] = Math::Sin(theta);
			lhs[2][2] = Math::Cos(theta);
		}

		template <typename T>
		void ZRotation(Matrix<T> & lhs, float theta)
		{
			Identity(lhs);
			lhs[0][0] = Math::Cos(theta);
			lhs[0][1] = Math::Sin(theta);
			lhs[1][0] = -Math::Sin(theta);
			lhs[1][1] = Math::Cos(theta);
		}

		template <typename T>
		void RotationAxis(Matrix<T> & lhs, Vec3<T> & axis, float theta)
		{
			Identity(lhs);
			axis = Normalize(axis);

			lhs[0][0] = (1.0f - Cos(theta)) * axis.x() * axis.x() + Cos(theta);
			lhs[1][0] = (1.0f - Cos(theta)) * axis.x() * axis.y() + Sin(theta) * axis.z();
			lhs[2][0] = (1.0f - Cos(theta)) * axis.x() * axis.z() - Sin(theta) * axis.y();
			lhs[0][1] = (1.0f - Cos(theta)) * axis.y() * axis.x() - Sin(theta) * axis.z();
			lhs[1][1] = (1.0f - Cos(theta)) * axis.y() * axis.y() + Cos(theta);
			lhs[2][1] = (1.0f - Cos(theta)) * axis.y() * axis.z() + Sin(theta) * axis.x();
			lhs[0][2] = (1.0f - Cos(theta)) * axis.z() * axis.x() + Sin(theta) * axis.y();
			lhs[1][2] = (1.0f - Cos(theta)) * axis.z() * axis.y() - Sin(theta) * axis.x();
			lhs[2][2] = (1.0f - Cos(theta)) * axis.z() * axis.z() + Cos(theta);

		}

		template <typename T>
		void Translate(Matrix<T> & lhs, float x, float y, float z)
		{
			Identity(lhs);
			//left hand coordinate system
			lhs[3][0] = x;
			lhs[3][1] = y;
			lhs[3][2] = z;
		}

		template <typename T>
		void Scale(Matrix<T> & lhs, float scale)
		{
			Identity(lhs);

			lhs[0][0] = scale;
			lhs[1][1] = scale;
			lhs[2][2] = scale;
		}

		template <typename T>
		Vec3<T> Max(Vec3<T> & lhs, Vec3<T> & rhs)
		{
			Vec3<T> ret;
			ret.x() = std::max(lhs.x(), rhs.x());
			ret.y() = std::max(lhs.y(), rhs.y());
			ret.z() = std::max(lhs.z(), rhs.z());
			return ret;
		}

		template <typename T>
		Vec3<T> Min(Vec3<T> & lhs, Vec3<T> & rhs)
		{
			Vec3<T> ret;
			ret.x() = std::min(lhs.x(), rhs.x());
			ret.y() = std::min(lhs.y(), rhs.y());
			ret.z() = std::min(lhs.z(), rhs.z());
			return ret;
		}

		template <typename T>
		T RandomInt(const T& from, const T& to)
		{
			std::random_device rd;
			std::mt19937 generator(rd());
			std::uniform_int_distribution<T> distribution(from, to);
			return static_cast<T>(distribution(generator));
		}

		template <typename T>
		T RandomReal(const T& from, const T& to)
		{
			std::random_device rd;
			std::mt19937 generator(rd());
			std::uniform_real_distribution<T> distribution(from, to);
			return static_cast<T>(distribution(generator));
		}
	}
}

#endif