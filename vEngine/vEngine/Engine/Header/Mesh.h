#ifndef MESH_H_
#define MESH_H_

#pragma once

#include "Common\Header\CommonPreDec.h"
#include "Engine\Header\RenderElement.h"
#include "TypeAndFormat.h"

namespace vEngine
{
	class Mesh :public RenderElement
	{
	public:
		Mesh(void);
		~Mesh(void); 
		
		DEBUG_CLASS_FILE_NAME

		Mesh(std::string name, RenderLayout* render_layout, float4x4 model_matrix, VertexType* vb,uint32_t vsize, uint32_t* ib, uint8_t mat_id = NULL)
			:RenderElement(model_matrix), name_(name), render_layout_(render_layout),
			material_id_(mat_id),
			vb_(vb), vsize_(vsize), ib_(ib)
		{};

		//virtual void AddToScene();

		virtual RenderLayout* GetRenderLayout();

		virtual void SetRenderParameters();

		virtual void Render(int pass_index);

		virtual void EndRender();

		VertexType* GetVertex(){return vb_;};
		uint32_t* GetIndex(){return ib_;};
		uint32_t GetVSize();
		uint8_t GetMaterialID();


	protected:

		std::string name_;
		RenderLayout* render_layout_;
		uint8_t material_id_;

		//stores vertex in cpu for obj management
		VertexType* vb_;
		uint32_t vsize_;
		uint32_t* ib_;
	};

}

#endif
