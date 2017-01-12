//
//
//TODO@This is a temporary solution for model loader in D3D. Because it uses D3DtextureLoader tool from Microsoft.
#ifndef D3DMODEL_H_
#define D3DMODEL_H_

#pragma once

#include "Engine\Header\EnginePreDec.h"
#include "D3DPreDec.h"
#include "Engine\Header\Model.h"

#include "D3DShaderobject.h"
#include "WICTextureLoader.h"

//#pragma comment(lib, "..\\External\\DirectXTK.lib")

namespace MocapGE
{
	//Use LoadFile to load .dae file
	//Use LoadShaderFile to load .fxo file, which is the compiled file of .fx file
	class D3DModel : public Model
	{
	public:
		D3DModel(void);
		~D3DModel(void);

		//Load .dae file
		virtual void LoadFile(std::string file_name);
		//use this to load .fxo file only
		virtual void LoadShaderFile(std::string file_name);

		virtual void SetRenderParameters();
		virtual void Render(int pass_index);
		virtual void EndRender();

		virtual Texture* LoadTexture( std::string file_name );

		virtual void LoadPomTexture( std::string file_name_ );


	};

}

#endif

