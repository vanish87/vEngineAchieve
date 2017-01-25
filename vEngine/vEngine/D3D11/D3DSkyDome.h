#ifndef D3DSKYDOME_H_
#define D3DSKYDOME_H_

#pragma once

#include "Engine\Header\EnginePreDec.h"
#include "D3DPreDec.h"
#include "Engine\Header\Model.h"



namespace vEngine
{
	class D3DSkyDome : public Model
	{
	public:
		D3DSkyDome(std::string file_name);
		~D3DSkyDome(void);

		virtual Texture* LoadTexture(std::string file_name);
		//use this to load .fxo file only
		virtual void LoadShaderFile(std::string file_name);

		virtual void Render( int pass_index );

		virtual void SetRenderParameters();

		virtual void EndRender();
	private:

		//no pom for sky dome
		virtual void LoadPomTexture( std::string file_name_ ){};

	private:
		Texture* cube_texture_;
		Mesh* sky_mesh_;
	};

}

#endif

