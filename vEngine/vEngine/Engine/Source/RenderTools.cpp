#include "Engine\Header\RenderTools.h"
#include "Engine\Header\RenderFactory.h"

namespace vEngine
{	
	Mesh* RenderTools::MakeFullScreenMesh()
	{
		//make a full screen qua for lighting pass
		VertexType* vb = new VertexType[6];
		uint32_t* ib = new uint32_t[6];
		//clock wise
		vb[0].position = float3(-1, +1, 1);
		vb[1].position = float3(+1, +1, 1);
		vb[2].position = float3(-1, -1, 1);
		vb[3].position = float3(+1, -1, 1);
		vb[4].position = float3(-1, -1, 1);
		vb[5].position = float3(+1, +1, 1);

		ib[0] = 0;
		ib[1] = 1;
		ib[2] = 2;
		ib[3] = 3;
		ib[4] = 4;
		ib[5] = 5;

		//call MakeRenderLayout
		RenderLayout* render_layout = Context::Instance().GetRenderFactory().MakeRenderLayout();
		//call MakeRenderBuffer(Vertex)
		InitData init_data;
		init_data.data = vb;
		init_data.row_pitch = 0;
		init_data.slice_pitch = 0;
		RenderBuffer* vertex_buffer = Context::Instance().GetRenderFactory().MakeRenderBuffer(init_data, AT_GPU_READ_WRITE, BU_VERTEX, 6, sizeof(VertexType));
		delete[] vb;
		//call MakeRenderBuffer(Index)
		init_data.data = ib;
		init_data.row_pitch = 0;
		init_data.slice_pitch = 0;
		RenderBuffer* index_buffer = Context::Instance().GetRenderFactory().MakeRenderBuffer(init_data, AT_GPU_READ_WRITE, BU_INDEX, 6, sizeof(uint32_t));
		delete[] ib;

		//add VertexBuffer to renderlayout;
		render_layout->AddBuffer(vertex_buffer, sizeof(VertexType));
		//add IndexBuffer to renderlayout;
		render_layout->AddBuffer(index_buffer, 6);
		//set Primitivetype of renderlayout;
		render_layout->SetPrimitive(PT_TRIANGLELIST);
		//set Input layout Semi
		std::vector<VertexUsage> inputlayout;
		inputlayout.push_back(VU_POSITION);
		render_layout->SetInputLayout(inputlayout);
		float4x4 model_matrix;
		Math::Identity(model_matrix);
		return new Mesh("full screen quad", render_layout, model_matrix, vb, 6, ib);
	}


}
