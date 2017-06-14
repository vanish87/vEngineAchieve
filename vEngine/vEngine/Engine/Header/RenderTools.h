#ifndef RENDERTOOLS_H_
#define RENDERTOOLS_H_

#pragma once
#include "Engine/Header/EnginePreDec.h"
#include <string>

namespace vEngine
{
	class RenderTools
	{
	private:
		RenderTools() {}; // ctor hidden
		RenderTools(RenderTools const&) {}; // copy ctor hidden
		RenderTools& operator=(RenderTools const&) { return *this; }; // assign op. hidden
		~RenderTools() {}; // dtor hidden

	public:
		static RenderTools& GetInstance()
		{
			static RenderTools Instance;
			return Instance;
		};

		Mesh* MakeFullScreenMesh();
		Mesh* MakeSphereMesh();
		Texture* LoadTexture(std::string file_name);//TODO a independent texture loader
	};
}

#endif