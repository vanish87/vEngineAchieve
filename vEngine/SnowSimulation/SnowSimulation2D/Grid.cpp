#include "Grid.h"

#include "Common/Header/Math.h"
#include "Engine/Header/Model.h"
#include "Engine/Header/SceneObject.h"
#include "Engine/Header/RenderTools.h"

namespace vEngine
{
	namespace Physics
	{
		const uint32_t	Grid::VOXEL_CELL_SIZE = 10;

		static Cell NullCell;

		Grid::Grid()
		{

		}

		Grid::~Grid()
		{

		}

		void Grid::VisualizeCells()
		{
			int xpos = 0;
			int ypos = 0;;
			int zpos = 0;
			for (xpos = -(int)Grid::VOXEL_GRID_SIZE/2; xpos < (int)Grid::VOXEL_GRID_SIZE/2; xpos++)
			{
				for (ypos = -(int)Grid::VOXEL_GRID_SIZE/2; ypos < (int)Grid::VOXEL_GRID_SIZE/2; ypos++)
				{
					if (   (xpos < -(int)Grid::VOXEL_GRID_SIZE / 2 + 1 || xpos >(int)Grid::VOXEL_GRID_SIZE / 2 - 2) 
						|| (ypos < -(int)Grid::VOXEL_GRID_SIZE / 2 + 1 || ypos >(int)Grid::VOXEL_GRID_SIZE / 2 - 2))
					{
						this->AddPointMeshAt(int3(xpos, ypos, 0));
					}
				}
			}
		}

		void Grid::VisualizeCellAt(const int3 & Index)
		{
			this->AddPointMeshAt(Index);
		}

		Cell& Grid::GetCell(const int3& GridCoordinate)
		{
			//map [-VOXEL_GRID_SIZE/2, VOXEL_GRID_SIZE/2] to [0, VOXEL_GRID_SIZE]
			//in order to access array by index
			int3 GridIndex = GridCoordinate + Grid::VOXEL_GRID_SIZE/2;
			//GridIndex = GridIndex / Grid::VOXEL_CELL_SIZE;

			if (   GridIndex.x() >= 0 && GridIndex.x() < Grid::VOXEL_GRID_SIZE
				&& GridIndex.y() >= 0 && GridIndex.y() < Grid::VOXEL_GRID_SIZE
				&& GridIndex.z() >= 0 && GridIndex.z() < Grid::VOXEL_GRID_SIZE)
			{
				return this->grid_data_[GridIndex.x()][GridIndex.y()][GridIndex.z()];
			}
			else
			{
				CHECK_ASSERT(false, "Invalid Cell Index");
				return NullCell;
			}
		}

		float3 Grid::GetGridPositionFromParticlePosition(const float3 & ParticlePos)
		{
			float3 GridPosition = ParticlePos / (float)Grid::VOXEL_CELL_SIZE;
			GridPosition.x() = ParticlePos.x() < 0 ? GridPosition.x() - 1 : GridPosition.x();
			GridPosition.y() = ParticlePos.y() < 0 ? GridPosition.y() - 1 : GridPosition.y();
			GridPosition.z() = ParticlePos.z() < 0 ? GridPosition.z() - 1 : GridPosition.z();
			return GridPosition;
		}

		int3 Grid::GetGridIndexFromParticlePosition(const float3& ParticlePos)
		{
			int3 GridIndex;
			GridIndex.x() = (int32_t)(ParticlePos.x() / Grid::VOXEL_CELL_SIZE);
			GridIndex.y() = (int32_t)(ParticlePos.y() / Grid::VOXEL_CELL_SIZE);
			GridIndex.z() = (int32_t)(ParticlePos.z() / Grid::VOXEL_CELL_SIZE);
			GridIndex.x() = ParticlePos.x() < 0 ? GridIndex.x()-1 : GridIndex.x();
			GridIndex.y() = ParticlePos.y() < 0 ? GridIndex.y()-1 : GridIndex.y();
			GridIndex.z() = ParticlePos.z() < 0 ? GridIndex.z()-1 : GridIndex.z();
			return GridIndex;
		}

		void Grid::Reset()
		{
			this->grid_data_.fill({});
		}

		void Grid::PrintInfo()
		{
			for (auto& x : this->grid_data_)
			{
				for (auto& y : x)
				{
					for (auto& z : y)
					{
						if (z.is_active_ || z.mass_ > 0)
						{
							z.PrintInfo();
						}
					}
				}
			}
			
		}

		void Grid::AddPointMeshAt(const int3 & Index)
		{
			Mesh* mesh = RenderTools::GetInstance().Make2DCricleMesh(3);

			Model* model = new Model();
			model->AddMesh(mesh);
			Material* meshMat = new Material();
			meshMat->diffuse = float4(0, 1, 0, 1);
			meshMat->specular = float4(0, 0, 0, 0);
			meshMat->shininess = 10;

			model->AddMaterial(meshMat);
			model->LoadShaderFile("DeferredLighting");

			float4x4 mat;
			Math::Identity(mat);
			Math::Translate(mat, Index.x() * (int)Grid::VOXEL_CELL_SIZE, Index.y() * (int)Grid::VOXEL_CELL_SIZE, Index.z() * (int)Grid::VOXEL_CELL_SIZE);

			//PRINT(xpos - (int)(Grid::VOXEL_GRID_SIZE / 2)<< ypos - (int)(Grid::VOXEL_GRID_SIZE / 2));
			model->SetModelMatrix(mat);

			SceneObject* s = new SceneObject(model);
			s->AddToScene();
			s->SetVisiable(true);
		}

		void Cell::PrintInfo()
		{
			PRINT("Cell-----------");
			PRINT("Mass " << this->mass_);
			PRINT("Force " << this->force_.x() << " " <<this->force_.y());
			PRINT("Velocity " << this->velocity_.x() << " " << this->velocity_.y());
			PRINT("VelocityNew " << this->velocity_new_.x() << " " << this->velocity_new_.y());
		}

		void Cell::Reset()
		{
			this->mass_ = 0;
			this->force_ = float3();
			this->velocity_ = float3();
			this->velocity_new_ = float3();
		}

}
}

