//
//  SandParticle.cpp
//  AppFrame
//
//  Created by Yuan Li on 2015/10/04.
//  Copyright 2015 Yuan Li. All rights reserved.
//

#include "Engine/Header/Physics/SandParticle.hpp"
#include "Common/Header/Vector.h"
#include "Common/Header/ReturnCode.h"
#include "Common/Header/Math.h"

namespace vEngine
{
	SandParticle::SandParticle()
		:Particle()
	{
		Math::Identity(this->sand_matrix_);
		this->terahedron_instance_.position[0] = float3(1, 0, -1 / Math::Sqrt(2));
		this->terahedron_instance_.position[1] = float3(-1, 0, -1 / Math::Sqrt(2));
		this->terahedron_instance_.position[2] = float3(0, 1, 1 / Math::Sqrt(2));
		this->terahedron_instance_.position[3] = float3(0, -1, 1 / Math::Sqrt(2));

		this->current_frame_info_.radius = Math::Sqrt(Math::Dot(this->terahedron_instance_.position[0], this->terahedron_instance_.position[0]));
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
		Math::Identity(this->sand_matrix_);
		this->terahedron_instance_.position[0] = float3(1, 0, -1 / Math::Sqrt(2));
		this->terahedron_instance_.position[1] = float3(-1, 0, -1 / Math::Sqrt(2));
		this->terahedron_instance_.position[2] = float3(0, 1, 1 / Math::Sqrt(2));
		this->terahedron_instance_.position[3] = float3(0, -1, 1 / Math::Sqrt(2));

		this->current_frame_info_.radius = Math::Sqrt(Math::Dot(this->terahedron_instance_.position[0], this->terahedron_instance_.position[0]));
	}

	void SandParticle::ApplyRestrictions()
	{
		this->restriction_instance_.Apply(*this);
	}

}

