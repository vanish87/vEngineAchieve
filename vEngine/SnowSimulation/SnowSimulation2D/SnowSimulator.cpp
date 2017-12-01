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
#include "Engine/Header/Text.h"
#include <string>

#define USE_OPENMP

#ifdef USE_OPENMP
#include <omp.h>
#endif // USE_OPENMP


namespace vEngine
{
	using namespace Physics;


	const float3	SnowSimulator::GRAVITY_CONSTANT = float3(0, -9.80f, 0);

	float SpeedLimit = 300;

	//const float BSPLINE_EPSILON = 1e-4f;

	static bool WithAPIC = false;

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
		float ParticleArea = PARTICLE_DIAM * PARTICLE_DIAM;
		float ParticleMass = DENSITY * ParticleArea *0.03;

		float CricleArea = Math::PI * Raduis * Raduis / 100000;
		uint32_t NumberOfParticle = CricleArea / ParticleArea;

		int Count = 0;

		float2 CriclePos[3] =
		{ Position , float2(80,50) , float2(0,0)};


		float4 CricleColor[3] =
		{ float4(1,0,0,1) , float4(0,0,1,1) , float4(1,1,1,1)};


		float2 CricleSpeed[3] =
		{ float2(200,-150) , float2(0,0) , float2(0,0) };

		for (uint32_t i = 0; i < this->particle_pool_.size(); ++i)
		{
			if (i < NumberOfParticle * (Count + 1))
			{
				Particle& it = this->particle_pool_[i];
				it.Create(CricleColor[Count]);
				float3 rand = float3(Math::RandomReal(-Raduis, Raduis), Math::RandomReal(-Raduis, Raduis), 0);
				while (Math::Dot(rand, rand) > Raduis * Raduis)
				{
					rand = float3(Math::RandomReal(-Raduis, Raduis), Math::RandomReal(-Raduis, Raduis), 0);
					it.SetLocation(rand);
				}
				rand.x() += CriclePos[Count].x();
				rand.y() += CriclePos[Count].y();
				it.SetLocation(rand);
				it.SetVelocity(float3(CricleSpeed[Count].x(), CricleSpeed[Count].y(), 0));

				it.SetMass(ParticleMass);

				it.SetScale(float3(1, 1, 1));
				it.SetVisiable(true);
				it.SetVMainThreadUpdate(false);

				it.AddToScene();

			}
			else
			{
				Count++;
			}

			
		}
	}

	void SnowSimulator::ResterizeParticleMassToGrid()
	{
		//map mass first
		#ifdef USE_OPENMP
		#pragma omp parallel for
		#endif
		for (int32_t i = 0; i < NUMBER_OF_PARTICLES; ++i)
			//for (MaterialPointParticle& it : this->particle_pool_)
		{
			MaterialPointParticle& it = this->particle_pool_[i];
			const float3 Position = it.GetPosition();

			//PRINT("From particle " << Position.x() << " " << Position.y());

			int3 GridIndex = this->eulerian_grid_.GetGridIndexFromParticlePosition(Position);
			float3 ParticleGridPosition = this->eulerian_grid_.GetGridPositionFromParticlePosition(Position);

			it.debug_grid_index_ = GridIndex;

			//PRINT("Get Index " << GridIndex.x() << " " << GridIndex.y());
			//PRINT("Get GridPos " << ParticleGridPosition.x() << " " << ParticleGridPosition.y());
			Math::Identity(it.D);

			for (int i = -1; i < 3; ++i)
			{
				for (int j = -1; j < 3; ++j)
				{
					int3 CurrentIndex = int3(GridIndex.x() + i, GridIndex.y() + j, GridIndex.z());
					float3 Nx = ParticleGridPosition - CurrentIndex;

					//PRINT("Index "<< CurrentIndex.x()<<" "<< CurrentIndex.y()<<" Get Nx =  " << Nx.x() << " " << Nx.y());
					it.weight_[i + 1][j + 1] = Math::GetBSpline(Nx);
					//TODO 
					//here we remove z for 2D
					it.weight_[i + 1][j + 1].z() = 1;

					it.weight_all_[i + 1][j + 1] = it.weight_[i + 1][j + 1].x() * it.weight_[i + 1][j + 1].y() * it.weight_[i + 1][j + 1].z();

					//PRINT_VAR(it.weight_[i + 1][j + 1]);// .z() = 1;
					float2 Nx2D(Nx.x(), Nx.y());
					it.D = it.D + Math::OuterProduct(Nx2D, Nx2D) * it.weight_all_[i + 1][j + 1];

					Cell& cell = this->eulerian_grid_.GetCell(CurrentIndex);
					cell.mass_ += it.GetMass() * it.weight_all_[i + 1][j + 1];
					//CHECK_ASSERT(cell.mass_ > 0);
				}
			}

			//cell.PrintInfo();
		}

		
	}


	void SnowSimulator::ResterizeParticleToGridWithAPIC()
	{
		//for each frame, we will recalculate all grid information
		this->eulerian_grid_.Reset();

		//map mass first
		this->ResterizeParticleMassToGrid();


		#ifdef USE_OPENMP
		#pragma omp parallel for
		#endif
		for (int32_t i = 0; i < NUMBER_OF_PARTICLES; ++i)
			//for (MaterialPointParticle& it : this->particle_pool_)
		{
			MaterialPointParticle& it = this->particle_pool_[i];
			const float3 Position = it.GetPosition();

			int3 GridIndex = this->eulerian_grid_.GetGridIndexFromParticlePosition(Position);
			float3 ParticleGridPosition = this->eulerian_grid_.GetGridPositionFromParticlePosition(Position);


			for (int i = -1; i < 3; ++i)
			{
				for (int j = -1; j < 3; ++j)
				{
					//float weight = it.weight_[i + 1][j + 1].x() * it.weight_[i + 1][j + 1].y();
					//if (weight > BSPLINE_EPSILON)
					int3 CurrentIndex = int3(GridIndex.x() + i, GridIndex.y() + j, GridIndex.z());
					Cell& cell = this->eulerian_grid_.GetCell(CurrentIndex);

					//if(cell.is_active_)					
					{
						float3 Nx = ParticleGridPosition - CurrentIndex;
						float2x2 temp = it.B * Math::Inverse(it.D);
						float3 VpAffine;
						VpAffine.x() = temp[0][0] * Nx.x() + temp[0][1] * Nx.y();
						VpAffine.y() = temp[1][0] * Nx.x() + temp[1][1] * Nx.y();

						cell.momentum_ += ((it.GetVelocity() + VpAffine) * it.GetMass() * it.weight_all_[i + 1][j + 1]);
						//cell.is_active_ = true;

						//CHECK_ASSERT(cell.mass_ > 0);
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
					if (z.mass_ > 0)
					{
						z.is_active_ = true;
						z.velocity_ = z.momentum_ / z.mass_;
						//z.PrintInfo();
					}
					else
					{
						z.velocity_ = float3(0, 0, 0);
						z.momentum_ = float3(0, 0, 0);
						z.mass_ = 0;
					}
				}
			}
		}
	}

	void SnowSimulator::ComputeParticleVelocityWithAPIC()
	{
		for (MaterialPointParticle& it : this->particle_pool_)
		{
			const float3 Position = it.GetPosition();

			int3 GridIndex = this->eulerian_grid_.GetGridIndexFromParticlePosition(Position);
			float3 ParticleGridPosition = this->eulerian_grid_.GetGridPositionFromParticlePosition(Position);

			Math::Identity(it.B);

			float3 Vp;
			for (int i = -1; i < 3; ++i)
			{
				for (int j = -1; j < 3; ++j)
				{
					int3 CurrentIndex = int3(GridIndex.x() + i, GridIndex.y() + j, GridIndex.z());
					Cell& cell = this->eulerian_grid_.GetCell(CurrentIndex);

					float3 Nx = ParticleGridPosition - CurrentIndex;
					float2 Nx2D(Nx.x(), Nx.y());
					float2 V2D(cell.velocity_new_.x(), cell.velocity_new_.y());

					//float weight = it.weight_[i + 1][j + 1].x() * it.weight_[i + 1][j + 1].y();
					//if (weight > BSPLINE_EPSILON)
					if (cell.is_active_)
					{
						Vp = Vp + cell.velocity_new_ * it.weight_all_[i + 1][j + 1];
						it.B = it.B + (Math::OuterProduct(V2D, Nx2D) * it.weight_all_[i + 1][j + 1]);
					}
				}
			}

			it.SetVelocity(Vp);
			//it.PrintInfo();
		}
	}

	void SnowSimulator::ResterizeParticleToGrid()
	{
		//to map particles mass and velocities to Grid with weight kernel
		//PRINT("Cell Size is "<< Grid::VOXEL_CELL_SIZE);

		//for each frame, we will recalculate all grid information
		this->eulerian_grid_.Reset();


		//map mass first
		this->ResterizeParticleMassToGrid();


		for (MaterialPointParticle& it : this->particle_pool_)
		{
			const float3 Position = it.GetPosition();

			int3 GridIndex = this->eulerian_grid_.GetGridIndexFromParticlePosition(Position);
			float3 ParticleGridPosition = this->eulerian_grid_.GetGridPositionFromParticlePosition(Position);

			
			for (int i = -1; i < 3; ++i)
			{
				for (int j = -1; j < 3; ++j)
				{
					int3 CurrentIndex = int3(GridIndex.x() + i, GridIndex.y() + j, GridIndex.z());
					Cell& cell = this->eulerian_grid_.GetCell(CurrentIndex);
					cell.momentum_ += (it.GetVelocity() * it.GetMass() * it.weight_all_[i + 1][j + 1]);
				}
			}

		}


		for (auto& x : this->eulerian_grid_.grid_data_)
		{
			for (auto& y : x)
			{
				for (auto& z : y)
				{
					if (z.mass_ > 0)
					{
						z.is_active_ = true;
						z.velocity_ = z.momentum_ / z.mass_;
						//z.PrintInfo();
					}
					else
					{
						z.velocity_ = float3(0, 0, 0);
						z.momentum_ = float3(0, 0, 0);
						z.mass_ = 0;
					}
				}
			}
		}



// 		PRINT_WARNING("begin of ResterizeParticleToGrid-----------------------------------------------");
// 		this->eulerian_grid_.PrintInfo();
// 		PRINT_WARNING("end of ResterizeParticleToGrid-----------------------------------------------");

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
			const float3 Position = it.GetPosition();

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

					it.density_ += (cell.mass_ *  it.weight_all_[i + 1][j + 1]);

				}
			}
			CHECK_ASSERT(it.density_ > 0);
			it.density_ /= CellArea;
			
			it.volume_ = it.GetMass() / it.density_;
			//it.PrintInfo();
		}

// 		PRINT_WARNING("begin of ComputeParticleVolumesAndDensities-----------------------------------------------");
// 		this->eulerian_grid_.PrintInfo();
// 		PRINT_WARNING("end of ComputeParticleVolumesAndDensities-----------------------------------------------");
	}

	void SnowSimulator::ComputeGridForce()
	{

		#ifdef USE_OPENMP
		#pragma omp parallel for
		#endif
		for (int32_t i = 0; i < NUMBER_OF_PARTICLES; ++i)
			//for (MaterialPointParticle& it : this->particle_pool_)
		{
			MaterialPointParticle& it = this->particle_pool_[i];
			it.ComputeEnergyDensity();

			const float3 Position = it.GetPosition();

			int3 GridIndex = this->eulerian_grid_.GetGridIndexFromParticlePosition(Position);
			float3 ParticleGridPosition = this->eulerian_grid_.GetGridPositionFromParticlePosition(Position);
			
			for (int i = -1; i < 3; ++i)
			{
				for (int j = -1; j < 3; ++j)
				{
					int3 CurrentIndex = int3(GridIndex.x() + i, GridIndex.y() + j, GridIndex.z());
					float3 Nx = ParticleGridPosition - CurrentIndex;

					it.weight_dev_[i + 1][j + 1] = Math::GetBSplineDerivative(Nx);// / Grid::VOXEL_CELL_SIZE;
					it.weight_gradient_[i + 1][j + 1].x() = it.weight_dev_[i + 1][j + 1].x() * it.weight_[i + 1][j + 1].y();
					it.weight_gradient_[i + 1][j + 1].y() = it.weight_[i + 1][j + 1].x() * it.weight_dev_[i + 1][j + 1].y();
					it.weight_gradient_[i + 1][j + 1].z() = 1;

					//it.weight_gradient_[i + 1][j + 1] = it.weight_gradient_[i + 1][j + 1] / Grid::VOXEL_CELL_SIZE;

					Cell& cell = this->eulerian_grid_.GetCell(CurrentIndex);

					//float weight = it.weight_[i + 1][j + 1].x() * it.weight_[i + 1][j + 1].y();
					//if (weight > BSPLINE_EPSILON)
					if(cell.is_active_)
					{
						float3 newf = float3(it.force_[0][0] * it.weight_gradient_[i + 1][j + 1][0] + it.force_[1][0] * it.weight_gradient_[i + 1][j + 1][1],
							it.force_[0][1] * it.weight_gradient_[i + 1][j + 1][0] + it.force_[1][1] * it.weight_gradient_[i + 1][j + 1][1],
							0);
						CHECK_ASSERT(Math::IsNAN(newf.x()) == false);
						CHECK_ASSERT(Math::IsNAN(newf.y()) == false);
						CHECK_ASSERT(Math::IsNAN(newf.z()) == false);

						cell.force_ += newf;
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
		int count = 0;
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


						//c.velocity_new_.x() = Math::Clamp(c.velocity_new_.x(), -SpeedLimit, SpeedLimit);
						//c.velocity_new_.y() = Math::Clamp(c.velocity_new_.y(), -SpeedLimit, SpeedLimit);
						//c.PrintInfo();
						count++;
					}
				}
			}
		}

		Text cout = L"Cell count:"+ std::to_wstring(count);
		cout.SetRect(int4(500, 500, 0, 0));
		cout.Draw();
	}

	void SnowSimulator::GridCollision()
	{

		float GridDeltaTime = MS_PER_UPDATE / Grid::VOXEL_CELL_SIZE;


		#ifdef USE_OPENMP
		#pragma omp parallel for
		#endif
		for (int i = 0; i < this->eulerian_grid_.grid_data_.size(); ++i)
		{
			Grid::GridDataTypeX x = this->eulerian_grid_.grid_data_[i];
			for (int j = 0; j < x.size(); ++j)
			{
				Grid::GridDataTypeY y = x[j];
				for (int k = 0; k < y.size(); ++k)
				{
					Cell& c = y[k];

					if (c.is_active_)
					{
						float3 CurrentPosition = float3(i, j, k);
						float3 NewPosition = c.velocity_new_ * GridDeltaTime + CurrentPosition;

						if (NewPosition.x() > ((int)Grid::VOXEL_GRID_SIZE) / 2 - 2 || NewPosition.x() < -((int)Grid::VOXEL_GRID_SIZE) / 2 + 1)
						{
							c.velocity_new_.x() = 0;
							c.velocity_new_.y() *= STICKY;
						}
						if (NewPosition.y() > ((int)Grid::VOXEL_GRID_SIZE) / 2 - 2 || NewPosition.y() < -((int)Grid::VOXEL_GRID_SIZE ) / 2 + 1)
						{
							c.velocity_new_.y() = 0;
							c.velocity_new_.x() *= STICKY;
						}
					}					
				}
			}
		}
	}

	void SnowSimulator::ComputeParticleDeformationGradient()
	{

		#ifdef USE_OPENMP
		#pragma omp parallel for
		#endif
		for (int32_t i = 0; i < NUMBER_OF_PARTICLES; ++i)
			//for (MaterialPointParticle& it : this->particle_pool_)
		{
			MaterialPointParticle& it = this->particle_pool_[i];
			const float3 Position = it.GetPosition();

			int3 GridIndex = this->eulerian_grid_.GetGridIndexFromParticlePosition(Position);
			float2x2 VelocityGrandient;
			Math::Identity(VelocityGrandient);
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
					//float weight = it.weight_[i + 1][j + 1].x() * it.weight_[i + 1][j + 1].y();
					if (cell.is_active_)
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

			//here CRIT_COMPRESS stores (1 - Sigma)
			//CRIT_STRETCH stores (1 + Sigma)
			Dp.x() = Math::Clamp(Dp.x(), CRIT_COMPRESS, CRIT_STRETCH);
			Dp.y() = Math::Clamp(Dp.y(), CRIT_COMPRESS, CRIT_STRETCH);

			float2x2 NewDp;
			NewDp[0][0] = Dp.x();
			NewDp[1][1] = Dp.y();

			it.Fe = Up * NewDp * Vtp;
			//PRINT_VAR(it.Fe);
			it.Fp = Up * Math::Inverse(NewDp)*Math::Transpose(Up)*Fp1;
			//PRINT_VAR(it.Fp);

			//CHECK_ASSERT(Math::determinant(it.Fp) < 10);

// 			CHECK_ASSERT(it.Fp[0][0] < 10);
// 			CHECK_ASSERT(it.Fp[0][1] < 10);
// 			CHECK_ASSERT(it.Fp[1][0] < 10);
// 			CHECK_ASSERT(it.Fp[1][1] < 10);

			CHECK_ASSERT(Math::IsNAN(it.Fe[0][0]) == false);
			CHECK_ASSERT(Math::IsNAN(it.Fe[0][1]) == false);
			CHECK_ASSERT(Math::IsNAN(it.Fe[1][0]) == false);
			CHECK_ASSERT(Math::IsNAN(it.Fe[1][1]) == false);

			CHECK_ASSERT(Math::IsNAN(it.Fp[0][0]) == false);
			CHECK_ASSERT(Math::IsNAN(it.Fp[0][1]) == false);
			CHECK_ASSERT(Math::IsNAN(it.Fp[1][0]) == false);
			CHECK_ASSERT(Math::IsNAN(it.Fp[1][1]) == false);

		}
	}

	void SnowSimulator::ComputeParticleVelocity()
	{
		#ifdef USE_OPENMP
		#pragma omp parallel for
		#endif
		for (int32_t i = 0; i < NUMBER_OF_PARTICLES; ++i)
			//for (MaterialPointParticle& it : this->particle_pool_)
		{
			MaterialPointParticle& it = this->particle_pool_[i];
			const float3 Position = it.GetPosition();

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

					//float weight = it.weight_[i + 1][j + 1].x() * it.weight_[i + 1][j + 1].y();
					//if (weight > BSPLINE_EPSILON)
					if(cell.is_active_)
					{
						Vpic += cell.velocity_new_ * it.weight_all_[i + 1][j + 1];
						Vflip += (cell.velocity_new_ - cell.velocity_) * it.weight_all_[i + 1][j + 1];
					}
				}
			}
			new_v = Vpic * (1 - FLIP_PERCENT) + (it.GetVelocity() + Vflip) * FLIP_PERCENT;
						
  			new_v.x() = Math::Clamp(new_v.x(), -SpeedLimit, SpeedLimit);
  			new_v.y() = Math::Clamp(new_v.y(), -SpeedLimit, SpeedLimit);

			it.SetVelocity(new_v);
			//it.PrintInfo();
		}
	}

	void SnowSimulator::ParticleCollision()
	{
		float ParticleDeltTime = MS_PER_UPDATE;

		#ifdef USE_OPENMP
		#pragma omp parallel for
		#endif
		for (int32_t i = 0; i < NUMBER_OF_PARTICLES; ++i)
			//for (MaterialPointParticle& it : this->particle_pool_)
		{
			MaterialPointParticle& it = this->particle_pool_[i];
			float3 NewPosition = it.GetVelocity() * ParticleDeltTime + it.GetPosition();
			float3 NewVelocity = it.GetVelocity();
			
			float left = (((int)Grid::VOXEL_GRID_SIZE) / 2 )*Grid::VOXEL_CELL_SIZE;
			float up   = (((int)Grid::VOXEL_GRID_SIZE) / 2)*Grid::VOXEL_CELL_SIZE;
			if (NewPosition.x() > left - (Grid::VOXEL_CELL_SIZE * 2) || NewPosition.x() < -left + (Grid::VOXEL_CELL_SIZE * 2))
			{
				NewVelocity.x() = -STICKY * NewVelocity.x();
			}
			if (NewPosition.y() > up - (Grid::VOXEL_CELL_SIZE * 2) || NewPosition.y() < -up + (Grid::VOXEL_CELL_SIZE * 2))
			{
				NewVelocity.y() = -STICKY * NewVelocity.y();
			}

			it.SetVelocity(NewVelocity);
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

		#ifdef USE_OPENMP
		omp_set_num_threads(32);
		#endif

		TestFunction();
		this->eulerian_grid_.VisualizeCells();
		RandomToFillCircle(20, float2(0, 100));
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
		if (WithAPIC)
		{
			SimulatorProfiler.Begin(Profiler::PE_FUNCTION_CALL);
			this->ResterizeParticleToGridWithAPIC();
			SimulatorProfiler.End(Profiler::PE_FUNCTION_CALL, "SnowSimulator Update ResterizeParticleToGrid");			
		}
		else
		{
			SimulatorProfiler.Begin(Profiler::PE_FUNCTION_CALL);
			this->ResterizeParticleToGrid();
			SimulatorProfiler.End(Profiler::PE_FUNCTION_CALL, "SnowSimulator Update ResterizeParticleToGrid");
		}

		SimulatorProfiler.Begin(Profiler::PE_FUNCTION_CALL);
		this->ComputeGridForce();
		SimulatorProfiler.End(Profiler::PE_FUNCTION_CALL, "SnowSimulator Update ComputeGridForce");

		SimulatorProfiler.Begin(Profiler::PE_FUNCTION_CALL);
		this->ComputeGridVelocity();
		SimulatorProfiler.End(Profiler::PE_FUNCTION_CALL, "SnowSimulator Update ComputeGridVelocity");

		//
		//Grid collision detections
		//and explicit time integration
		SimulatorProfiler.Begin(Profiler::PE_FUNCTION_CALL);
		this->GridCollision();
		SimulatorProfiler.End(Profiler::PE_FUNCTION_CALL, "SnowSimulator Update GridCollision");
		//

		SimulatorProfiler.Begin(Profiler::PE_FUNCTION_CALL);
		this->ComputeParticleDeformationGradient();
		SimulatorProfiler.End(Profiler::PE_FUNCTION_CALL, "SnowSimulator Update ComputeParticleDeformationGradient");

		if (WithAPIC)
		{
			SimulatorProfiler.Begin(Profiler::PE_FUNCTION_CALL);
			this->ComputeParticleVelocityWithAPIC();
			SimulatorProfiler.End(Profiler::PE_FUNCTION_CALL, "SnowSimulator Update ComputeParticleVelocity");			
		}
		else
		{
			SimulatorProfiler.Begin(Profiler::PE_FUNCTION_CALL);
			this->ComputeParticleVelocity();
			SimulatorProfiler.End(Profiler::PE_FUNCTION_CALL, "SnowSimulator Update ComputeParticleVelocity");
		}

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


