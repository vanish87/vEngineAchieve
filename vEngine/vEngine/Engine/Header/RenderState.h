#ifndef RENDERSTATE_H_
#define RENDERSTATE_H_


#pragma once
#include "Engine/Header/EnginePreDec.h"
#include "Engine/Header/TypeAndFormat.h"

namespace vEngine
{
	class RenderState
	{
	public:
		RenderState(void);
		virtual ~RenderState(void);

	protected:
		RenderStateType state_type_;

	};

}
#endif
