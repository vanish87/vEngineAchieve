#ifndef RENDERSTATE_H_
#define RENDERSTATE_H_


#pragma once
#include "EnginePreDec.h"
#include "TypeAndFormat.h"

namespace vEngine
{
	class RenderState
	{
	public:
		RenderState(void);
		~RenderState(void);

	protected:
		RenderStateType state_type_;

	};

}
#endif
