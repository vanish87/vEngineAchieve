#ifndef MATRIX_H_
#define MATRIX_H_

#pragma once

#include "Common/Header/Vector.h"

namespace vEngine
{

	

	template <typename T>
	class Matrix
	{
	private:
		Vec4<Vec4<T>> data_;		

		static const short row_ = 4; 
		static const short col_ = 4;
		static const short size_ = 16; 

	public:
		Matrix()
		{
			data_[0][0] = 0;	data_[0][1] = 0;	data_[0][2] = 0;	data_[0][3] = 0;
			data_[1][0] = 0;	data_[1][1] = 0;	data_[1][2] = 0;	data_[1][3] = 0;
			data_[2][0] = 0;	data_[2][1] = 0;	data_[2][2] = 0;	data_[2][3] = 0;
			data_[3][0] = 0;	data_[3][1] = 0;	data_[3][2] = 0;	data_[3][3] = 0;
		}
		explicit Matrix(const Vec4<T> & x, const Vec4<T> & y, const Vec4<T> & z, const Vec4<T> & w )
		{
			data_[0] = x;
			data_[1] = y;
			data_[2] = z;
			data_[3] = w;
		}
		explicit Matrix(const T & e11, const T & e12, const T & e13, const T & e14,
						const T & e21, const T & e22, const T & e23, const T & e24,
						const T & e31, const T & e32, const T & e33, const T & e34,
						const T & e41, const T & e42, const T & e43, const T & e44 )
		{
			data_[0][0] = e11;	data_[0][1] = e12;	data_[0][2] = e13;	data_[0][3] = e14;
			data_[1][0] = e21;	data_[1][1] = e22;	data_[1][2] = e23;	data_[1][3] = e24;
			data_[2][0] = e31;	data_[2][1] = e32;	data_[2][2] = e33;	data_[2][3] = e34;
			data_[3][0] = e41;	data_[3][1] = e42;	data_[3][2] = e43;	data_[3][3] = e44;
		}
		explicit Matrix(const T (&src)[4][4] )
		{
			for(size_t i = 0; i < row_ ; i++)
				for(size_t j = 0; j < col_ ; j++)
					data_[i][j] = src[i][j];
		}

		Matrix(const Matrix & rhs)//copy constructor
		{
			data_[0] = rhs.Row(0);
			data_[1] = rhs.Row(1);
			data_[2] = rhs.Row(2);
			data_[3] = rhs.Row(3);
		}

		Matrix& operator=(const Matrix & rhs)
		{
			if (this == &rhs) return *this;
			data_ = rhs.data_;
			return *this;
		}

		static size_t size(){ return size_; }
		static size_t row(){return row_;}
		static size_t col(){return col_;}

		T& operator()(size_t row, size_t col)
		{ 
			return data_[row][col]; 
		}
		const T& operator()(size_t row, size_t col) const
		{ 
			return data_[row][col]; 
		}

		const Vec4<T> & Row(size_t index) const
		{ 
			return data_[index]; 
		}	

		Vec4<T> & Row(size_t index)
		{ 
			return data_[index]; 
		}

		const Vec4<T> &	operator[]( int index ) const //get const row
		{
			return data_[index];
		}
		Vec4<T> &		operator[]( int index ) //get row
		{
			return data_[index];
		}

		template <typename S>
		Matrix<T>		operator*( const Matrix<S> & rhs ) const
		{
			Matrix<T> ret = Math::Multiply(*this, rhs);
			return ret;
		}

		template <typename S>
		Matrix<T>		operator+(const Matrix<S> & rhs) const
		{
			Matrix<T> ret;
			ret[0] = this->data_[0] + rhs[0];
			ret[1] = this->data_[1] + rhs[1];
			ret[2] = this->data_[2] + rhs[2];
			ret[3] = this->data_[3] + rhs[3];
			return ret;
		}

		template <typename S>
		Matrix<T> operator-(const Matrix<S> & rhs) const
		{
			Matrix<T> ret;
			ret[0] = this->data_[0] - rhs[0];
			ret[1] = this->data_[1] - rhs[1];
			ret[2] = this->data_[2] - rhs[2];
			ret[3] = this->data_[3] - rhs[3];
			return ret;
		}

		//here to use same T for ambiguous operator
		Matrix<T> operator*(const T & rhs) const
		{
			Matrix<T> ret;
			ret[0] = data_[0] * rhs;
			ret[1] = data_[1] * rhs;
			ret[2] = data_[2] * rhs;
			ret[3] = data_[3] * rhs;
			return ret;
		}
	};

	template <typename T>
	class Matrix2D
	{
	private:
		Vec2<Vec2<T>> data_;

		static const short row_ = 2;
		static const short col_ = 2;
		static const short size_ = 2;

	public:
		Matrix2D()
		{
			data_[0][0] = 0;	data_[0][1] = 0;
			data_[1][0] = 0;	data_[1][1] = 0;
		}
		explicit Matrix2D(const Vec2<T> & x, const Vec2<T> & y)
		{
			data_[0] = x;
			data_[1] = y;
		}
		explicit Matrix2D(const T & e11, const T & e12,
			const T & e21, const T & e22)
		{
			data_[0][0] = e11;	data_[0][1] = e12;
			data_[1][0] = e21;	data_[1][1] = e22;

		}
		explicit Matrix2D(const T(&src)[2][2])
		{
			for (size_t i = 0; i < row_; i++)
				for (size_t j = 0; j < col_; j++)
					data_[i][j] = src[i][j];
		}

		Matrix2D(const Matrix2D & rhs)//copy constructor
		{
			data_[0] = rhs.Row(0);
			data_[1] = rhs.Row(1);
		}

		Matrix2D& operator=(const Matrix2D & rhs)
		{
			if (this == &rhs) return *this;
			data_ = rhs.data_;
			return *this;
		}

		static size_t size() { return size_; }
		static size_t row() { return row_; }
		static size_t col() { return col_; }

		T& operator()(size_t row, size_t col)
		{
			return data_[row][col];
		}
		const T& operator()(size_t row, size_t col) const
		{
			return data_[row][col];
		}

		const Vec2<T> & Row(size_t index) const
		{
			CHECK_ASSERT(index < this->row_);
			return data_[index];
		}

		Vec2<T> & Row(size_t index)
		{
			CHECK_ASSERT(index < this->row_);
			return data_[index];
		}

		const Vec2<T> &	operator[](int index) const //get const row
		{
			CHECK_ASSERT(index < this->row_);
			return data_[index];
		}
		Vec2<T> &		operator[](int index) //get row
		{
			CHECK_ASSERT(index < this->row_);
			return data_[index];
		}

		template <typename S>
		Matrix2D<T>		operator*(const Matrix2D<S> & rhs) const
		{
			Matrix2D<T> ret = Math::Multiply(*this, rhs);
			return ret;
		}

		template <typename S>
		Matrix2D<T>		operator+(const Matrix2D<S> & rhs) const
		{
			Matrix2D<T> ret;
			ret[0] = this->data_[0] + rhs[0];
			ret[1] = this->data_[1] + rhs[1];
			return ret;
		}

		template <typename S>
		Matrix2D<T>		operator-(const Matrix2D<S> & rhs) const
		{
			Matrix2D<T> ret;
			ret[0] = this->data_[0] - rhs[0];
			ret[1] = this->data_[1] - rhs[1];
			return ret;
		}

		Matrix2D<T> 	operator*(const T & rhs) const
		{
			Matrix2D<T> ret;
			ret[0] = data_[0] * rhs;
			ret[1] = data_[1] * rhs;
			return ret;
		}

		void Print()
		{
			PRINT(data_[0][0] << " " << data_[0][1]);
			PRINT(data_[1][0] << " " << data_[1][1]);
		}

	};

	template <typename T>
	class Matrix3D
	{
	private:
		Vec3<Vec3<T>> data_;

		static const short row_ = 3;
		static const short col_ = 3;
		static const short size_ = 3;

	public:
		Matrix3D()
		{
			data_[0][0] = 0;	data_[0][1] = 0; data_[0][2] = 0;
			data_[1][0] = 0;	data_[1][1] = 0; data_[1][2] = 0;
			data_[2][0] = 0;	data_[2][1] = 0; data_[2][2] = 0;
		}
		explicit Matrix3D(const Vec3<T> & x, const Vec3<T> & y, const Vec3<T> & z)
		{
			data_[0] = x;
			data_[1] = y;
			data_[2] = z;
		}
		explicit Matrix3D(const T & e11, const T & e12, const T & e13,
						  const T & e21, const T & e22, const T & e23,
						  const T & e31, const T & e32, const T & e33)
		{
			data_[0][0] = e11;	data_[0][1] = e12, data_[0][2] = e13;
			data_[1][0] = e21;	data_[1][1] = e22, data_[1][2] = e23;
			data_[2][0] = e31;	data_[2][1] = e32, data_[2][2] = e33;

		}
		explicit Matrix3D(const T(&src)[3][3])
		{
			for (size_t i = 0; i < row_; i++)
				for (size_t j = 0; j < col_; j++)
					data_[i][j] = src[i][j];
		}

		Matrix3D(const Matrix3D & rhs)//copy constructor
		{
			data_[0] = rhs.Row(0);
			data_[1] = rhs.Row(1);
			data_[2] = rhs.Row(2);
		}

		Matrix3D& operator=(const Matrix3D & rhs)
		{
			if (this == &rhs) return *this;
			data_ = rhs.data_;
			return *this;
		}

		static size_t size() { return size_; }
		static size_t row() { return row_; }
		static size_t col() { return col_; }

		T& operator()(size_t row, size_t col)
		{
			return data_[row][col];
		}
		const T& operator()(size_t row, size_t col) const
		{
			return data_[row][col];
		}

		const Vec3<T> & Row(size_t index) const
		{
			CHECK_ASSERT(index < this->row_);
			return data_[index];
		}

		Vec3<T> & Row(size_t index)
		{
			CHECK_ASSERT(index < this->row_);
			return data_[index];
		}

		const Vec3<T> &	operator[](int index) const //get const row
		{
			CHECK_ASSERT(index < this->row_);
			return data_[index];
		}
		Vec3<T> &		operator[](int index) //get row
		{
			CHECK_ASSERT(index < this->row_);
			return data_[index];
		}

		template <typename S>
		Matrix3D<T>		operator*(const Matrix3D<S> & rhs) const
		{
			Matrix3D<T> ret = Math::Multiply(*this, rhs);
			return ret;
		}

		template <typename S>
		Matrix3D<T>		operator+(const Matrix3D<S> & rhs) const
		{
			Matrix3D<T> ret;
			ret[0] = this->data_[0] + rhs[0];
			ret[1] = this->data_[1] + rhs[1];
			ret[1] = this->data_[2] + rhs[2];
			return ret;
		}

		template <typename S>
		Matrix3D<T>		operator-(const Matrix3D<S> & rhs) const
		{
			Matrix3D<T> ret;
			ret[0] = this->data_[0] - rhs[0];
			ret[1] = this->data_[1] - rhs[1];
			ret[1] = this->data_[2] - rhs[2];
			return ret;
		}

		Matrix3D<T> 	operator*(const T & rhs) const
		{
			Matrix3D<T> ret;
			ret[0] = data_[0] * rhs;
			ret[1] = data_[1] * rhs;
			ret[2] = data_[2] * rhs;
			return ret;
		}

		void Print()
		{
			PRINT(data_[0][0] << " " << data_[0][1] << " " << data_[0][2]);
			PRINT(data_[1][0] << " " << data_[1][1] << " " << data_[1][2]);
			PRINT(data_[2][0] << " " << data_[2][1] << " " << data_[2][2]);
		}

	};

	template <typename T>
	Matrix2D<T>	operator*(const T & lhs, const Matrix2D<T> & rhs)
	{
		return rhs * lhs;
	}


	template <typename T>
	Matrix3D<T>	operator*(const T & lhs, const Matrix3D<T> & rhs)
	{
		return rhs * lhs;
	}

	template <typename T>
	Matrix<T>	operator*(const T & lhs, const Matrix<T> & rhs)
	{
		return rhs * lhs;
	}


	
}

#endif