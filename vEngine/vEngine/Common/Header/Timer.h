#ifndef TIMER_H_
#define TIMER_H_

#pragma once
#include <windows.h>

#include "Common\Header\CommonPreDec.h"

namespace MocapGE
{
	class Timer
	{
	public:
		Timer(void);
		~Timer(void);

		void Retart();
		//return time in ms
		double Time();
		float Timef();


	private:
		double PC_freq_;
		uint64_t counter_start_;
	};

}

#endif

