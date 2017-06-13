//
//  Restriction.hpp
//  AppFrame
//
//  Created by Yuan Li on 2015/10/04.
//  Copyright 2015 Yuan Li. All rights reserved.
//

#ifndef Restriction_hpp
#define Restriction_hpp

#include "Common\Header\CommonPreDec.h"
#include "Engine\Header\Physics\Particle.hpp"

namespace vEngine
{
	namespace Physics
	{
		class Restriction
		{
		public:
			Restriction();
			virtual ~Restriction();
			virtual ReturnCode Apply(Physics::Particle& ParticleIn);
		};

	}
}

#endif /* Restriction_hpp */
