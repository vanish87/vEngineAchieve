#include "Engine\Header\Font.h"
#include "Engine\Header\Context.h"

#include "Engine\Header\PostProcess.h"
#include "D3D11\D3DShaderobject.h"

namespace vEngine
{
	static int2 TextureSize(1280,720);

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
	

	void Font::LoadFontFile(std::string file_name)
	{
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
		
		InitData init_data;
		init_data.data = new uint32_t[TextureSize.x() * TextureSize.y()];
		init_data.row_pitch = sizeof(uint32_t) * TextureSize.x();
		init_data.slice_pitch = 0;

		this->bitmap_texture_ = Context::Instance().GetRenderFactory().MakeTexture2D(&init_data, TextureSize.x(), TextureSize.y(), 1, 1,
			R8G8B8A8_U, 1, 0, AT_CPU_WRITE_GPU_READ, TU_SHADER_RES);

		delete[] init_data.data;

		//RenderBuffer* render_buffer = Context::Instance().GetRenderFactory().MakeRenderBuffer(bitmap_texture_, AT_CPU_WRITE_GPU_READ, BU_SHADER_RES);
		byte* data = static_cast<byte*>(bitmap_texture_->Map(AT_CPU_WRITE_GPU_READ));
		CHECK_ASSERT(data != nullptr);
		for (int32_t i = 0; i < TextureSize.x() * TextureSize.y(); ++i)
		{
			uint32_t index = i * 4;
			data[index] = 0;
			data[index + 1] = 150;
			data[index + 2] = 0;
			data[index + 3] = 255;
		}
		bitmap_texture_->UnMap();

		ShaderObject* output_to_tex_so_ = Context::Instance().GetRenderFactory().MakeShaderObject();
		output_to_tex_so_->LoadBinaryFile("FxFiles/DebugShader.cso");
		output_to_tex_so_->SetTechnique("PPTech");

		output_to_tex_pp_ = new PostProcess();
		output_to_tex_pp_->SetPPShader(output_to_tex_so_);
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
					continue;
				}

				//RenderBuffer* render_buffer = Context::Instance().GetRenderFactory().MakeRenderBuffer(bitmap_texture_, AT_CPU_WRITE_GPU_READ, BU_SHADER_RES);
				byte* data = static_cast<byte*>(bitmap_texture_->Map(AT_CPU_WRITE_GPU_READ));
				CHECK_ASSERT(data != nullptr);

				uint32_t* new_bit = new uint32_t[face->glyph->bitmap.rows * face->glyph->bitmap.pitch];

				for (uint32_t i = 0; i < face->glyph->bitmap.rows; ++i)
				{
					for (int32_t j = 0; j < face->glyph->bitmap.pitch; ++j)
					{
						uint32_t index = i * face->glyph->bitmap.pitch + j ;
						int color = face->glyph->bitmap.buffer[index];
						byte r = color == 0 ? 0 : 255;
						byte g = color == 0 ? 0 : 255;
						byte b = color == 0 ? 0 : 255;
						byte a = 255;
						new_bit[index] = r;
					}
				}
				
				
				for (int32_t i = 0; i < TextureSize.y(); i++)
				{
					for (int32_t j = 0; j < TextureSize.x(); j ++)
					{
						uint32_t index = (i * TextureSize.x() + j) * 4;
						if (face->glyph->bitmap.pitch > j && face->glyph->bitmap.rows > (uint32_t)i)
						{
							uint32_t bitmap_index = i * face->glyph->bitmap.pitch + j;
							CHECK_ASSERT(bitmap_index < face->glyph->bitmap.pitch * face->glyph->bitmap.rows);
							int color = face->glyph->bitmap.buffer[bitmap_index];

							data[index]		= color;
							data[index + 1] = color;
							data[index + 2] = color;
							data[index + 3] = 255;
						}
						else
						{
							data[index] = 50;
							data[index + 1] = 0;
							data[index + 2] = 0;
							data[index + 3] = 255;
						}
					}
				}
				bitmap_texture_->UnMap();

				std::ofstream LogFile;
				LogFile.open("bitmap.txt", std::fstream::out);
				if (LogFile.is_open())
				{
					for (uint32_t i = 0; i < face->glyph->bitmap.rows; ++i)
					{
						for (int32_t j = 0; j < face->glyph->bitmap.pitch; ++j)
						{
							uint32_t index = i * face->glyph->bitmap.pitch + j;
							int color = face->glyph->bitmap.buffer[index];
							if (color == 0 )
							{
								LogFile << ' ';
							}
							else
							{
								LogFile << '*';
							}
						}
						LogFile << std::endl;
					}
					LogFile.close();
				}
			}
		}
	}

	void Font::DumpToScreen()
	{
		static bool inited = false;
		if (!inited)
		{
			output_to_tex_pp_->SetInput(bitmap_texture_, 0);
			output_to_tex_pp_->SetOutput(Context::Instance().GetRenderFactory().GetRenderEngine().CurrentFrameBuffer()->GetTexture(0), 0);
			inited = true;
		}
		output_to_tex_pp_->Apply();
	}

}

