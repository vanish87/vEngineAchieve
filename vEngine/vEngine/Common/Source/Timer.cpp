#include "Common\Header\Timer.h"

namespace MocapGE
{
	Timer::Timer(void)
	{
	}


	Timer::~Timer(void)
	{
	}

	void Timer::Retart()
	{
		PC_freq_ = 0.0;
		counter_start_ = 0;

		LARGE_INTEGER freq;
		if(!QueryPerformanceFrequency(&freq))
			PRINT("QueryPerformanceFrequency failed!");

		PC_freq_ = double(freq.QuadPart)/1000.0;
		//in second
		//PC_freq_ = double(freq.QuadPart);

		QueryPerformanceCounter(&freq);
		counter_start_ = freq.QuadPart;
	}

	double Timer::Time()
	{
		LARGE_INTEGER counter;
		QueryPerformanceCounter(&counter);
		return double(counter.QuadPart-counter_start_)/PC_freq_;
	}
	float Timer::Timef()
	{
		return static_cast<float>(this->Time());
	}
}
