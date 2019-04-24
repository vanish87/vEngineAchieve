#include "Engine/Header/RenderTools.h"
#include "Engine/Header/RenderFactory.h"

namespace vEngine
{
	typedef std::vector<VertexType> VertexList;
	typedef std::vector<int> IndexList;
	void BuildGeoSphere(int numSubdivisions, float radius, VertexList& vertices, IndexList& indices);
	void Subdivide(VertexList& vertices, IndexList& indices);

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


		vb[0].uv = float2(-1, +1);
		vb[1].uv = float2(+1, +1);
		vb[2].uv = float2(-1, -1);
		vb[3].uv = float2(+1, -1);
		vb[4].uv = float2(-1, -1);
		vb[5].uv = float2(+1, +1);


		vb[0].normal = float3(0, 0, -1);
		vb[1].normal = float3(0, 0, -1);
		vb[2].normal = float3(0, 0, -1);
		vb[3].normal = float3(0, 0, -1);
		vb[4].normal = float3(0, 0, -1);
		vb[5].normal = float3(0, 0, -1);


		vb[0].tangent = float3(0, 1, 0);
		vb[1].tangent = float3(0, 1, 0);
		vb[2].tangent = float3(0, 1, 0);
		vb[3].tangent = float3(0, 1, 0);
		vb[4].tangent = float3(0, 1, 0);
		vb[5].tangent = float3(0, 1, 0);


		vb[0].bitangent = float3(1, 0, 0);
		vb[1].bitangent = float3(1, 0, 0);
		vb[2].bitangent = float3(1, 0, 0);
		vb[3].bitangent = float3(1, 0, 0);
		vb[4].bitangent = float3(1, 0, 0);
		vb[5].bitangent = float3(1, 0, 0);

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

		inputlayout.push_back(VU_NORMAL);
		inputlayout.push_back(VU_TEXCOORD);
		inputlayout.push_back(VU_TANGENT);
		inputlayout.push_back(VU_BINORMAL);
		render_layout->SetInputLayout(inputlayout);
		float4x4 model_matrix;
		Math::Identity(model_matrix);
		return new Mesh("full screen quad", render_layout, model_matrix, vb, 6, ib);
	}
	void BuildGeoSphere(int numSubdivisions, float radius, VertexList& vertices, IndexList& indices)
	{
		numSubdivisions = Math::Min(numSubdivisions, 5);

		const float X = 0.525731f;
		const float Z = 0.850651f;

		float3 pos[12] =
		{
			float3(-X, 0.0f, Z),  float3(X, 0.0f, Z),
			float3(-X, 0.0f, -Z), float3(X, 0.0f, -Z),
			float3(0.0f, Z, X),   float3(0.0f, Z, -X),
			float3(0.0f, -Z, X),  float3(0.0f, -Z, -X),
			float3(Z, X, 0.0f),   float3(-Z, X, 0.0f),
			float3(Z, -X, 0.0f),  float3(-Z, -X, 0.0f)
		};

		uint32_t k[60] =
		{
			1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
			1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
			3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
			10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
		};

		vertices.resize(12);
		indices.resize(60);

		for (int i = 0; i < 12; ++i)
		{
			VertexType v;
			v.position = pos[i];
			vertices[i] = v;
		}

		for (int i = 0; i < 60; ++i)
			indices[i] = k[i];

		for (int i = 0; i < numSubdivisions; ++i)
			Subdivide(vertices, indices);

		for (int i = 0; i < vertices.size(); ++i)
		{
			vertices[i].position = Math::Normalize(vertices[i].position);
			vertices[i].position = vertices[i].position * radius;
		}
	}

	void Subdivide(VertexList& vertices, IndexList& indices)
	{
		VertexList vin = vertices;
		IndexList  iin = indices;

		vertices.resize(0);
		indices.resize(0);

		//       v1
		//       *
		//      / \
		//     /   \
		//  m0*-----*m1
		//   / \   / \
		//  /   \ /   \
		// *-----*-----*
		//v0    m2     v2

		int numTris = (int)iin.size() / 3;
		for (int i = 0; i < numTris; ++i)
		{
			float3 v0 = vin[iin[i * 3 + 0]].position;
			float3 v1 = vin[iin[i * 3 + 1]].position;
			float3 v2 = vin[iin[i * 3 + 2]].position;

			float3 m0 = (v0 + v1) * 0.5f;
			float3 m1 = (v1 + v2) * 0.5f;
			float3 m2 = (v0 + v2) * 0.5f;
			VertexType vt0, vt1, vt2, vtm0, vtm1, vtm2;
			vt0.position = v0;
			vt1.position = v1;
			vt2.position = v2;
			vtm0.position = m0;
			vtm1.position = m1;
			vtm2.position = m2;

			vertices.push_back(vt0); // 0
			vertices.push_back(vt1); // 1
			vertices.push_back(vt2); // 2
			vertices.push_back(vtm0); // 3
			vertices.push_back(vtm1); // 4
			vertices.push_back(vtm2); // 5


			indices.push_back(i * 6 + 0);
			indices.push_back(i * 6 + 3);
			indices.push_back(i * 6 + 5);

			indices.push_back(i * 6 + 3);
			indices.push_back(i * 6 + 4);
			indices.push_back(i * 6 + 5);

			indices.push_back(i * 6 + 5);
			indices.push_back(i * 6 + 4);
			indices.push_back(i * 6 + 2);

			indices.push_back(i * 6 + 3);
			indices.push_back(i * 6 + 1);
			indices.push_back(i * 6 + 4);
		}
	}
	Mesh* RenderTools::MakeSphereMesh()
	{
		VertexList vertices;
		IndexList indices;

		BuildGeoSphere(2, 1, vertices, indices);

		VertexType* vb = new VertexType[vertices.size()];
		uint32_t* ib = new uint32_t[indices.size()];

		for (size_t i = 0; i < vertices.size(); i++)
		{
			vb[i].position = vertices[i].position;
		}
		for (size_t i = 0; i < indices.size(); i++)
		{
			ib[i] = indices[i];
		}

		//call MakeRenderLayout
		RenderLayout* render_layout = Context::Instance().GetRenderFactory().MakeRenderLayout();
		//call MakeRenderBuffer(Vertex)
		InitData init_data;
		init_data.data = vb;
		init_data.row_pitch = 0;
		init_data.slice_pitch = 0;
		RenderBuffer* vertex_buffer = Context::Instance().GetRenderFactory().MakeRenderBuffer(init_data, AT_GPU_READ_WRITE, BU_VERTEX, (uint32_t)vertices.size(), sizeof(VertexType));
		//delete[] vb;
		//call MakeRenderBuffer(Index)
		init_data.data = ib;
		init_data.row_pitch = 0;
		init_data.slice_pitch = 0;
		RenderBuffer* index_buffer = Context::Instance().GetRenderFactory().MakeRenderBuffer(init_data, AT_GPU_READ_WRITE, BU_INDEX, (uint32_t)indices.size(), sizeof(uint32_t));
		//delete[] ib;

		//add VertexBuffer to renderlayout;
		render_layout->AddBuffer(vertex_buffer, sizeof(VertexType));
		//add IndexBuffer to renderlayout;
		render_layout->AddBuffer(index_buffer, (uint32_t)indices.size());
		//set Primitivetype of renderlayout;
		render_layout->SetPrimitive(PT_TRIANGLELIST);
		//set Input layout Semi
		std::vector<VertexUsage> inputlayout;
		inputlayout.push_back(VU_POSITION);
		render_layout->SetInputLayout(inputlayout);
		float4x4 model_matrix;
		Math::Identity(model_matrix);
		return new vEngine::Mesh("SkyDome", render_layout, model_matrix, vb, (uint32_t)vertices.size(), ib);
	}

	Mesh * RenderTools::Make2DCricleMesh(uint32_t DivisionDegree)
	{
		//make a full screen qua for lighting pass
		VertexType* vb = new VertexType[DivisionDegree*3];
		uint32_t* ib = new uint32_t[DivisionDegree*3];
		float rad = 0; 
		for (uint32_t i = 0; i < DivisionDegree * 3; i += 3)
		{
			float x = Math::Cos(rad);
			float y = Math::Sin(rad);
			float x1 = Math::Cos(rad+ (Math::PI*2) / DivisionDegree);
			float y1 = Math::Sin(rad+ (Math::PI*2) / DivisionDegree);
			//clock wise
			vb[i].position = float3(x, y, 0);
			vb[i + 1].position = float3(0, 0, 0);
			vb[i + 2].position = float3(x1, y1, 0);


			vb[i].uv = float2(x, y);
			vb[i + 1].uv = float2(0, 0);
			vb[i + 2].uv = float2(x1, y1);


			vb[i].normal = float3(0, 0, -1);
			vb[i + 1].normal = float3(0, 0, -1);
			vb[i + 2].normal = float3(0, 0, -1);


			vb[i].tangent = float3(0, 1, 0);
			vb[i + 1].tangent = float3(0, 1, 0);
			vb[i + 2].tangent = float3(0, 1, 0);


			vb[i].bitangent = float3(1, 0, 0);
			vb[i + 1].bitangent = float3(1, 0, 0);
			vb[i + 2].bitangent = float3(1, 0, 0);

			ib[i] = i;
			ib[i + 1] = i+1;
			ib[i + 2] = i+2;

			rad += (Math::PI * 2) / DivisionDegree;
		}
		
		//call MakeRenderLayout
		RenderLayout* render_layout = Context::Instance().GetRenderFactory().MakeRenderLayout();
		//call MakeRenderBuffer(Vertex)
		InitData init_data;
		init_data.data = vb;
		init_data.row_pitch = 0;
		init_data.slice_pitch = 0;
		RenderBuffer* vertex_buffer = Context::Instance().GetRenderFactory().MakeRenderBuffer(init_data, AT_GPU_READ_WRITE, BU_VERTEX, DivisionDegree * 3, sizeof(VertexType));
		delete[] vb;
		//call MakeRenderBuffer(Index)
		init_data.data = ib;
		init_data.row_pitch = 0;
		init_data.slice_pitch = 0;
		RenderBuffer* index_buffer = Context::Instance().GetRenderFactory().MakeRenderBuffer(init_data, AT_GPU_READ_WRITE, BU_INDEX, DivisionDegree * 3, sizeof(uint32_t));
		delete[] ib;

		//add VertexBuffer to renderlayout;
		render_layout->AddBuffer(vertex_buffer, sizeof(VertexType));
		//add IndexBuffer to renderlayout;
		render_layout->AddBuffer(index_buffer, DivisionDegree * 3);
		//set Primitivetype of renderlayout;
		render_layout->SetPrimitive(PT_TRIANGLELIST);
		//set Input layout Semi
		std::vector<VertexUsage> inputlayout;
		inputlayout.push_back(VU_POSITION);

		inputlayout.push_back(VU_NORMAL);
		inputlayout.push_back(VU_TEXCOORD);
		inputlayout.push_back(VU_TANGENT);
		inputlayout.push_back(VU_BINORMAL);
		render_layout->SetInputLayout(inputlayout);
		float4x4 model_matrix;
		Math::Identity(model_matrix);
		return new Mesh("2D Cricle Mesh", render_layout, model_matrix, vb, 6, ib);
	}

}
