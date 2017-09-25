#ifndef VECTOR_H_
#define VECTOR_H_

//#include <assert.h>
#include <array>
#include "Common/Header/CommonPreDec.h"

#pragma once

namespace vEngine
{
	template <typename T>
	class Vec2
	{
	private:
		typedef std::array<T, 2>	DataType;
		

	private:
		DataType vec_;
		static const short size_ = 2;

	public:
		Vec2()
		{
			vec_.fill(0);
		}
		explicit Vec2(const T & x, const T & y)
		{
			this->x() = x;
			this->y() = y;
		}
		Vec2(const Vec2 & rhs)//copy constructor
		{
			this->x() = rhs.x();
			this->y() = rhs.y();
		}

		T& x()
		{
			return vec_[0];
		}

		T& y()
		{
			return vec_[1];
		}

		const T& x() const
		{
			return vec_[0];
		}

		const T& y() const
		{
			return vec_[1];
		}

		static size_t size()
		{
			return size_;
		}

		Vec2& operator=(const Vec2 & rhs)
		{
			if (this == &rhs) return *this;
			vec_ = rhs.vec_;
			return *this;
		}

		T& operator[](size_t index)
		{
			CHECK_ASSERT(index < 2);
			return vec_[index];
		}

		/*
		you should explicitly indicate the type convertion you want to do;
		for example, if you want assign to float2 with sum of int2 and float2, you should write like this:
		int2 a;float2 b,c;
		c = b + a;//not c = a + b;
		you should make sure that the type of c's is same as lhs(b)'s in the expression;
		*/
		template <typename S>
		Vec2 operator+(const Vec2<S> & rhs) const
		{ 
			return Vec2(vec_[0] + rhs.x() , vec_[1] + rhs.y());
		}


		template <typename S>
		Vec2 operator-(const Vec2<S> & rhs) const
		{ 
			return Vec2(vec_[0] - rhs.x() , vec_[1] - rhs.y());
		}

		Vec2 operator*(const T & rhs) const
		{ 
			return Vec2(vec_[0] * rhs , vec_[1] * rhs);
		}

		Vec2 operator/(const T & rhs) const
		{ 
			CHECK_ASSERT( rhs != 0);
			return Vec2(vec_[0] * (1.0f / rhs) , vec_[1]* (1.0f  / rhs));
		}


	};

	template <typename T>
	class Vec3
	{
	private:
		typedef std::array<T, 3>	DataType;


	private:
		DataType vec_;
		static const short size_ = 3;

	public:
		Vec3()
		{
		}
		explicit Vec3(const T & x, const T & y, const T & z)
		{
			this->x() = x;
			this->y() = y;
			this->z() = z;
		}
		Vec3(const Vec3 & rhs)//copy constructor
		{
			this->x() = rhs.x();
			this->y() = rhs.y();
			this->z() = rhs.z();
		}

		T& x()
		{
			return vec_[0];
		}

		T& y()
		{
			return vec_[1];
		}

		T& z()
		{
			return vec_[2];
		}

		const T& x() const
		{
			return vec_[0];
		}

		const T& y() const
		{
			return vec_[1];
		}

		const T& z() const
		{
			return vec_[2];
		}

		static size_t size()
		{
			return size_;
		}

		Vec3& operator=(const Vec3 & rhs)
		{
			if (this == &rhs) return *this;
			vec_ = rhs.vec_;
			return *this;
		}

		T& operator[](size_t index)
		{
			CHECK_ASSERT(index < 3);
			return vec_[index];
		}

		template <typename S>
		Vec3 operator+(const Vec3<S> & rhs) const
		{ 
			return Vec3(vec_[0] + rhs.x() , vec_[1] + rhs.y() , vec_[2] + rhs.z());
		}

		template <typename S>
		Vec3 operator-(const Vec3<S> & rhs) const
		{ 
			return Vec3(vec_[0] - rhs.x() , vec_[1] - rhs.y() , vec_[2] - rhs.z());
		}

		Vec3 operator*(const T & rhs) const
		{ 
			return Vec3(vec_[0] * rhs , vec_[1] * rhs , vec_[2] * rhs);
		}

		Vec3 operator/(const T & rhs) const
		{ 
			CHECK_ASSERT( rhs != 0);
			return Vec3(vec_[0] * (1.0f / rhs) , vec_[1]* (1.0f  / rhs) , vec_[2]* (1.0f  / rhs));
		}

		void Print()
		{
			std::cout<< vec_[0] <<" " << vec_[1] <<" " << vec_[2] <<std::endl;
		}

	};

	template <typename T>
	class Vec4
	{
	private:
		typedef std::array<T, 4>	DataType;


	private:
		DataType vec_;
		static const short size_ = 4;

	public:
		Vec4()
		{
		}
		explicit Vec4(const T & x, const T & y, const T & z, const T & w)
		{
			this->x() = x;
			this->y() = y;
			this->z() = z;
			this->w() = w;
		}
		Vec4(const Vec4 & rhs)//copy constructor
		{
			this->x() = rhs.x();
			this->y() = rhs.y();
			this->z() = rhs.z();
			this->w() = rhs.w();
		}

		T& x()
		{
			return vec_[0];
		}

		T& y()
		{
			return vec_[1];
		}

		T& z()
		{
			return vec_[2];
		}

		T& w()
		{
			return vec_[3];
		}

		const T& x() const
		{
			return vec_[0];
		}

		const T& y() const
		{
			return vec_[1];
		}

		const T& z() const
		{
			return vec_[2];
		}

		const T& w() const
		{
			return vec_[3];
		}

		static size_t size()
		{
			return size_;
		}

		Vec4& operator=(const Vec4 & rhs)
		{
			if (this == &rhs) return *this;
			vec_ = rhs.vec_;
			return *this;
		}

		const T& operator[](size_t index) const
		{
			CHECK_ASSERT(index < 4);
			return vec_[index];
		}

		T& operator[](size_t index)
		{
			CHECK_ASSERT(index < 4);
			return vec_[index];
		}

		template <typename S>
		Vec4 operator+(const Vec4<S> & rhs) const
		{ 
			return Vec4(vec_[0] + rhs.x() , vec_[1] + rhs.y() , vec_[2] + rhs.z() , vec_[3] + rhs.w());
		}

		template <typename S>
		Vec4 operator-(const Vec4<S> & rhs) const
		{ 
			return Vec4(vec_[0] - rhs.x() , vec_[1] - rhs.y() , vec_[2] - rhs.z() , vec_[3] - rhs.w());
		}		
		
		Vec4 operator*(const T & rhs) const
		{ 
			return Vec4(vec_[0] * rhs , vec_[1] * rhs , vec_[2] * rhs , vec_[3] * rhs);
		}

		Vec4 operator/(const T & rhs) const
		{ 
			CHECK_ASSERT( rhs != 0);
			return Vec4(vec_[0] * (1.0f / rhs) , vec_[1]* (1.0f  / rhs) , vec_[2]* (1.0f  / rhs),vec_[3]* (1.0f  / rhs));
		}
	};


}

#endif