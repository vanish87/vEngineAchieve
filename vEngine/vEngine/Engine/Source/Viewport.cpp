#include "Engine\Header\Viewport.h"

namespace MocapGE
{
	Viewport::Viewport(void)
	{
	}

	Viewport::Viewport( uint32_t left, uint32_t top, uint32_t width, uint32_t height )
		:left_(left),top_(top),width_(width),height_(height),
		camera_(new Camera(width/(float)height))
	{
		
	}


	Viewport::~Viewport(void)
	{
	}

	void Viewport::SetCamera( Camera* & camera )
	{
		this->camera_ = camera;
	}


}
