#include "Engine\Header\Font.h"
#include "Engine\Header\Context.h"

#include "Engine\Header\PostProcess.h"
#include "D3D11\D3DShaderobject.h"

namespace vEngine
{
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

		error = FT_Set_Pixel_Sizes(face, 227, 128);
		//error = FT_Set_Char_Size(face, 512, 512);
		CHECK_ASSERT(error == FT_Err_Ok);

		InitData init_data;
		init_data.data = new uint32_t[227 * 128];
		init_data.row_pitch = sizeof(uint32_t) * 227;
		init_data.slice_pitch = 0;

		this->bitmap_texture_ = Context::Instance().GetRenderFactory().MakeTexture2D(&init_data, 227, 128, 1, 1,
			R8G8B8A8_U, 1, 0, AT_CPU_WRITE_GPU_READ, TU_SHADER_RES);

		delete[] init_data.data;

		RenderBuffer* render_buffer = Context::Instance().GetRenderFactory().MakeRenderBuffer(bitmap_texture_, AT_CPU_WRITE_GPU_READ, BU_SHADER_RES);
		byte* data = static_cast<byte*>(render_buffer->Map(AT_CPU_WRITE_GPU_READ));
		CHECK_ASSERT(data != nullptr);
		for (uint32_t i = 0; i < 227 * 128; ++i)
		{
			uint32_t index = i * 4;
			data[index] = 50;
			data[index + 1] = 50;
			data[index + 2] = 50;
			data[index + 3] = 255;
		}
		render_buffer->UnMap();

		D3DShaderobject* output_to_tex_so_ = new D3DShaderobject();
		output_to_tex_so_->LoadFxoFile("FxFiles/DebugShader.cso");
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
				RenderBuffer* render_buffer = Context::Instance().GetRenderFactory().MakeRenderBuffer(bitmap_texture_, AT_CPU_WRITE_GPU_READ, BU_SHADER_RES);
				byte* data = static_cast<byte*>(render_buffer->Map(AT_CPU_WRITE_GPU_READ));
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
				
				
				for (uint32_t i = 0; i < 128; i++)
				{
					for (int32_t j = 0; j < 256; j ++)
					{
						uint32_t index = (i * 256 + j) * 4;
						if (face->glyph->bitmap.pitch > j && face->glyph->bitmap.rows > i)
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
				render_buffer->UnMap();

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
		output_to_tex_pp_->SetInput(bitmap_texture_, 0);
		output_to_tex_pp_->SetOutput(Context::Instance().GetRenderFactory().GetRenderEngine().CurrentFrameBuffer()->GetRenderView(0)->GetTexture(), 0);
		output_to_tex_pp_->Apply();
	}

}

