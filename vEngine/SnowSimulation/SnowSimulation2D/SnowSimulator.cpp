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
	extern const float MS_PER_UPDATE;

	const static float ALPHA = 0.95f;
	
	static Profiler SimulatorProfiler("SimulatorProfiler");

	SnowSimulator::SnowSimulator()
	{
	};
	SnowSimulator::~SnowSimulator()
	{

	};

	void SnowSimulator::Start()
	{
		simulation_thread_.Create(this);
	}

	void SnowSimulator::RandomToFillCircle(float Raduis)
	{
		for (Particle& it : this->particle_pool_)
		{
			//SandParticle& it = this->ParticlePool[i];
			it.Create();
			it.AddToScene();
			float3 rand = float3(Math::RandomReal(-Raduis, Raduis), Math::RandomReal(-Raduis, Raduis), 0);
			while (Math::Dot(rand, rand) > Raduis * Raduis)
			{
				rand = float3(Math::RandomReal(-Raduis, Raduis), Math::RandomReal(-Raduis, Raduis), 0);
				it.SetLocation(rand);
			}
			rand.y() += 100;
			it.SetLocation(rand);
			it.SetVelocity(float3(0, -10, 0));

			it.SetMass(0.0005);

			it.SetScale(float3(1, 1, 1));
			it.SetVisiable(true);
		}
	}

	void SnowSimulator::ResterizeParticleToGrid()
	{
		//to map particles mass and velocities to Grid with weight kernel
		PRINT("Cell Size is "<< Grid::VOXEL_CELL_SIZE);

		this->eulerian_grid_.Reset();

		//map mass first
		for (MaterialPointParticle& it : this->particle_pool_)
		{
			const float3 Position = it.GetLocation();

			PRINT("From paticle " << Position.x() << " " << Position.y());
			
			int3 GridIndex = this->eulerian_grid_.GetGridIndexFromParticlePosition(Position);
			float3 ParticleGridPosition = this->eulerian_grid_.GetGridPositionFromParticlePosition(Position);

			it.debug_grid_index_ = GridIndex;

			PRINT("Get Index " << GridIndex.x() << " " << GridIndex.y());
			PRINT("Get GridPos " << ParticleGridPosition.x() << " " << ParticleGridPosition.y());

			float3 Nx = ParticleGridPosition - GridIndex;
			PRINT("Get Nx =  " << Nx.x() << " " << Nx.y());
			it.weight_ = Math::GetBSpline(Nx);
			it.weight_.z() = 1;
			
			Cell& cell = this->eulerian_grid_.GetCell(GridIndex);
			cell.mass_ += it.GetMass() * it.weight_.x() * it.weight_.y() * it.weight_.z();
			CHECK_ASSERT(cell.mass_ > 0);
			PRINT("GridIndex =  " << GridIndex.x() << " " << GridIndex.y());
			//cell.PrintInfo();
		}

		for (MaterialPointParticle& it : this->particle_pool_)
		{
			const float3 Position = it.GetLocation();

			int3 GridIndex = this->eulerian_grid_.GetGridIndexFromParticlePosition(Position);
			float3 ParticleGridPosition = this->eulerian_grid_.GetGridPositionFromParticlePosition(Position);

			
			Cell& cell = this->eulerian_grid_.GetCell(GridIndex);

			CHECK_ASSERT(cell.mass_ > 0)
			cell.velocity_ = cell.velocity_ + (it.GetVelocity() * it.GetMass() * it.weight_ / cell.mass_);

		}

		PRINT_WARNING("begin of ResterizeParticleToGrid-----------------------------------------------");
		this->eulerian_grid_.PrintInfo();
		PRINT_WARNING("end of ResterizeParticleToGrid-----------------------------------------------");
		/*
		for (auto& x : this->eulerian_grid_.grid_data_)
		{
			for (auto& y : x)
			{
				for (auto& z : y)
				{

				}
			}
		}*/
	}

	void SnowSimulator::ComputeParticleVolumesAndDensities()
	{

		for (MaterialPointParticle& it : this->particle_pool_)
		{
			const float3 Position = it.GetLocation();

			int3 GridIndex = this->eulerian_grid_.GetGridIndexFromParticlePosition(Position);
			float3 ParticleGridPosition = this->eulerian_grid_.GetGridPositionFromParticlePosition(Position);

			Cell& cell = this->eulerian_grid_.GetCell(GridIndex);

			//TODO Check here add z value and cell size for 3D later
			it.density_ = (cell.mass_ * it.weight_.x() + cell.mass_ * it.weight_.y())/(Grid::VOXEL_CELL_SIZE * Grid::VOXEL_CELL_SIZE);
			CHECK_ASSERT(it.density_ > 0);
			it.volume_ = it.GetMass() / it.density_;

			it.PrintInfo();
		}

		PRINT_WARNING("begin of ComputeParticleVolumesAndDensities-----------------------------------------------");
		this->eulerian_grid_.PrintInfo();
		PRINT_WARNING("end of ComputeParticleVolumesAndDensities-----------------------------------------------");
	}

	void SnowSimulator::ComputeGridForce()
	{
		for (MaterialPointParticle& it : this->particle_pool_)
		{
			it.ComputeEnergyDensity();

			const float3 Position = it.GetLocation();

			int3 GridIndex = this->eulerian_grid_.GetGridIndexFromParticlePosition(Position);
			float3 ParticleGridPosition = this->eulerian_grid_.GetGridPositionFromParticlePosition(Position);

			it.weight_dev_ = Math::GetBSplineDerivative(ParticleGridPosition - GridIndex);
			 

			Cell& cell = this->eulerian_grid_.GetCell(GridIndex);


			float3 newf = float3(it.force_[0][0] * it.weight_dev_[0] + it.force_[0][1] * it.weight_dev_[1],
								 it.force_[1][0] * it.weight_dev_[0] + it.force_[1][1] * it.weight_dev_[1],
								 0);

			cell.force_ = cell.force_ + newf;
		}
	}



	void SnowSimulator::ComputeGridVelocity()
	{
		for (auto& x : this->eulerian_grid_.grid_data_)
		{
			for (auto& y : x)
			{
				Cell& c = y[32];
				if(c.mass_ > 0)
				{
					c.velocity_new_ = c.velocity_ + (GRAVITY_CONSTANT + c.force_ / c.mass_) * MS_PER_UPDATE;
					c.PrintInfo();
				}
			}
		}
	}

	void SnowSimulator::ComputeParticleVelocity()
	{
		for (MaterialPointParticle& it : this->particle_pool_)
		{
			const float3 Position = it.GetLocation();

			int3 GridIndex = this->eulerian_grid_.GetGridIndexFromParticlePosition(Position);
			float3 ParticleGridPosition = this->eulerian_grid_.GetGridPositionFromParticlePosition(Position);
						
			Cell& cell = this->eulerian_grid_.GetCell(GridIndex);

			float3 Vpic = cell.velocity_new_ * it.weight_;
			float3 Vflip = it.GetVelocity() + (cell.velocity_new_ - cell.velocity_) * it.weight_;

			it.SetVelocity(Vpic * (1 - ALPHA) + Vflip * ALPHA);
			it.PrintInfo();
		}
	}

	//Do main context init stuffs
	ReturnCode SnowSimulator::Init()
	{		
		//this->eulerian_grid_.VisualizeCells();
		RandomToFillCircle(20);
		this->ResterizeParticleToGrid();
		this->ComputeParticleVolumesAndDensities();
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
		this->ResterizeParticleToGrid();
		SimulatorProfiler.End(Profiler::PE_FUNCTION_CALL, "SnowSimulator Update ResterizeParticleToGrid");

		SimulatorProfiler.Begin(Profiler::PE_FUNCTION_CALL);
		this->ComputeGridForce();
		SimulatorProfiler.End(Profiler::PE_FUNCTION_CALL, "SnowSimulator Update ComputeGridForce");

		SimulatorProfiler.Begin(Profiler::PE_FUNCTION_CALL);
		this->ComputeGridVelocity();
		SimulatorProfiler.End(Profiler::PE_FUNCTION_CALL, "SnowSimulator Update ComputeGridVelocity");

		SimulatorProfiler.Begin(Profiler::PE_FUNCTION_CALL);
		this->ComputeParticleVelocity();
		SimulatorProfiler.End(Profiler::PE_FUNCTION_CALL, "SnowSimulator Update ComputeParticleVelocity");

		SimulatorProfiler.Begin(Profiler::PE_FUNCTION_CALL);
		for (Particle& it : this->particle_pool_)
		{
			//SandParticle& it = this->ParticlePool[i];
			//apply gravity first
			//it.ApplyForce(SnowSimulator::GRAVITY_CONSTANT * it.GetMass());

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


