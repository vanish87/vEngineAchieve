#include "Engine\Header\RenderBuffer.h"

namespace vEngine
{
	RenderBuffer::RenderBuffer(void)
	{
	}


	RenderBuffer::~RenderBuffer(void)
	{
	}

	BufferUsage RenderBuffer::GetUsage()
	{
		return usage_;
	}

}

