#ifndef RENDERLAYOUT_H_
#define RENDERLAYOUT_H_

#pragma once
#include <vector>

#include "EnginePreDec.h"
#include "TypeAndFormat.h"
#include "RenderBuffer.h"

namespace MocapGE
{
	class RenderLayout
	{
	public:
		RenderLayout(void);
		~RenderLayout(void);

		virtual RenderLayout* GetLayout() = 0;
		//vertex size for VBU_VERTEX
		//stores index count for VBU_INDEX
		virtual void AddBuffer(RenderBuffer* render_buffer, uint32_t vertex_size);
		void SetPrimitive( PrimitiveType primitive_type );
		void SetInputLayout( std::vector<VertexUsage> input_layout );

		RenderBuffer* GetBuffer(VertexBufferUsage usage);
		std::vector<VertexUsage> GetInputLayout();
		uint32_t GetVertexSize();
		uint32_t GetIndexCount();

		PrimitiveType GetPrimitive(){return type_;};

	protected:
		struct LayoutInstance
		{
			RenderBuffer* buffer;
			VertexBufferUsage usage;
			//stores vertex size for VBU_VERTEX
			//stores index count for VBU_INDEX
			uint32_t vertex_size;
		};
		std::vector<LayoutInstance*> data_;
		PrimitiveType type_;
		std::vector<VertexUsage> input_layout_;
	};
}

#endif

