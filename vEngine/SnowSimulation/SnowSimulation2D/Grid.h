// <author>Yuan Li</author>
// <email>yuan.li@nyu.edu</email>
// <date>2013-01-10</date>
// <summary>Picking and Assembling gameplay</summary>

#ifndef GRID_H
#define GRID_H

#pragma once

#include <unordered_map>
#include "Common/Header/CommonPreDec.h"

namespace vEngine
{
	namespace Physics
	{

		class Cell
		{

		};

		class Grid
		{
			Grid();
			virtual ~Grid();



			static const uint32_t	VOXEL_CELL_SIZE;

			struct SpatialHashZ
			{
				std::unordered_map < int32_t, std::list<Cell*>> m;
			};
			struct SpatialHashY
			{
				std::unordered_map < int32_t, SpatialHashZ> m;
			};
			struct SpatialHash
			{
				std::unordered_map < int32_t, SpatialHashY> m;
			};


			SpatialHash SpatialHashInstance;
		};
	}
}
#endif //GRID_H