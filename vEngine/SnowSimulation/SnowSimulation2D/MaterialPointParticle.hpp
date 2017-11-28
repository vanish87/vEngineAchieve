//
//  SandParticle.hpp
//  AppFrame
//
//  Created by Yuan Li on 2015/10/04.
//  Copyright 2015 Yuan Li. All rights reserved.
//

#ifndef SandParticle_hpp
#define SandParticle_hpp


#include "Common/Header/CommonPreDec.h"
#include "Common/Header/ReturnCode.h"

#include "Engine/Header/Physics/Particle.hpp"
#include "Engine/Header/Physics/ConeRestriction.hpp"

namespace vEngine
{
	namespace Physics
	{
		class MaterialPointParticle : public Particle
		{
		public:
			MaterialPointParticle();
			virtual ~MaterialPointParticle();

			void Update() override;
			virtual void Reset() override;


			void ComputeEnergyDensity();

			ConeRestriction restriction_instance_;

			float density_;
			float volume_;

			float2x2 force_;

			float2x2 velocity_gradient_;

			float weight_all_[4][4];
			float weight_gradient_all_[4][4];

			float3 weight_[4][4];
			float3 weight_dev_[4][4];
			float3 weight_gradient_[4][4];

			//APIC matrix
			float2x2 B;
			float2x2 D;

			float2x2 Fe;
			float2x2 Fp;

			float2x2 R;
			float2x2 S;

			float2x2 s;
			float2x2 v;
			float2x2 d;


			//debug
			int3 debug_grid_index_;

			void PrintInfo();

		};

	}
}


#endif /* SandParticle_hpp */
