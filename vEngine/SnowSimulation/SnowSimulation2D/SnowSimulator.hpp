//
//  SnowSimulator.hpp
//  AppFrame
//
//  Created by Yuan Li on 2015/10/04.
//  Copyright 2015 Yuan Li. All rights reserved.
//

#ifndef SNOW_SIMULATOR_HPP
#define SNOW_SIMULATOR_HPP

#pragma once

#include "Common/Header/CommonPreDec.h"
#include "Common/Header/ReturnCode.h"
#include <unordered_map>
#include <list>
#include "Common/Header/Thread.h"
#include "MaterialPointParticle.hpp"
#include "Grid.h"

namespace vEngine
{
	class SnowSimulatorThread: public Thread
	{
	public:
		virtual ReturnCode Main(void* para) override;
	};


	class SnowSimulator
	{
	public:
		SnowSimulator();
		virtual ~SnowSimulator();

		void Start();

		//Do main context init staffs
		ReturnCode Init();
		//Deinit staffs
		ReturnCode Deinit();
		//User update
		ReturnCode Update();

		ReturnCode Reset();

		static const uint32_t	NUMBER_OF_PARTICLES = 500;
		static const float3		GRAVITY_CONSTANT;

	private:

		void RandomToFillCircle(float Raduis, float2 Position);

		void ResterizeParticleMassToGrid();

		//from The Affine Particle-In-Cell (APIC) method
		//http://www.seas.upenn.edu/~cffjiang/research/mpmcourse/mpmcourse.pdf
		void ResterizeParticleToGridWithAPIC();
		void ComputeParticleVelocityWithAPIC();

		void ResterizeParticleToGrid();
		void ComputeParticleVolumesAndDensities();
		void ComputeGridForce();
		void ComputeGridVelocity();
		void GridCollision();
		void ComputeParticleDeformationGradient();
		void ComputeParticleVelocity();
		void ParticleCollision();
		void ComputeParticlePosition();


		std::array<Physics::MaterialPointParticle, SnowSimulator::NUMBER_OF_PARTICLES> particle_pool_;

		Physics::Grid eulerian_grid_;
		
		SnowSimulatorThread simulation_thread_;
	};
}


#endif /* SnowSimulator_hpp */
