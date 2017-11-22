// <author>Yuan Li</author>
// <email>yuan.li@nyu.edu</email>
// <date>2013-01-10</date>
// <summary>Picking and Assembling gameplay</summary>

#ifndef GRID_H
#define GRID_H

#pragma once

#include <array>
#include "Common/Header/CommonPreDec.h"
#include "Common/Header/Vector.h"

namespace vEngine
{
	namespace Physics
	{

		class Cell
		{
		public:
			Cell()
				:is_active_(false)
			{

			}
			virtual ~Cell()
			{

			}
			float mass_;
			float3 velocity_;
			float3 velocity_new_;
			float3 force_;

			bool is_active_;

			void PrintInfo();
			void Reset();
		};

		class Grid
		{
		public:
			Grid();
			virtual ~Grid();

			void VisualizeCells();
			void VisualizeCellAt(const int3& Index);
			Cell& GetCell(const int3& GridCoordinate);

			float3 GetGridPositionFromParticlePosition(const float3& ParticlePos);
			int3   GetGridIndexFromParticlePosition(const float3& ParticlePos);

			void Reset();

			void PrintInfo();

			static const uint32_t	VOXEL_CELL_SIZE;
			static const uint32_t	VOXEL_GRID_SIZE = 96;

			typedef std::array<std::array<std::array<Cell, VOXEL_GRID_SIZE>, VOXEL_GRID_SIZE>, VOXEL_GRID_SIZE> GridDataType;
			
			GridDataType grid_data_;

			private:
				void AddPointMeshAt(const int3& Index);

		};
	}
}
#endif //GRID_H