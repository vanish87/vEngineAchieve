#ifndef D3DRENDERLAYOUT_H_
#define D3DRENDERLAYOUT_H_

#pragma once
#include "Engine\Header\Renderlayout.h"

namespace vEngine
{
	class D3DRenderLayout : public RenderLayout
	{
	public:
		D3DRenderLayout(void);
		~D3DRenderLayout(void);

		virtual RenderLayout* GetLayout();

	};
}

#endif

