#include "Engine\Header\FrameBuffer.h"

namespace vEngine
{
	FrameBuffer::FrameBuffer(void)
		: FrameBuffer(128, 128)
	{

	}

	FrameBuffer::FrameBuffer(uint32_t width, uint32_t height)
		: width_(width), height_(height)
	{
		viewport_.SetWidth(width);
		viewport_.SetHeight(height);
	}

	FrameBuffer::~FrameBuffer( void )
	{

	}

	void FrameBuffer::AddRenderView( RenderView* render_view )
	{
		//TODO : Check if the render_view is exist.
		for (size_t i =0; i< render_views_.size(); i++)
		{
			if (render_views_[i]->GetTexture() == render_view->GetTexture())
			{
				delete render_view;
				return;
			}
		}
		render_views_.push_back(render_view);
	}


	Viewport& FrameBuffer::GetViewport()
	{
		return viewport_;
	}
	
}
