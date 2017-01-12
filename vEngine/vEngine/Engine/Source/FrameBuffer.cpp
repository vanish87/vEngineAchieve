#include "FrameBuffer.h"

namespace MocapGE
{
	FrameBuffer::FrameBuffer( void )
	{

	}

	FrameBuffer::FrameBuffer( uint32_t left, uint32_t top, uint32_t width, uint32_t height)
		: left_(left), top_(top), width_(width), height_(height)
	{

	}

	FrameBuffer::~FrameBuffer( void )
	{

	}

	Camera* FrameBuffer::GetFrameCamera()
	{
		return viewport_->GetCamera();
	}

	void FrameBuffer::AddRenderView( RenderView* render_view )
	{
		//TODO : Check if the render_view is exist.
		for (size_t i =0; i< render_views_.size(); i++)
		{
			if(render_views_[i]->GetTexture() == render_view->GetTexture())
				return;
		}
		render_views_.push_back(render_view);
	}

	void FrameBuffer::SetFrameCamera( Camera* camera )
	{
		viewport_->SetCamera(camera);
	}

	Viewport* FrameBuffer::GetViewport()
	{
		return viewport_;
	}

	void FrameBuffer::SetViewPort( Viewport* viewport )
	{
		viewport_ = viewport;
	}


}
