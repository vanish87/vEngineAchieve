#include "Engine\Header\RenderLayout.h"

namespace vEngine
{

	RenderLayout::RenderLayout(void)
	{
	}


	RenderLayout::~RenderLayout(void)
	{
	}

	void RenderLayout::AddBuffer( RenderBuffer* render_buffer ,uint32_t vertex_size)
	{
		LayoutInstance* li = new LayoutInstance();
		li->buffer = render_buffer;
		BufferUsage bu = render_buffer->GetUsage();
		switch (bu)
		{
		case BU_VERTEX:
			li->usage = VBU_VERTEX;
			li->vertex_size = vertex_size;
			break;
		case BU_INDEX:
			li->usage = VBU_INDEX;
			li->vertex_size = vertex_size;
			break;
			default:
				NULL;
				break;
		}
		data_.push_back(li);
	}

	void RenderLayout::SetPrimitive( PrimitiveType primitive_type )
	{
		type_ = primitive_type;
	}

	void RenderLayout::SetInputLayout( std::vector<VertexUsage> input_layout )
	{
		input_layout_ = input_layout;
	}

	RenderBuffer* RenderLayout::GetBuffer( VertexBufferUsage usage )
	{
		for(size_t i =0; i < data_.size(); i++)
			if(data_[i]->usage == usage)
				return data_[i]->buffer;
		return nullptr;
	}

	std::vector<VertexUsage> RenderLayout::GetInputLayout()
	{
		return input_layout_;
	}

	uint32_t RenderLayout::GetVertexSize()
	{
		for(size_t i =0; i < data_.size(); i++)
			if(data_[i]->usage == VBU_VERTEX)
				return data_[i]->vertex_size;
		return 0;
	}

	uint32_t RenderLayout::GetIndexCount()
	{
		for(size_t i =0; i < data_.size(); i++)
			if(data_[i]->usage == VBU_INDEX)
				return data_[i]->vertex_size;
		return 0;
	}

}
