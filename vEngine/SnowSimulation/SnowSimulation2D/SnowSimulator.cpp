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

#include "SnowConstants.h"

namespace vEngine
{
	using namespace Physics;


	const float3	SnowSimulator::GRAVITY_CONSTANT = float3(0, -9.8f, 0);

	const float BSPLINE_EPSILON = 1e-4;

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

	void SnowSimulator::RandomToFillCircle(float Raduis, float2 Position)
	{
		for (uint32_t i = 0; i < this->particle_pool_.size(); ++i)
		{
			if (i < 200)
			{
				Particle& it = this->particle_pool_[i];
				it.Create();
				float3 rand = float3(Math::RandomReal(-Raduis, Raduis), Math::RandomReal(-Raduis, Raduis), 0);
				while (Math::Dot(rand, rand) > Raduis * Raduis)
				{
					rand = float3(Math::RandomReal(-Raduis, Raduis), Math::RandomReal(-Raduis, Raduis), 0);
					it.SetLocation(rand);
				}
				rand.x() += Position.x();
				rand.y() += Position.y();
				it.SetLocation(rand);
				it.SetVelocity(float3(90, -150, 0));

				it.SetMass(0.0005);

				it.SetScale(float3(1, 1, 1));
				it.SetVisiable(true);
				it.SetVMainThreadUpdate(false);

				it.AddToScene();

			}
			else
			{
				Particle& it = this->particle_pool_[i];
				it.Create();
				float3 rand = float3(Math::RandomReal(-Raduis, Raduis), Math::RandomReal(-Raduis, Raduis), 0);
				while (Math::Dot(rand, rand) > Raduis * Raduis)
				{
					rand = float3(Math::RandomReal(-Raduis, Raduis), Math::RandomReal(-Raduis, Raduis), 0);
					it.SetLocation(rand);
				}
				rand.x() += 60;
				rand.y() += 50;
				it.SetLocation(rand);
				it.SetVelocity(float3(0, 0, 0));

				it.SetMass(0.0005);

				it.SetScale(float3(1, 1, 1));
				it.SetVisiable(true);
				it.SetVMainThreadUpdate(false);

				it.AddToScene();
			}

			
		}
	}

	void SnowSimulator::ResterizeParticleToGrid()
	{
		//to map particles mass and velocities to Grid with weight kernel
		//PRINT("Cell Size is "<< Grid::VOXEL_CELL_SIZE);

		this->eulerian_grid_.Reset();

		//map mass first
		for (MaterialPointParticle& it : this->particle_pool_)
		{
			const float3 Position = it.GetLocation();

			//PRINT("From paticle " << Position.x() << " " << Position.y());
			
			int3 GridIndex = this->eulerian_grid_.GetGridIndexFromParticlePosition(Position);
			float3 ParticleGridPosition = this->eulerian_grid_.GetGridPositionFromParticlePosition(Position);

			it.debug_grid_index_ = GridIndex;

			//PRINT("Get Index " << GridIndex.x() << " " << GridIndex.y());
			//PRINT("Get GridPos " << ParticleGridPosition.x() << " " << ParticleGridPosition.y());

			for (int i = -1; i < 3; ++i)
			{
				for (int j = -1; j < 3; ++j)
				{
					int3 CurrentIndex = int3(GridIndex.x() + i, GridIndex.y() + j, GridIndex.z());
					float3 Nx = ParticleGridPosition - CurrentIndex;

					//PRINT("Index "<< CurrentIndex.x()<<" "<< CurrentIndex.y()<<" Get Nx =  " << Nx.x() << " " << Nx.y());
					it.weight_[i+1][j+1] = Math::GetBSpline(Nx);
					//TODO 
					//here we remove z for 2D
					it.weight_[i + 1][j + 1].z() = 1;


					//PRINT_VAR(it.weight_[i + 1][j + 1]);// .z() = 1;

					Cell& cell = this->eulerian_grid_.GetCell(CurrentIndex);
					cell.mass_ += it.GetMass() * it.weight_[i+1][j+1].x() * it.weight_[i + 1][j + 1].y();
					//CHECK_ASSERT(cell.mass_ > 0);
				}
			}

			//cell.PrintInfo();
		}

		for (MaterialPointParticle& it : this->particle_pool_)
		{
			const float3 Position = it.GetLocation();

			int3 GridIndex = this->eulerian_grid_.GetGridIndexFromParticlePosition(Position);
			float3 ParticleGridPosition = this->eulerian_grid_.GetGridPositionFromParticlePosition(Position);

			
			for (int i = -1; i < 3; ++i)
			{
				for (int j = -1; j < 3; ++j)
				{
					float weight = it.weight_[i + 1][j + 1].x() * it.weight_[i + 1][j + 1].y();
					if (weight > BSPLINE_EPSILON)
					{
						int3 CurrentIndex = int3(GridIndex.x() + i, GridIndex.y() + j, GridIndex.z());
						Cell& cell = this->eulerian_grid_.GetCell(CurrentIndex);
						cell.velocity_ = cell.velocity_ + (it.GetVelocity() * it.GetMass() * weight);
						cell.is_active_ = true;

						CHECK_ASSERT(cell.mass_ > 0);
					}
				}
			}

		}


		for (auto& x : this->eulerian_grid_.grid_data_)
		{
			for (auto& y : x)
			{
				for (auto& z : y)
				{
					if (z.is_active_)
					{
						z.velocity_ = z.velocity_ / z.mass_;
						//z.PrintInfo();
					}
				}
			}
		}
//  
//   		PRINT_WARNING("begin of ResterizeParticleToGrid-----------------------------------------------");
//   		this->eulerian_grid_.PrintInfo();
//   		PRINT_WARNING("end of ResterizeParticleToGrid-----------------------------------------------");
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


			//TODO Check here add z value and cell size for 3D later
			float CellArea = 1.0f / (Grid::VOXEL_CELL_SIZE * Grid::VOXEL_GRID_SIZE);
			CellArea *= CellArea;


			it.density_ = 0;
			for (int i = -1; i < 3; ++i)
			{
				for (int j = -1; j < 3; ++j)
				{
					int3 CurrentIndex = int3(GridIndex.x() + i, GridIndex.y() + j, GridIndex.z());
					Cell& cell = this->eulerian_grid_.GetCell(CurrentIndex);

					float weight = it.weight_[i + 1][j + 1].x() * it.weight_[i + 1][j + 1].y();
					if (weight > BSPLINE_EPSILON)
					{
						it.density_ += (cell.mass_ *  weight);
					}
				}
			}
			it.density_ /= CellArea;
			CHECK_ASSERT(it.density_ > 0);
			it.volume_ = it.GetMass() / it.density_;
			//it.PrintInfo();
		}

// 		PRINT_WARNING("begin of ComputeParticleVolumesAndDensities-----------------------------------------------");
// 		this->eulerian_grid_.PrintInfo();
// 		PRINT_WARNING("end of ComputeParticleVolumesAndDensities-----------------------------------------------");
	}

	void SnowSimulator::ComputeGridForce()
	{
		for (MaterialPointParticle& it : this->particle_pool_)
		{
			it.ComputeEnergyDensity();

			const float3 Position = it.GetLocation();

			int3 GridIndex = this->eulerian_grid_.GetGridIndexFromParticlePosition(Position);
			float3 ParticleGridPosition = this->eulerian_grid_.GetGridPositionFromParticlePosition(Position);
			
			for (int i = -1; i < 3; ++i)
			{
				for (int j = -1; j < 3; ++j)
				{
					int3 CurrentIndex = int3(GridIndex.x() + i, GridIndex.y() + j, GridIndex.z());
					float3 Nx = ParticleGridPosition - CurrentIndex;

					it.weight_dev_[i + 1][j + 1] = Math::GetBSplineDerivative(Nx);
					it.weight_gradient_[i + 1][j + 1].x() = it.weight_dev_[i + 1][j + 1].x() * it.weight_[i + 1][j + 1].y();
					it.weight_gradient_[i + 1][j + 1].y() = it.weight_[i + 1][j + 1].x() * it.weight_dev_[i + 1][j + 1].y();
					it.weight_gradient_[i + 1][j + 1].z() = 1;

					//it.weight_gradient_[i + 1][j + 1] = it.weight_gradient_[i + 1][j + 1] / Grid::VOXEL_CELL_SIZE;

					Cell& cell = this->eulerian_grid_.GetCell(CurrentIndex);

					float weight = it.weight_[i + 1][j + 1].x() * it.weight_[i + 1][j + 1].y();
					if (weight > BSPLINE_EPSILON)
					{
						float3 newf = float3(it.force_[0][0] * it.weight_gradient_[i + 1][j + 1][0] + it.force_[1][0] * it.weight_gradient_[i + 1][j + 1][1],
							it.force_[0][1] * it.weight_gradient_[i + 1][j + 1][0] + it.force_[1][1] * it.weight_gradient_[i + 1][j + 1][1],
							0);
						CHECK_ASSERT(Math::IsNAN(newf.x()) == false);
						CHECK_ASSERT(Math::IsNAN(newf.y()) == false);
						CHECK_ASSERT(Math::IsNAN(newf.z()) == false);

						cell.force_ = cell.force_ + newf;
					}
				}
			}
		}
//  		PRINT_WARNING("begin of ComputeGridForce-----------------------------------------------");
//  		this->eulerian_grid_.PrintInfo();
//  		PRINT_WARNING("end of ComputeGridForce-----------------------------------------------");
	}



	void SnowSimulator::ComputeGridVelocity()
	{
		for (auto& x : this->eulerian_grid_.grid_data_)
		{
			for (auto& y : x)
			{
				for (auto&c : y)
				{
					if (c.is_active_)
					{
						c.velocity_new_ = c.velocity_ + (GRAVITY_CONSTANT + c.force_ / c.mass_) * MS_PER_UPDATE;
						CHECK_ASSERT(Math::IsNAN(c.velocity_new_.x()) == false);
						CHECK_ASSERT(Math::IsNAN(c.velocity_new_.y()) == false);
						CHECK_ASSERT(Math::IsNAN(c.velocity_new_.z()) == false);
						//c.PrintInfo();
					}
				}
			}
		}
	}

	void SnowSimulator::ComputeParticleDeformationGradient()
	{
		for (MaterialPointParticle& it : this->particle_pool_)
		{
			const float3 Position = it.GetLocation();

			int3 GridIndex = this->eulerian_grid_.GetGridIndexFromParticlePosition(Position);
			float2x2 VelocityGrandient;
			for (int i = -1; i < 3; ++i)
			{
				for (int j = -1; j < 3; ++j)
				{
					int3 CurrentIndex = int3(GridIndex.x() + i, GridIndex.y() + j, GridIndex.z());
					Cell& cell = this->eulerian_grid_.GetCell(CurrentIndex);
					//float2x2 VelocityGrandient = VelocityMat * Math::Transpose(ParticleWeightDev);
					//we have row majar matrix here so we need to use outer product for this V * WDevt
					//wchich is equal to  * Math::Transpose(VelocityMat) * ParticleWeightDev;
					//but this will confuse us to understand the equation from paper
					float weight = it.weight_[i + 1][j + 1].x() * it.weight_[i + 1][j + 1].y();
					if (weight > BSPLINE_EPSILON)
					{
						VelocityGrandient = VelocityGrandient +
							Math::OuterProduct(float2(cell.velocity_new_.x(), cell.velocity_new_.y()),
											   float2(it.weight_gradient_[i + 1][j + 1].x(), it.weight_gradient_[i + 1][j + 1].y()));
					}
				}
			}

			float2x2 I;
			Math::Identity(I);
			//^Fe+1
			it.Fe = (I + VelocityGrandient * MS_PER_UPDATE) * it.Fe;
			//PRINT_VAR(it.Fe);

			//^Fp+1 = Fp
			//Fp+1 = ^Fe+1 * ^Fp+1
			float2x2 Fp1 = it.Fe * it.Fp;
			//PRINT_VAR(Fp1);

			float2x2 Up;
			float2	 Dp;
			float2x2 Vtp;
			Math::GetSVD2D(it.Fe, Up, Dp, Vtp);

			Dp.x() = Math::Clamp(Dp.x(), CRIT_COMPRESS, CRIT_STRETCH);
			Dp.y() = Math::Clamp(Dp.y(), CRIT_COMPRESS, CRIT_STRETCH);

			float2x2 NewDp;
			NewDp[0][0] = Dp.x();
			NewDp[1][1] = Dp.y();

			it.Fe = Up * NewDp * Vtp;
			//PRINT_VAR(it.Fe);
			it.Fp = Up * Math::Inverse(NewDp)*Math::Transpose(Up)*Fp1;
			//PRINT_VAR(it.Fp);

		}
	}

	void SnowSimulator::ComputeParticleVelocity()
	{
		for (MaterialPointParticle& it : this->particle_pool_)
		{
			const float3 Position = it.GetLocation();

			int3 GridIndex = this->eulerian_grid_.GetGridIndexFromParticlePosition(Position);
			float3 ParticleGridPosition = this->eulerian_grid_.GetGridPositionFromParticlePosition(Position);
						
			float3 Vpic;
			float3 Vflip;
			float3 new_v;
			for (int i = -1; i < 3; ++i)
			{
				for (int j = -1; j < 3; ++j)
				{
					int3 CurrentIndex = int3(GridIndex.x() + i, GridIndex.y() + j, GridIndex.z());
					Cell& cell = this->eulerian_grid_.GetCell(CurrentIndex);

					float weight = it.weight_[i + 1][j + 1].x() * it.weight_[i + 1][j + 1].y();
					if (weight > BSPLINE_EPSILON)
					{
						Vpic = Vpic + cell.velocity_new_ * weight;
						Vflip = Vflip + (cell.velocity_new_ - cell.velocity_) * weight;
					}
				}
			}
			new_v = Vpic * (1 - FLIP_PERCENT) + (it.GetVelocity() + Vflip) * FLIP_PERCENT;
						
// 			new_v.x() = Math::Clamp(new_v.x(), -500.0f, 500.0f);
// 			new_v.y() = Math::Clamp(new_v.y(), -500.0f, 500.0f);

			it.SetVelocity(new_v);
			//it.PrintInfo();
		}
	}

	void SnowSimulator::ParticleCollision()
	{
		for (MaterialPointParticle& it : this->particle_pool_)
		{

		}
	}

	void SnowSimulator::ComputeParticlePosition()
	{
		for (MaterialPointParticle& it : this->particle_pool_)
		{
			it.Update();
		}
	}

	void TestFunction()
	{
		float2x2 A = float2x2(1,0, 0.13128, 1.17665);
		float2x2 Up;
		float2	 Dp;
		float2x2 Vtp;

		Math::GetSVD2D(A, Up, Dp, Vtp);
	}


	//Do main context init stuffs
	ReturnCode SnowSimulator::Init()
	{
		TestFunction();
		//this->eulerian_grid_.VisualizeCells();
		RandomToFillCircle(20, float2(20, 100));
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
		RandomToFillCircle(8, float2(0,0));
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

		//
		//Grid collision detections
		//and explicit time integration
		//

		SimulatorProfiler.Begin(Profiler::PE_FUNCTION_CALL);
		this->ComputeParticleDeformationGradient();
		SimulatorProfiler.End(Profiler::PE_FUNCTION_CALL, "SnowSimulator Update ComputeParticleDeformationGradient");

		SimulatorProfiler.Begin(Profiler::PE_FUNCTION_CALL);
		this->ComputeParticleVelocity();
		SimulatorProfiler.End(Profiler::PE_FUNCTION_CALL, "SnowSimulator Update ComputeParticleVelocity");

		//
		//Particle collision detections
		SimulatorProfiler.Begin(Profiler::PE_FUNCTION_CALL);
		this->ParticleCollision();
		SimulatorProfiler.End(Profiler::PE_FUNCTION_CALL, "SnowSimulator Update ParticleCollision");
		

		SimulatorProfiler.Begin(Profiler::PE_FUNCTION_CALL);
		this->ComputeParticlePosition();
		SimulatorProfiler.End(Profiler::PE_FUNCTION_CALL, "SnowSimulator Update ComputeParticleVelocity");

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


