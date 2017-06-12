//
//  SandParticle.hpp
//  AppFrame
//
//  Created by Yuan Li on 2015/10/04.
//  Copyright © 2015年 Yuan Li. All rights reserved.
//

#ifndef SandParticle_hpp
#define SandParticle_hpp


#include "Common\Header\CommonPreDec.h"
#include "Common\Header\ReturnCode.h"

#include "Engine\Header\Physics\Particle.hpp"
#include "Engine\Header\Physics\ConeRestriction.hpp"

namespace vEngine
{
	class SandParticle : public Physics::Particle
	{
		friend class SandSimulator;
	public:
		SandParticle();
		virtual ~SandParticle();

		void Update();
		virtual void Reset();



	private:
		void ApplyRestrictions();

		float4x4 SandMatrix;


		//sand is not a sphere but a combination of four smaller sphere that on the four
		//points of tetrahedron.
		//This intends to provide enough frictions for sand
		static const uint32_t NumberOfSphere = 4;
		struct Tetrahedron
		{
			float3 Position[SandParticle::NumberOfSphere];
		};

		SandParticle::Tetrahedron TerahedronInstance;

		ConeRestriction RestrictionInstance;

	};
}


#endif /* SandParticle_hpp */
