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
		viewport_.GetCamera().SetProjection(Math::PI / 4, (float)width / height, 1.0f, 1000.0f);
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
				//PRINT_AND_ASSERT("render view exists");
				return;
			}
		}
		render_views_.push_back(render_view);
	}


	RenderView* FrameBuffer::GetRenderView(size_t index)
	{
		CHECK_ASSERT(index < render_views_.size());
		return render_views_[index];
	}

	Viewport& FrameBuffer::GetViewport()
	{
		return viewport_;
	}
	
}
