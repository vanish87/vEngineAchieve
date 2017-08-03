//
//
//TODO@This is a temporary solution for model loader in D3D. Because it uses D3DtextureLoader tool from Microsoft.
#ifndef D3DMODEL_H_
#define D3DMODEL_H_

#pragma once

#include "Engine/Header/EnginePreDec.h"
#include "D3DPreDec.h"
#include "Engine/Header/Model.h"

#include "D3DShaderobject.h"

//#pragma comment(lib, "..\\External\\DirectXTK.lib")

namespace vEngine
{

	class D3DModel : public Model
	{
	public:
		D3DModel(void);
		~D3DModel(void);

		DEBUG_CLASS_FILE_NAME
			
		virtual Texture* LoadTexture( std::string file_name );
		virtual void LoadPomTexture( std::string file_name_ );


	};

}

#endif

