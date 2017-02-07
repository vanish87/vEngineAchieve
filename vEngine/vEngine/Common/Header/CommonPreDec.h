#ifndef COMMON_PREDEC_H_
#define COMMON_PREDEC_H_

//#pragma warning(disable:4819 4355) take care

//deal with the 'max' macro in windows.h colliding with 'max' in 'std'
//#ifndef NOMINMAX
//#define NOMINMAX
//#endif // !NOMINMAX

#pragma once

//dll defines
//#define VENGINE_API __declspec(dllexport)
#define VENGINE_API

//#include <assert.h>
//#include "boost/smart_ptr.hpp"

#include <iostream>
#define PRINT(x) std::cout<<x<<std::endl;
#define PRINT_AND_RETURN(x, returnVal) {PRINT(x);return returnVal;}
#define PRINT_FILE_AND_FUCTION PRINT("in File "<<__FILE__<<" Line "<<__LINE__<<" Function "<<__FUNCTION__);
#define PRINT_AND_ASSERT(x) \
{\
	PRINT(x); \
	PRINT_FILE_AND_FUCTION;\
	__debugbreak();\
}
#define CHECK_AND_ASSERT(condition,x) \
{\
	if(!(condition)){\
		PRINT(x); \
		PRINT_FILE_AND_FUCTION;\
		__debugbreak();}\
}
#define COMPILE_PRINT_AND_ASSERT(exp, x) {static_assert(exp, x);}

#define DEBUG_CLASS_FILE_NAME virtual std::string GetName() override { return __FILE__; };
#define DEBUG_CLASS_NAME(x) virtual std::string GetName() override { return x; };

//try to add Init function to init class_name
#define SINGLETON_PRAVATE(class_name)\
private:\
	class_name(); \
	class_name(class_name const&){}; \
	class_name& operator=(class_name const&){}; \
	~class_name(){}; 


//#define ENABLE_TEST

namespace vEngine
{
	typedef __int8  int8_t;
	typedef __int16 int16_t;
	typedef __int32 int32_t;
	typedef __int64 int64_t;
	typedef unsigned __int8  uint8_t;
	typedef unsigned __int16 uint16_t;
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