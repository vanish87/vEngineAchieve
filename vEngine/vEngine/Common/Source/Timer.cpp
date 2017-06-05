#include "Common\Header\Timer.h"

namespace vEngine
{
	Timer::Timer(void)
	{
		this->Retart();
	}


	Timer::~Timer(void)
	{
	}

	void Timer::Retart()
	{
		#ifdef USE_WINDOWS_TIME_QUERY
		PC_freq_ = 0.0;
		counter_start_ = 0;

		LARGE_INTEGER freq;
		if(!QueryPerformanceFrequency(&freq))
			PRINT_AND_BREAK("QueryPerformanceFrequency failed!");

		PC_freq_ = double(freq.QuadPart)/1000.0;
		//in second
		//PC_freq_ = double(freq.QuadPart);

		QueryPerformanceCounter(&freq);
		counter_start_ = freq.QuadPart;
		#else
		start_ = std::chrono::high_resolution_clock::now();
		#endif
	}

	double Timer::Time()
	{
		#ifdef USE_WINDOWS_TIME_QUERY
		LARGE_INTEGER counter;
		QueryPerformanceCounter(&counter);
		return double(counter.QuadPart - counter_start_) / PC_freq_;
		#else
		std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
		return static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(end - start_).count());
		#endif
	}
	float Timer::Timef()
	{
		return static_cast<float>(this->Time());
	}
}
