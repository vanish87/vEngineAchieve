//
//  SandSimulator.cpp
//  AppFrame
//
//  Created by Yuan Li on 2015/10/04.
//  Copyright © 2015年 Yuan Li. All rights reserved.
//

#include "Engine\Header\Physics\SandSimulator.hpp"
#include "Common\Header\Vector.h"
#include "Common\Header\Math.h"
#include "Common\Header\ReturnCode.h"

namespace vEngine
{
	using namespace Physics;

	const float3	SandSimulator::GRAVITY_CONSTANT = float3(0, -9.8f, 0);
	const float3	SandSimulator::FRICTION_CONSTANT = float3(0, 0.01f, 0);
	const uint32_t	SandSimulator::VOXEL_CELL_SIZE = 10;

	float SandSimulator::Alpha = 0.5;
	float SandSimulator::Beta = 1.5;
	float SandSimulator::NormalRestitution = 1;
	
	extern const float MS_PER_UPDATE;
	const float MS_PER_UPDATE = 1 / 60.0f;

	SandSimulator::SandSimulator()
	{

	};
	SandSimulator::~SandSimulator()
	{

	};
	//Do main context init staffs
	ReturnCode SandSimulator::Init()
	{
		for (std::array<SandParticle, SandSimulator::NUMBER_OF_PARTICLES>::iterator it = this->ParticlePool.begin();
		it != this->ParticlePool.end();
			++it)
		{
			it->Create();
			it->AddToScene();
			it->SetLocation(float3(Math::RandomReal(-10.0f, 10.0f), 63, 100));
			it->SetScale(float3(1, 1, 1));
		}

		return RCSuccess();
	};
	//Deinit staffs
	ReturnCode SandSimulator::Deinit()
	{
		return RCSuccess();
	};

	ReturnCode SandSimulator::Reset()
	{
		for (std::array<SandParticle, SandSimulator::NUMBER_OF_PARTICLES>::iterator it = this->ParticlePool.begin();
		it != this->ParticlePool.end();
			++it)
		{
			it->Reset();
			it->SetLocation(float3(Math::RandomReal(-10.0f, 10.0f), 63, 100));
			it->SetScale(float3(1, 1, 1));
		}
		return RCSuccess();
	}
	ReturnCode SandSimulator::SetConntactParameter(float Mass, float AlphaIn, float BetaIn, float NormalRestitutionIn)
	{
		this->Reset();
		for (std::array<SandParticle, SandSimulator::NUMBER_OF_PARTICLES>::iterator it = this->ParticlePool.begin();
		it != this->ParticlePool.end();
			++it)
		{
			it->SetMass(Mass);
		}
		SandSimulator::Alpha = AlphaIn;
		SandSimulator::Beta = BetaIn;
		SandSimulator::NormalRestitution = NormalRestitutionIn;

		return RCSuccess();
	}
	;

	//User update
	ReturnCode SandSimulator::Update()
	{
		this->UpdateSpatialHash();

		for (std::array<SandParticle, SandSimulator::NUMBER_OF_PARTICLES>::iterator it = this->ParticlePool.begin();
		it != this->ParticlePool.end();
			++it)
		{
			//apply gravity first
			it->ApplyForce(SandSimulator::GRAVITY_CONSTANT*it->GetMass());

			//Update first: calculate velocity and position
			//do some time correction if particles collided this frame
			it->Update();

			const float3 Position = it->GetLocation();
			int3 HashId;
			HashId.x() = (int32_t)Position.x() / SandSimulator::VOXEL_CELL_SIZE;// = int3((int32_t)Position.x(), (int32_t)Position.y(), (int32_t)Position.z());// / SandSimulator::VOXEL_CELL_SIZE;
			HashId.y() = (int32_t)Position.y() / SandSimulator::VOXEL_CELL_SIZE; 
			HashId.z() = (int32_t)Position.z() / SandSimulator::VOXEL_CELL_SIZE;

			std::list<SandParticle*> plist = this->SpatialHashInstance.m[HashId.x()].m[HashId.y()].m[HashId.z()];
			//check detection and apply contact force
			this->CheckDection(*it, plist);

			//Apply other restrictions(e.g glass boarder, floor)
			it->ApplyRestrictions();

			/*
			float3 Friction = it->GetVelocity();
			Friction = Friction * -1;
			Math::Normalize(Friction);
			Friction = Friction * SandSimulator::FRICTION_CONSTANT;*/
		}

		return RCSuccess();
	}
	ReturnCode SandSimulator::UpdateSpatialHash()
	{
		this->SpatialHashInstance.m.clear();
		for (std::array<SandParticle, SandSimulator::NUMBER_OF_PARTICLES>::iterator it = this->ParticlePool.begin();
		it != this->ParticlePool.end();
			++it)
		{
			const float3 Position = it->GetLocation();
			int3 HashId;			
			HashId.x() = (int32_t)Position.x() / SandSimulator::VOXEL_CELL_SIZE;// = int3((int32_t)Position.x(), (int32_t)Position.y(), (int32_t)Position.z());// / SandSimulator::VOXEL_CELL_SIZE;
			HashId.y() = (int32_t)Position.y() / SandSimulator::VOXEL_CELL_SIZE;
			HashId.z() = (int32_t)Position.z() / SandSimulator::VOXEL_CELL_SIZE;

			this->SpatialHashInstance.m[HashId.x()].m[HashId.y()].m[HashId.z()].push_back(&*it);
		}
		return RCSuccess();
	};

	ReturnCode SandSimulator::CheckDection(SandParticle& PaticleIn, std::list<SandParticle*>& Cadidates)
	{
		for (std::list<SandParticle*>::iterator it = Cadidates.begin(); it != Cadidates.end(); ++it)
		{
			if (&PaticleIn == *it) continue;
			this->HandleCollisionWith(PaticleIn, **it);
		}
		return RCSuccess();
	}
	ReturnCode SandSimulator::HandleCollisionWith(SandParticle & Target1, SandParticle & Target2)
	{
		//if contacted, apply contact force
		//each sand particle has for sphere that centered on the points of tetrahedron
		float m1 = Target1.GetMass();
		float m2 = Target2.GetMass();

		float3 v1 = Target1.GetVelocity();
		float3 v2 = Target2.GetVelocity();

		for (int i = 0; i < SandParticle::NumberOfSphere; ++i)
		{
			float3 x1 = Target1.GetLocation() + Target1.TerahedronInstance.Position[i];
			float3 x2 = Target2.GetLocation() + Target2.TerahedronInstance.Position[i];

			float3 VectorToX2 = x2 - x1;

			float Distance = Math::Dot(VectorToX2, VectorToX2);
			float RadiusSum = Target1.GetRadius() + Target2.GetRadius();

			if (Distance <= RadiusSum * RadiusSum && Distance > 0)
			{
				float3 Fn = SandSimulator::GetContactForce(x1, x2, m1, m2, v1, v2, Target1.GetRadius(), Target2.GetRadius());
				//fn points to t2
				//Target2.ApplyForce(Fn);
				//Target1.ApplyForce(Fn * -1);
			}
		}
		//apply friction
		//friction are simulated by geometry struct of sand particle.

		/*float3 Friction = Target1.GetVelocity();
		Friction = Friction * -1;
		Math::Normalize(Friction);
		Friction = Friction * SandSimulator::FRICTION_CONSTANT;
		Target1.ApplyForce(Friction);

		Friction = Target2.GetVelocity();
		Friction = Friction * -1;
		Math::Normalize(Friction);
		Friction = Friction * SandSimulator::FRICTION_CONSTANT;
		Target2.ApplyForce(Friction);*/

		return RCSuccess();
	}

	float SandSimulator::GetKd(float MassEff, float TimeContact)
	{
		if (MassEff > 0 && TimeContact > 0)
		{
			float NORMAL_RESTITUTION = SandSimulator::NormalRestitution;
			return 2 * MassEff * -Math::Ln(NORMAL_RESTITUTION) / TimeContact;
		}
		return 0.0f;
	}

	float SandSimulator::GetKr(float MassEff, float TimeContact)
	{
		if (MassEff > 0 && TimeContact > 0)
		{
			float NORMAL_RESTITUTION = SandSimulator::NormalRestitution;
			float lnE = Math::Ln(NORMAL_RESTITUTION);
			return (MassEff / (TimeContact * TimeContact)) *(lnE * lnE + Math::PI * Math::PI);
		}
		return 0.0f;
	}

	//Hertz theory Kd, Kr
	float SandSimulator::GetKd(float3 T1, float3 T2)
	{

		return 0.0f;
	}

	float SandSimulator::GetKr(float3 T1, float3 T2)
	{
		//x is Radius
		float Reff = (T1.x() * T2.x()) / (T1.x() + T2.x());
		//y is Young's moduli
		//z is Poisson ratios
		float Eeff = ((1 - T1.y()*T1.y()) / T1.z()) + ((1 - T2.y()*T2.y()) / T2.z());
		Eeff = 1 / Eeff;
		return 4.0f / 3.0f * Eeff * Math::Sqrt(Reff);
	}

	float3 SandSimulator::GetContactForce(const float3 x1, const float3 x2,
		const float m1, const float m2,
		const float3 v1, const float3 v2,
		const float r1, const float r2)
	{
		//if contacted, apply contact force
		float3 VectorToX2 = x2 - x1;
		float Distance = Math::Dot(VectorToX2, VectorToX2);
		float RadiusSum = r1 + r2;

		float3 CenterVector = x2 - x1;
		float3 Normal = Math::Normalize(CenterVector);
		float Overlap = Math::Max((float)0.0, RadiusSum - Math::Sqrt(Distance));
		float Meff = (m1 * m2) / (m1 + m2);
		//dissipation
		float Kd = SandSimulator::GetKd(Meff, MS_PER_UPDATE);
		//stiffness
		float Kr = SandSimulator::GetKr(Meff, MS_PER_UPDATE);

		float Ov = Math::Dot(v1 - v2, Normal);

		float fn = -Kd * Math::Pow(Overlap, SandSimulator::Alpha) * Ov - Kr * Math::Pow(Overlap, SandSimulator::Beta);
		//point to T2
		float3 Fn = Normal * fn;
		return Fn;
	}

}


