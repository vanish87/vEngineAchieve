//
//  Restriction.cpp
//  AppFrame
//
//  Created by Yuan Li on 2015/10/04.
//  Copyright © 2015年 Yuan Li. All rights reserved.
//

#include "Engine\Header\Physics\Restriction.hpp"
#include "Common\Header\ReturnCode.h"

namespace vEngine
{

	using namespace Physics;

	namespace Physics
	{
		Restriction::Restriction()
		{
		};
		Restriction::~Restriction()
		{

		};
		ReturnCode Restriction::Apply(Particle& ParticleIn)
		{
			return RCSuccess();
		};
	}
}
