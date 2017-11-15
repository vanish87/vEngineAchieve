//
//  SnowSimulator.cpp
//  AppFrame
//
//  Created by Yuan Li on 2015/10/04.
//  Copyright  2015 Yuan Li. All rights reserved.
//

#include "SnowSimulator.hpp"
#include "Common/Header/Vector.h"
#include "Common/Header/Math.h"
#include "Common/Header/ReturnCode.h"
#include "Engine/Header/Profiler.h"

namespace vEngine
{
	using namespace Physics;

	const float3	SnowSimulator::GRAVITY_CONSTANT = float3(0, -9.8f, 0);
	
	static Profiler SimulatorProfiler("SimulatorProfiler");

	SnowSimulator::SnowSimulator()
	{
	};
	SnowSimulator::~SnowSimulator()
	{

	};

	void SnowSimulator::Start()
	{
		SimulationThread.Create(this);
	}

	void SnowSimulator::RandomToFillCircle(float Raduis)
	{
		for (Particle& it : this->ParticlePool)
		{
			//SandParticle& it = this->ParticlePool[i];
			it.Create();
			it.AddToScene();
			float3 rand = float3(Math::RandomReal(-50.0f, 50.0f), Math::RandomReal(-50.0f, 50.0f), 0);
			while (Math::Dot(rand, rand) > Raduis * Raduis)
			{
				rand = float3(Math::RandomReal(-50.0f, 50.0f), Math::RandomReal(-50.0f, 50.0f), 0);
				it.SetLocation(rand);
			}
			rand.y() += 200;
			it.SetLocation(rand);
			it.SetScale(float3(1, 1, 1));
			it.SetVisiable(true);
		}
	}

	//Do main context init stuffs
	ReturnCode SnowSimulator::Init()
	{		
		RandomToFillCircle(8);
		return RCSuccess();
	};
	//Deinit stuffs
	ReturnCode SnowSimulator::Deinit()
	{
		return RCSuccess();
	};

	ReturnCode SnowSimulator::Reset()
	{
		RandomToFillCircle(8);
		return RCSuccess();
	}

	//User update
	ReturnCode SnowSimulator::Update()
	{

		SimulatorProfiler.Begin(Profiler::PE_FUNCTION_CALL);
		for (Particle& it : this->ParticlePool)
		{
			//SandParticle& it = this->ParticlePool[i];
			//apply gravity first
			it.ApplyForce(SnowSimulator::GRAVITY_CONSTANT * it.GetMass());

			//Update first: calculate velocity and position
			//do some time correction if particles collided this frame
			it.Update();
		}
		SimulatorProfiler.End(Profiler::PE_FUNCTION_CALL, "SnowSimulator Update Particles");

		return RCSuccess();
	}
			
	ReturnCode SnowSimulatorThread::Main(void* para)
	{
		SimulatorProfiler.SetEnable(true);
		SimulatorProfiler.RegisterEventHandler(&LogHanlder);

		SnowSimulator* sim = static_cast<SnowSimulator*>(para);
		sim->Init();
		while (true)
		{
			sim->Update();
		}
		sim->Deinit();
	}

}


