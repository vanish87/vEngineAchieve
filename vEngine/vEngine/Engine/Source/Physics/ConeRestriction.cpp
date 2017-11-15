//
//  ConeRestriction.cpp
//  AppFrame
//
//  Created by Yuan Li on 2015/10/04.
//  Copyright 2015 Yuan Li. All rights reserved.
//

#include "Engine/Header/Physics/ConeRestriction.hpp"
#include "Engine/Header/Physics/SandSimulator.hpp"
#include "Common/Header/ReturnCode.h"
#include "Common/Header/Math.h"

namespace vEngine
{
	using namespace Physics;

	ConeRestriction::ConeRestriction()
	{
	};
	ConeRestriction::~ConeRestriction()
	{

	};

	//http://mathworld.wolfram.com/Cone.html
	//http://www.wolframalpha.com/input/?i=cone

	ReturnCode ConeRestriction::Apply(Particle& ParticleIn)
	{
		float3 Position = ParticleIn.GetLocation();
		Position = Position - Position0;
		float k = Math::Sqrt(Position.x()*Position.x() + Position.z()*Position.z()) * h / r;
		float cone_y = k + Position0.y();
		float3 vel = ParticleIn.GetVelocity();
		if (-cone_y >= ParticleIn.GetLocation().y())
		{
			float3 x1 = ParticleIn.GetLocation();
			float3 x2 = float3(x1.x(), -cone_y, x1.z());
			float  m1 = ParticleIn.GetMass();
			float  m2 = this->Mass;
			float3 v1 = ParticleIn.GetVelocity();
			float3 v2 = float3(0, 0, 0);
			//float3 Fn = SandSimulator::GetContactForce(x1, x2, m1, m2, v1, v2, 1.414f, 0.1f);
			//points to T2
			//ParticleIn.ApplyForce(Fn*-1);       

			vel.y() = vel.y() * -1;
			ParticleIn.SetLocation(x2);
		}

		ParticleIn.SetVelocity(vel);
		return RCSuccess();
	};
}

