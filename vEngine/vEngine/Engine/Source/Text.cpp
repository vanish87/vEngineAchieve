#include "Engine/Header/Text.h"
#include "Engine/Header/Context.h"
#include "Engine/Header/texture.h"
#include "Engine/Header/Font.h"

namespace vEngine
{
	static TextRenderElement TextElement;
	static Texture* BitmapTexture = nullptr;
	static int2 TextureSize(1280, 800);
	static std::mutex DRAW_MUTEX;

	TextRenderElement::TextRenderElement()
	{

	}

	RenderLayout* TextRenderElement::GetRenderLayout()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void TextRenderElement::SetRenderParameters(const float4x4& parent)
	{
		if (this->shader_object_ == nullptr)
		{
			//this->shader_object_ = Context::Instance().GetRenderFactory().MakeShaderObject();
			//this->shader_object_->LoadBinaryFile("FxFiles/DebugShader.cso");
			this->shader_object_ = ShaderObject::FindShaderByName("DebugShader");
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

	Text::Text(std::wstring text_to_display)
		:Text()
	{
		contents_ = text_to_display;
		text_field_ = int4(0, 0, 100, 100);
		font_name_ = "msyh";
	}

	void Text::Draw()
	{
		std::unique_lock<std::mutex> lk(DRAW_MUTEX);
		Font::GetFontByName(this->font_name_).DrawD3DText(this->contents_, int2(this->text_field_.x(), this->text_field_.y()));
	}
	void Text::Draw(const std::wstring& new_text)
	{
		this->contents_ = new_text;
		this->Draw();
	}
		
	void Text::SetRect(const int4& rect)
	{
		this->text_field_ = rect;
	}

	void Text::SetContent(std::wstring new_text)
	{
		this->contents_ = new_text;
	}

}

