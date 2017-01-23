#include "D3DSkyDome.h"

#include "D3DRenderEngine.h"
#include "D3DTexture.h"
#include "DirectXTex.h"

namespace vEngine
{

	D3DSkyDome::D3DSkyDome(std::string file_name )
	{
		cube_texture_ = LoadTexture(file_name);

		VertexList vertices;
		IndexList indices;

		BuildGeoSphere(2, 900.0f , vertices ,indices);

		VertexType* vb = new VertexType[vertices.size()];
		uint32_t* ib = new uint32_t[indices.size()];

		for(size_t i = 0; i < vertices.size(); i++)
		{
			vb[i].position  = vertices[i].position;
		}
		for(size_t i = 0; i < indices.size(); i++)
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
		RenderBuffer* vertex_buffer = Context::Instance().GetRenderFactory().MakeRenderBuffer(init_data, AT_GPU_READ_WRITE, BU_VERTEX, (uint32_t)vertices.size() ,sizeof(VertexType));
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
		sky_mesh_ = new vEngine::Mesh("SkyDome", render_layout, model_matrix, vb, (uint32_t)vertices.size(), ib);

		model_matrix_ = model_matrix;
	}


	D3DSkyDome::~D3DSkyDome(void)
	{
	}

	Texture* D3DSkyDome::LoadTexture( std::string file_name )
	{
		if(file_name.empty())return 0;
		//only for load d3d Texture
		//if I have a original texture file loader, remove it, do Texture loading on Model Class
		D3DRenderEngine* d3d_re = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());	
		ID3D11Resource* texture;
		std::wstring widestr = std::wstring(file_name.begin(), file_name.end());
		DirectX::TexMetadata metadata;
		DirectX::ScratchImage image;
		DirectX::LoadFromDDSFile(widestr.c_str(), NULL, &metadata, image);
		HRESULT result = DirectX::CreateTexture(d3d_re->D3DDevice(), image.GetImages(), image.GetImageCount(), metadata, &texture);
		if(FAILED(result))
			PRINT("Cannot Load Texture File");

		D3D11_RESOURCE_DIMENSION dimension;
		texture->GetType(&dimension);
		assert(dimension == D3D11_RESOURCE_DIMENSION_TEXTURE2D);

		Texture* d3d_tex = Context::Instance().GetRenderFactory().MakeTexture2D(texture);

		return d3d_tex;
	}

	void D3DSkyDome::LoadShaderFile( std::string file_name )
	{
		shader_object_ = new D3DShaderobject();
		D3DShaderobject* d3d_shader_object = static_cast<D3DShaderobject*>(shader_object_);
		d3d_shader_object->LoadFxoFile(file_name);

		//Default init for SkyDome shader
		d3d_shader_object->SetTechnique("SkyDomeTech");

		d3d_shader_object->SetShaderResourceVariable("background_tex");
		d3d_shader_object->SetMatrixVariable("g_world_matrix");
		d3d_shader_object->SetMatrixVariable("g_view_proj_matrix");
		d3d_shader_object->SetMatrixVariable("g_view_matrix");
		d3d_shader_object->SetMatrixVariable("g_inv_proj_matrix");
		d3d_shader_object->SetMatrixVariable("g_inv_view_matrix");
		d3d_shader_object->SetMatrixVariable("g_model_matrix");
		
		d3d_shader_object->SetVectorVariable("g_eye_pos");

		sky_mesh_->SetShaderObject(d3d_shader_object);
	}

	void D3DSkyDome::Render( int pass_index )
	{
		sky_mesh_->SetRenderParameters();
		sky_mesh_->Render(pass_index);
		sky_mesh_->EndRender();
	}

	void D3DSkyDome::SetRenderParameters()
	{

		D3DRenderEngine* d3d_re = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());	
		D3DShaderobject* d3d_shader_object = static_cast<D3DShaderobject*>(shader_object_);
		float3 cam_pos = d3d_re->CurrentFrameBuffer()->GetViewport().GetCamera().GetPos();
		Math::Translate(model_matrix_, cam_pos.x(), cam_pos.y(), cam_pos.z());
		d3d_shader_object->SetMatrixVariable("g_world_matrix", model_matrix_);
		d3d_re->TrunoffCull();
		RenderBuffer* cude_srv = Context::Instance().GetRenderFactory().MakeRenderBuffer(cube_texture_, AT_GPU_READ_WRITE, BU_SHADER_RES); 
		shader_object_->SetReource("background_tex", cude_srv , 1);
		//throw std::exception("The method or operation is not implemented.");
	}

	void D3DSkyDome::EndRender()
	{
		D3DRenderEngine* d3d_re = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());	
		d3d_re->SetNormalState();

		//throw std::exception("The method or operation is not implemented.");
	}


	void D3DSkyDome::BuildGeoSphere(int numSubdivisions, float radius, VertexList& vertices, IndexList& indices)
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

		for(int i = 0; i < 12; ++i)
		{
			VertexType v;
			v.position = pos[i];
			vertices[i] = v;
		}

		for(int i = 0; i < 60; ++i)
			indices[i] = k[i];

		for(int i = 0; i < numSubdivisions; ++i)
			Subdivide(vertices, indices);

		//投影顶点到球面上，然后缩放顶点到球心的距离
		for(int i = 0; i < vertices.size(); ++i)
		{
			vertices[i].position = Math::Normalize(vertices[i].position);
			vertices[i].position = vertices[i].position * radius;
		}
	}

	void D3DSkyDome::Subdivide( VertexList& vertices, IndexList& indices )
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
		// v0    m2     v2

		int numTris = (int)iin.size()/3;
		for(int i = 0; i < numTris; ++i)
		{
			float3 v0 = vin[ iin[i*3+0] ].position;
			float3 v1 = vin[ iin[i*3+1] ].position;
			float3 v2 = vin[ iin[i*3+2] ].position;

			float3 m0 = (v0 + v1) * 0.5;
			float3 m1 = (v1 + v2) * 0.5;
			float3 m2 = (v0 + v2) * 0.5;
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

			//索引出四个三角形
			indices.push_back(i*6+0);
			indices.push_back(i*6+3);
			indices.push_back(i*6+5);

			indices.push_back(i*6+3);
			indices.push_back(i*6+4);
			indices.push_back(i*6+5);

			indices.push_back(i*6+5);
			indices.push_back(i*6+4);
			indices.push_back(i*6+2);

			indices.push_back(i*6+3);
			indices.push_back(i*6+1);
			indices.push_back(i*6+4);
		}
	}



}
