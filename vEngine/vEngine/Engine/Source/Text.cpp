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

	static byte* data;
	
	Text::Text()
		:SceneObject(&TextElement)
	{

	}

	void Text::Draw(std::wstring text)
	{
		Font::GetFontByName("msyh").DrawD3DText(text, vEngine::int2(50, 50));
	}
		
}

