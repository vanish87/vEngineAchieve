#include <limits>
#include "Common\Header\AABBox.h"
#include "Common\Header\Math.h"

namespace MocapGE
{
	AABBox::AABBox(void)
	{
	}


	AABBox::~AABBox(void)
	{
	}


	AABBox::AABBox(std::vector<VertexType*> vertice, uint32_t vcout )
	{
		ComputeAABB(vertice, vcout);
	}

	void AABBox::ComputeAABB( std::vector<VertexType*> vertice, uint32_t vcout )
	{
		float3 min( std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
		float3 max(-std::numeric_limits<float>::max(),-std::numeric_limits<float>::max(),-std::numeric_limits<float>::max());
		for(size_t i=0; i< vcout; i++)
		{
			float3 pos = vertice[i]->position;
			//std::cout<<pos.x()<<" "<<pos.y()<<" "<<pos.z()<<std::endl;
			max = Math::Max(pos, max);
			min = Math::Min(pos, min);
		}

		std::cout<<min.x()<<" "<<min.y()<<" "<<min.z()<<std::endl;
		std::cout<<max.x()<<" "<<max.y()<<" "<<max.z()<<std::endl;
		max_ = max;
		min_ = min;
	}

	MocapGE::float3 AABBox::Min()
	{
		return min_;
	}

	MocapGE::float3 AABBox::Max()
	{
		return max_;
	}

}
