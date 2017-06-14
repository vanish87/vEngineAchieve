#include "Engine/Header/Font.h"
#include "Engine/Header/Context.h"

#include "Engine/Header/PostProcess.h"
#include "Engine/Header/Shaderobject.h"

namespace vEngine
{
	static int2 TextureSize(1280,720);
	static std::unordered_map<std::string, Font*> FontMaps;
	static PostProcess* output_to_tex_pp_ = nullptr;

	Font::Font(void)
	{
		FT_Error error = FT_Init_FreeType(&library);
		CHECK_ASSERT(error == FT_Err_Ok);

	}


	Font::~Font(void)
	{
		FT_Error error = FT_Done_FreeType(library);
		CHECK_ASSERT(error == FT_Err_Ok);
	} 
	

	Font& Font::GetFontByName(std::string name)
	{
		if (FontMaps.find(name) == FontMaps.end())
		{
			FontMaps[name] = new Font();
			FontMaps[name]->LoadFontFile("Media/fonts/"+ name + ".ttf");
		}

		return *FontMaps[name];
	}

	void Font::LoadFontFile(std::string file_name)
	{
		TextureSize.x() = Context::Instance().GetConfigure().render_setting.width;
		TextureSize.y() = Context::Instance().GetConfigure().render_setting.height;

		//"Media/fonts/chinese.msyh.ttf"
		FT_Error error = FT_New_Face(library, file_name.c_str(), 0, &face);
		CHECK_ASSERT(error == FT_Err_Ok);

		//error = FT_Set_Pixel_Sizes(face, 16, 16);  here 16 = 12 point * 96/72;
		error = FT_Set_Char_Size(face, 12 << 6, 12 << 6, 96, 96); //12 point in font size and 96 dpi for screen
		//TODO : base dpi(96) is enough for 1080P screen but it could be larger than 96 if there is larger screen resolution.
		//This value does not have a relationship with screen resolution but the larger screen seems to have a larger dpi value.
		//The character widths and heights are specified in 1/64th of points. => "<<6"
		//So only need to change font size(12 in this case).
		//horz_resolution: Standard values are 72 or 96 dpi for display devices like the screen. 
		CHECK_ASSERT(error == FT_Err_Ok);
		
		this->cpu_data = (byte*)(new uint32_t[TextureSize.x() * TextureSize.y()]);

		InitData init_data;
		init_data.data = this->cpu_data;
		init_data.row_pitch = sizeof(uint32_t) * TextureSize.x();
		init_data.slice_pitch = 0;

		this->bitmap_texture_ = Context::Instance().GetRenderFactory().MakeTexture2D(&init_data, TextureSize.x(), TextureSize.y(), 1, 1,
			R8G8B8A8_U, 1, 0, AT_CPU_WRITE_GPU_READ, TU_SHADER_RES);

		//delete[] init_data.data;

		//RenderBuffer* render_buffer = Context::Instance().GetRenderFactory().MakeRenderBuffer(bitmap_texture_, AT_CPU_WRITE_GPU_READ, BU_SHADER_RES);
		byte* data = static_cast<byte*>(bitmap_texture_->Map(AT_CPU_WRITE_GPU_READ));
		CHECK_ASSERT(data != nullptr);
		for (int32_t i = 0; i < TextureSize.x() * TextureSize.y(); ++i)
		{
			uint32_t index = i * 4;
			data[index] = 0;
			data[index + 1] = 0;
			data[index + 2] = 0;
			data[index + 3] = 0;
		}
		bitmap_texture_->UnMap();
				
	}

	void Font::SetPPShader(ShaderObject* shander_object)
	{
		CHECK_ASSERT(shander_object != nullptr);
		shander_object->SetMatrixVariable("g_model_matrix");
		shander_object->SetMatrixVariable("g_world_matrix");
		shander_object->SetMatrixVariable("g_mwv_inv_transpose");
		shander_object->SetMatrixVariable("g_view_proj_matrix");
		shander_object->SetMatrixVariable("g_view_matrix");
		shander_object->SetMatrixVariable("g_inv_proj_matrix");
		shander_object->SetMatrixVariable("g_inv_view_matrix");
		shander_object->SetVectorVariable("g_eye_pos");
		shander_object->SetVectorVariable("g_eye_z");

		shander_object->SetShaderResourceVariable("font_bitmap_tex");


	}

	void Font::DrawD3DText(std::wstring Text, int2 Position)
	{
		FT_Error error;
		int offset = 0;
		//data = static_cast<byte*>(BitmapTexture->Map(AT_CPU_WRITE_GPU_READ));

		CHECK_ASSERT(this->cpu_data != nullptr);
		for (auto c : Text)
		{
			FT_UInt gindex = FT_Get_Char_Index(face, c);
			CHECK_ASSERT(gindex > 0);
			if (gindex != 0 && FT_Load_Glyph(face, gindex, FT_LOAD_NO_BITMAP) == 0)
			{
				error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
				CHECK_ASSERT(error == FT_Err_Ok);

				if (face->glyph->bitmap.buffer == nullptr)
				{
					//space or other unknown char
					offset += face->glyph->metrics.horiAdvance >> 6;
					continue;
				}

				int2 Size = int2(face->glyph->bitmap.pitch, face->glyph->bitmap.rows);
				int2 Pos = int2(Position.x() + offset + face->glyph->bitmap_left, Position.y() - face->glyph->bitmap_top);
				this->FillBitmap(face->glyph->bitmap.buffer, Size, Pos);
				offset += face->glyph->metrics.horiAdvance >> 6;				
			}
		}
	}

	void Font::FillBitmap(byte* Buffer, int2 Size, int2 Position)
	{
		int StartPos = Position.x() + TextureSize.x() * Position.y();
		for (int32_t i = 0; i < Size.y(); ++i)
		{
			for (int32_t j = 0; j < Size.x(); ++j)
			{
				int32_t index = i * Size.x() + j;
				if(index < 0) continue;
				int color = Buffer[index];

				int32_t bitmap_index = StartPos + (i * TextureSize.x()) + j;
				if (bitmap_index < 0) continue;
				bitmap_index *= 4;
				this->cpu_data[bitmap_index] = color;
				this->cpu_data[bitmap_index + 1] = color;
				this->cpu_data[bitmap_index + 2] = color;
				this->cpu_data[bitmap_index + 3] = color>0?color:0;
			}
		}

	}

	void Font::DumpToScreen()
	{
		static bool inited = false;
		if (!inited)
		{
			//ShaderObject* output_to_tex_so_ = Context::Instance().GetRenderFactory().MakeShaderObject();
			ShaderObject* output_to_tex_so_= ShaderObject::FindShaderByName("DebugShader");
			output_to_tex_so_->SetTechnique("PPTech");

			output_to_tex_pp_ = new PostProcess();
			output_to_tex_pp_->SetPPShader(output_to_tex_so_);

			output_to_tex_pp_->SetInput(this->bitmap_texture_, 0);
			output_to_tex_pp_->SetOutput(Context::Instance().GetRenderFactory().GetRenderEngine().CurrentFrameBuffer()->GetTexture(0), 0);
			inited = true;
		}

		{
			byte* data = static_cast<byte*>(this->bitmap_texture_->Map(AT_CPU_WRITE_GPU_READ));
			CHECK_ASSERT(data != nullptr);

			memcpy(data, this->cpu_data, TextureSize.x() * TextureSize.y() * sizeof(uint32_t));
			/*for (int32_t i = 0; i < TextureSize.x() * TextureSize.y(); ++i)
			{
				uint32_t index = i * 4;
				data[index] = this->cpu_data[index];
				data[index + 1] = this->cpu_data[index + 1];
				data[index + 2] = this->cpu_data[index + 2];
				data[index + 3] = this->cpu_data[index + 3];
			}*/
			bitmap_texture_->UnMap();
		}
		output_to_tex_pp_->Apply(false);
	}

	void Font::ClearCPUBuffer()
	{
		//byte* data = static_cast<byte*>(BitmapTexture->Map(AT_CPU_WRITE_GPU_READ));
		CHECK_ASSERT(this->cpu_data != nullptr);

		memset(this->cpu_data, 0, TextureSize.x() * TextureSize.y() * sizeof(uint32_t));
		/*for (int32_t i = 0; i < TextureSize.x() * TextureSize.y(); ++i)
		{
			uint32_t index = i * 4;
			this->cpu_data[index] = 0;
			this->cpu_data[index + 1] = 0;
			this->cpu_data[index + 2] = 0;
			this->cpu_data[index + 3] = 0;
		}*/
		//BitmapTexture->UnMap();
	}

}

