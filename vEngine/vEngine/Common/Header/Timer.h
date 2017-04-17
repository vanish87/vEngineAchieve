#ifndef TIMER_H_
#define TIMER_H_

#pragma once

#ifdef _MSC_VER
#define USE_WINDOWS_TIME_QUERY
#endif

#ifdef USE_WINDOWS_TIME_QUERY
#include <windows.h>
#else
#include <chrono>
#endif

#include "Common\Header\CommonPreDec.h"

namespace vEngine
{
	class Timer
	{
	public:
		Timer(void);
		~Timer(void);

		void Retart();
		//return time in ms(1 millisecond = 10e-3 second)
		double Time();
		float Timef();


	private:
		#ifdef USE_WINDOWS_TIME_QUERY
		double PC_freq_;
		uint64_t counter_start_;
		#else
		std::chrono::high_resolution_clock::time_point start_;
		#endif
	};

}

#endif

