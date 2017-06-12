//
//  SandParticle.cpp
//  AppFrame
//
//  Created by Yuan Li on 2015/10/04.
//  Copyright © 2015年 Yuan Li. All rights reserved.
//

#include "Engine\Header\Physics\SandParticle.hpp"
#include "Common\Header\Vector.h"
#include "Common\Header\ReturnCode.h"
#include "Common\Header\Math.h"

namespace vEngine
{
	SandParticle::SandParticle()
		:Particle()
	{
		Math::Identity(this->SandMatrix);
		this->TerahedronInstance.Position[0] = float3(1, 0, -1 / Math::Sqrt(2));
		this->TerahedronInstance.Position[1] = float3(-1, 0, -1 / Math::Sqrt(2));
		this->TerahedronInstance.Position[2] = float3(0, 1, 1 / Math::Sqrt(2));
		this->TerahedronInstance.Position[3] = float3(0, -1, 1 / Math::Sqrt(2));

		this->CurrentFrameInfo.Radius = Math::Sqrt(Math::Dot(this->TerahedronInstance.Position[0], this->TerahedronInstance.Position[0]));
	};
	SandParticle::~SandParticle()
	{

	};

	void SandParticle::Update()
	{
		Particle::Update();
	}

	void SandParticle::Reset()
	{
		Particle::Reset();
		Math::Identity(this->SandMatrix);
		this->TerahedronInstance.Position[0] = float3(1, 0, -1 / Math::Sqrt(2));
		this->TerahedronInstance.Position[1] = float3(-1, 0, -1 / Math::Sqrt(2));
		this->TerahedronInstance.Position[2] = float3(0, 1, 1 / Math::Sqrt(2));
		this->TerahedronInstance.Position[3] = float3(0, -1, 1 / Math::Sqrt(2));

		this->CurrentFrameInfo.Radius = Math::Sqrt(Math::Dot(this->TerahedronInstance.Position[0], this->TerahedronInstance.Position[0]));
	}

	void SandParticle::ApplyRestrictions()
	{
		this->RestrictionInstance.Apply(*this);
	}

}

