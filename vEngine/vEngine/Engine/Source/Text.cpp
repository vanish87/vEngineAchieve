#include "Engine\Header\Text.h"
#include "Engine\Header\Context.h"
#include "Engine\Header\texture.h"
#include "Engine\Header\Font.h"

namespace vEngine
{
	static TextRenderElement TextElement;
	static Texture* BitmapTexture = nullptr;
	static int2 TextureSize(1280, 800);

	TextRenderElement::TextRenderElement()
	{

	}

	RenderLayout* TextRenderElement::GetRenderLayout()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void TextRenderElement::SetRenderParameters()
	{
		if (this->shader_object_ == nullptr)
		{
			this->shader_object_ = Context::Instance().GetRenderFactory().MakeShaderObject();
			this->shader_object_->LoadBinaryFile("FxFiles/DebugShader.cso");
		}
		//throw std::logic_error("The method or operation is not implemented.");
	}

	void TextRenderElement::Render(int pass_index)
	{
		if (pass_index == 3)
		{

		}
		//throw std::logic_error("The method or operation is not implemented.");
	}

	void TextRenderElement::EndRender()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}
	
	Text::Text()
		:SceneObject(&TextElement)
	{

	}

	void Text::Draw(std::wstring text)
	{
		if (BitmapTexture == nullptr)
		{
			InitData init_data;
			init_data.data = new uint32_t[TextureSize.x() * TextureSize.y()];
			init_data.row_pitch = sizeof(uint32_t) * TextureSize.x();
			init_data.slice_pitch = 0;

			BitmapTexture = Context::Instance().GetRenderFactory().MakeTexture2D(&init_data, TextureSize.x(), TextureSize.y(), 1, 1,
				R8G8B8A8_U, 1, 0, AT_CPU_WRITE_GPU_READ, TU_SHADER_RES);

			delete[] init_data.data;
		}

		Font::GetFontByName("msyh").DrawD3DText(text, vEngine::int2(50, 50), BitmapTexture);
	}

	Texture* Text::GetUITexture()
	{
		return BitmapTexture;
	}

}

