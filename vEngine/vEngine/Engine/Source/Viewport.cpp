#include "Engine\Header\Viewport.h"

namespace vEngine
{
	Viewport::Viewport(void)
	{
	}

	Viewport::Viewport( uint32_t left, uint32_t top, uint32_t width, uint32_t height )
		:left_(left),top_(top),width_(width),height_(height)
	{
		camera_.SetProjection(Math::PI / 4, 1.0f*width / height, 1, 100);
	}


	Viewport::~Viewport(void)
	{
	}

	void Viewport::SetCamera(Camera* camera)
	{
		assert(camera != nullptr && *camera != GameObject::NullObject());
		this->camera_.SetView(camera->GetPos(), camera->GetLookAt(), camera->GetUp());
		float2 NearFar = camera->GetNearFar();
		this->camera_.SetProjection(camera->GetFovy(), camera->GetAspect(), NearFar.x(), NearFar.y());
	}

}
