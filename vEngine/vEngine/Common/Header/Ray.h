#ifndef RAY_H_
#define RAY_H_

#pragma once
#include "Common/Header/CommonPreDec.h"
#include "Common/Header/Vector.h"

namespace vEngine
{
	class Ray
	{
	public:
		Ray(void);
		~Ray(void){};
		Ray(float3 orig, float3 dir)
			:origin_(orig), direction_(dir)
		{
		}

		float3 Origin(){return origin_;};
		float3 Direction(){return direction_;};

		void SetOrigin(float3 origin){origin_ = origin;};
		void SetDirection(float3 direction){direction_ = direction;};

	private:
		float3 origin_;
		float3 direction_;
	};

}

#endif

