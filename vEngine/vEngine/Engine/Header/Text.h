#ifndef TEXT_H_
#define TEXT_H_

#pragma once
#include <string>
#include "Common/Header/CommonPreDec.h"
#include "Engine/Header/RenderElement.h"
#include "Engine/Header/SceneObject.h"

namespace vEngine
{
	//TODO: next step for text rendering is generate a texture of all texts
	//then use vertex and texture coordinate to render string with this texture
	class TextRenderElement : public RenderElement
	{

	public:
		TextRenderElement();

		DEBUG_CLASS_FILE_NAME

		virtual RenderLayout* GetRenderLayout() override;
		virtual void SetRenderParameters(const float4x4& parent) override;
		virtual void Render(int pass_index) override;
		virtual void EndRender() override;
	};

	class Text : public SceneObject
	{
	public:
		Text(std::wstring text_to_display);

		//this is a temporary solution for text rendering
		//it renders text to a full screen texture, then this texture will be blended with final frame buffer
		//see TextRenderElement for better solution
		void Draw();
		void Draw(const std::wstring& new_text);
		void SetRect(const int4& rect);
		void SetContent(std::wstring new_text);

	private:
		Text();
	protected:
		std::string font_name_;
		int4 text_field_;
		std::wstring contents_;
	};
}

#endif