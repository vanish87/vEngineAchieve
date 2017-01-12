#ifndef COMMON_PREDEC_H_
#define COMMON_PREDEC_H_

//#pragma warning(disable:4819 4355) take care

//deal with the 'max' macro in windows.h colliding with 'max' in 'std'
//#ifndef NOMINMAX
//#define NOMINMAX
//#endif // !NOMINMAX

#pragma once

//dll defines
//#define MOCAPGE_API __declspec(dllexport)
#define MOCAPGE_API

#include <assert.h>
//#include "boost/smart_ptr.hpp"

#include <iostream>
#define PRINT(x) std::cout<<x<<std::endl;

namespace MocapGE
{
	typedef __int8  int8_t;
	typedef __int32 int32_t;
	typedef __int64 int64_t;
	typedef unsigned __int8  uint8_t;
	typedef unsigned __int32 uint32_t;
	typedef unsigned __int64 uint64_t;

	//template<typename T>
	//class Matrix;

	//class XMLParser;
	//typedef std::shared_ptr<XMLParser> XMLParserPtr;

	template <typename T>
	class Vec2;
	typedef Vec2<int32_t> int2;
	typedef Vec2<float> float2;

	template <typename T>
	class Vec3;
	typedef Vec3<int32_t> int3;
	typedef Vec3<float> float3;

	template <typename T>
	class Vec4;
	typedef Vec4<int32_t> int4;	
	typedef Vec4<float> float4;

 	template <typename T>
 	class Matrix;
	typedef Matrix<float> float4x4;

	class Timer;

	template <typename T>
	class Sphere;
	typedef Sphere<float> sphere;

	class Ray;
	class AABBox;
}

#endif