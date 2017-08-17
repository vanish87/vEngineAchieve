#ifndef FONT_H_
#define FONT_H_

#pragma once

#include "Common/Header/CommonPreDec.h"
#include "EnginePreDec.h"

#include "freetype2/include/ft2build.h"
#include FT_FREETYPE_H
#include <string>

namespace vEngine
{
	class Font
	{
	public:
		Font(void);
		virtual ~Font(void);

		static Font& GetFontByName(std::string name);

		void LoadFontFile(std::string file_name);
		//temp solution for d3d shader object
		void SetPPShader(ShaderObject* shander_object);

		void DrawD3DText(std::wstring Text, int2 Position);
		void ClearCPUBuffer();

		void FillBitmap(byte* Buffer, int2 Size, int2 Position);
		void DumpToScreen();
		
	private:
		//Mesh* font_mesh_;
		Texture* bitmap_texture_;
		bool inited_;


		FT_Library  library;
		FT_Face     face;

		byte* cpu_data;

	};
}
#endif

