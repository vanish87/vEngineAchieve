#ifndef MATRIX_H_
#define MATRIX_H_

#pragma once

#include "Common\Header\Vector.h"

namespace MocapGE
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

		Matrix<T> &		operator*(T const & rhs)
		{
			for (size_t i = 0; i < row_; ++ i)
			{
				data_[i] =data_[i]* rhs;
			}
			return *this;
		}


	};
}

#endif