//
//  SandSimulator.cpp
//  AppFrame
//
//  Created by Yuan Li on 2015/10/04.
//  Copyright  2015 Yuan Li. All rights reserved.
//

#include "Engine/Header/Physics/SandSimulator.hpp"
#include "Common/Header/Vector.h"
#include "Common/Header/Math.h"
#include "Common/Header/ReturnCode.h"
#include "Engine/Header/Profiler.h"

namespace vEngine
{
	using namespace Physics;

	const float3	SandSimulator::GRAVITY_CONSTANT = float3(0, -9.8f, 0);
	const float3	SandSimulator::FRICTION_CONSTANT = float3(0, 0.01f, 0);
	const uint32_t	SandSimulator::VOXEL_CELL_SIZE = 5;

	float SandSimulator::kAlpha = 0.5;
	float SandSimulator::kBeta = 1.5;
	float SandSimulator::kNormalRestitution = 1;
	
	extern const float MS_PER_UPDATE;
	//const float MS_PER_UPDATE = 1 / 60.0f;

	static Profiler SimulatorProfiler("SimulatorProfiler");

	SandSimulator::SandSimulator()
	{
	};
	SandSimulator::~SandSimulator()
	{

	};

	void SandSimulator::Start()
	{
		SimulationThread.Create(this);
	}

	//Do main context init stuffs
	ReturnCode SandSimulator::Init()
	{
		for (SandParticle& it : this->ParticlePool)
		{
			//SandParticle& it = this->ParticlePool[i];
			it.Create();
			it.AddToScene();
			it.SetLocation(float3(Math::RandomReal(-50.0f, 50.0f), 63, 100));
			it.SetScale(float3(1, 1, 1));
		}

		return RCSuccess();
	};
	//Deinit stuffs
	ReturnCode SandSimulator::Deinit()
	{
		return RCSuccess();
	};

	ReturnCode SandSimulator::Reset()
	{
		for (SandParticle& it : this->ParticlePool)
		{
			it.Reset();
			it.SetLocation(float3(Math::RandomReal(-50.0f, 50.0f), 63, 100));
			it.SetScale(float3(1, 1, 1));
		}
		return RCSuccess();
	}
	ReturnCode SandSimulator::SetConntactParameter(float mass, float alpha, float beta, float normal_restitution)
	{
		this->Reset();
		for (SandParticle& it : this->ParticlePool)
		{
			//SandParticle& it = this->ParticlePool[i];
			it.SetMass(mass);
		}
		SandSimulator::kAlpha = alpha;
		SandSimulator::kBeta = beta;
		SandSimulator::kNormalRestitution = normal_restitution;

		return RCSuccess();
	}
	;

	//User update
	ReturnCode SandSimulator::Update()
	{
		SimulatorProfiler.Begin(Profiler::PE_FUNCTION_CALL);
		this->UpdateSpatialHash();
		SimulatorProfiler.End(Profiler::PE_FUNCTION_CALL, "SandSimulator UpdateSpatialHash");


		SimulatorProfiler.Begin(Profiler::PE_FUNCTION_CALL);
		for (SandParticle& it : this->ParticlePool)
		{
			//SandParticle& it = this->ParticlePool[i];
			//apply gravity first
			it.ApplyForce(SandSimulator::GRAVITY_CONSTANT * it.GetMass());

			//Update first: calculate velocity and position
			//do some time correction if particles collided this frame
			it.Update();

			const float3 Position = it.GetLocation();
			int3 HashId;
			HashId.x() = (int32_t)Position.x() / SandSimulator::VOXEL_CELL_SIZE;// = int3((int32_t)Position.x(), (int32_t)Position.y(), (int32_t)Position.z());// / SandSimulator::VOXEL_CELL_SIZE;
			HashId.y() = (int32_t)Position.y() / SandSimulator::VOXEL_CELL_SIZE; 
			HashId.z() = (int32_t)Position.z() / SandSimulator::VOXEL_CELL_SIZE;

			std::list<SandParticle*> plist = this->SpatialHashInstance.m[HashId.x()].m[HashId.y()].m[HashId.z()];
			//check detection and apply contact force
			this->CheckDection(it, plist);

			//Apply other restrictions(e.g glass boarder, floor)
			it.ApplyRestrictions();

			/*
			float3 Friction = it->GetVelocity();
			Friction = Friction * -1;
			Math::Normalize(Friction);
			Friction = Friction * SandSimulator::FRICTION_CONSTANT;*/
		}
		SimulatorProfiler.End(Profiler::PE_FUNCTION_CALL, "SandSimulator Update Particles");

		return RCSuccess();
	}
	ReturnCode SandSimulator::UpdateSpatialHash()
	{
		this->SpatialHashInstance.m.clear();
		for (SandParticle& it : this->ParticlePool)
		{
			//SandParticle& it = this->ParticlePool[i];
			const float3 Position = it.GetLocation();
			int3 HashId;			
			HashId.x() = (int32_t)Position.x() / SandSimulator::VOXEL_CELL_SIZE;// = int3((int32_t)Position.x(), (int32_t)Position.y(), (int32_t)Position.z());// / SandSimulator::VOXEL_CELL_SIZE;
			HashId.y() = (int32_t)Position.y() / SandSimulator::VOXEL_CELL_SIZE;
			HashId.z() = (int32_t)Position.z() / SandSimulator::VOXEL_CELL_SIZE;

			this->SpatialHashInstance.m[HashId.x()].m[HashId.y()].m[HashId.z()].push_back(&it);
		}
		return RCSuccess();
	};

	ReturnCode SandSimulator::CheckDection(SandParticle& paticle, std::list<SandParticle*>& cadidates)
	{
		for (auto it: cadidates)
		{
			if (&paticle == it) continue;
			this->HandleCollisionWith(paticle, *it);
		}
		return RCSuccess();
	}
	ReturnCode SandSimulator::HandleCollisionWith(SandParticle & target1, SandParticle & target2)
	{
		//if contacted, apply contact force
		//each sand particle has for sphere that centered on the points of tetrahedron
		float m1 = target1.GetMass();
		float m2 = target2.GetMass();

		float3 v1 = target1.GetVelocity();
		float3 v2 = target2.GetVelocity();

		for (int i = 0; i < SandParticle::kNumberOfSphere; ++i)
		{
			float3 x1 = target1.GetLocation() + target1.terahedron_instance_.position[i];
			float3 x2 = target2.GetLocation() + target2.terahedron_instance_.position[i];

			float3 VectorToX2 = x2 - x1;

			float Distance = Math::Dot(VectorToX2, VectorToX2);
			float RadiusSum = target1.GetRadius() + target2.GetRadius();

			if (Distance <= RadiusSum * RadiusSum && Distance > 0)
			{
				float3 Fn = SandSimulator::GetContactForce(x1, x2, m1, m2, v1, v2, target1.GetRadius(), target2.GetRadius());
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

	float SandSimulator::GetKd(float mass_eff, float time_contact)
	{
		if (mass_eff > 0 && time_contact > 0)
		{
			float NORMAL_RESTITUTION = SandSimulator::kNormalRestitution;
			return 2 * mass_eff * -Math::Ln(NORMAL_RESTITUTION) / time_contact;
		}
		return 0.0f;
	}

	float SandSimulator::GetKr(float mass_eff, float time_contact)
	{
		if (mass_eff > 0 && time_contact > 0)
		{
			float NORMAL_RESTITUTION = SandSimulator::kNormalRestitution;
			float lnE = Math::Ln(NORMAL_RESTITUTION);
			return (mass_eff / (time_contact * time_contact)) *(lnE * lnE + Math::PI * Math::PI);
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
		const float  m1 , const float  m2,
		const float3 v1 , const float3 v2,
		const float  r1 , const float  r2)
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

		float fn = -Kd * Math::Pow(Overlap, SandSimulator::kAlpha) * Ov - Kr * Math::Pow(Overlap, SandSimulator::kBeta);
		//point to T2
		float3 Fn = Normal * fn;
		return Fn;
	}

	ReturnCode SimulatorThread::Main(void* para)
	{
		SimulatorProfiler.SetEnable(true);
		SimulatorProfiler.RegisterEventHandler(&LogHanlder);

		SandSimulator* sim = static_cast<SandSimulator*>(para);
		sim->Init();
		while (true)
		{
			sim->Update();
		}
		sim->Deinit();
	}

}


