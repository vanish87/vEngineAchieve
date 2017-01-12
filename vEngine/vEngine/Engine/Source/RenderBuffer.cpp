#include "Engine\Header\RenderBuffer.h"

namespace MocapGE
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

