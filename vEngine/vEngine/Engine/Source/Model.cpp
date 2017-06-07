#include "Engine\Header\Model.h"
#include "Engine\Header\Context.h"
#include "Engine\Header\ResourceLoader.h"
#include <assimp/Importer.hpp>

namespace vEngine
{
	Model::Model(void)
		:pom_enabled_(false), pom_texture_(nullptr)
	{
	}


	Model::~Model(void)
	{
	}

	// 	void Model::AddToScene()
	// 	{
	// 		Context::Instance().GetSceneManager().AddSceneObject(new SceneObject(this));
	// 	}

	RenderLayout* Model::GetRenderLayout()
	{
		throw std::exception("The method or operation is not implemented.");
	}

	void Model::Render(int pass_index)
	{
		//render each mesh
		throw std::exception("The method or operation is not implemented.");
	}

	void Model::SetRenderParameters()
	{
		//only do the general setup
		throw std::exception("The method or operation is not implemented.");
	}

	void Model::EndRender()
	{
		throw std::exception("The method or operation is not implemented.");
	}

	void Model::Load()
	{
		Assimp::Importer importer;
		unsigned int flags = aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate |
			aiProcess_ConvertToLeftHanded |
			aiProcess_ImproveCacheLocality | aiProcess_OptimizeGraph |
			aiProcess_OptimizeMeshes | aiProcess_JoinIdenticalVertices |
			aiProcess_RemoveRedundantMaterials;
		ai_scene_ = importer.ReadFile(file_name_, flags);
		if (!ai_scene_)
		{
			PRINT_ERROR(importer.GetErrorString());
			return;
		}

		if (ai_scene_->HasMeshes())
		{
			aiMatrix4x4 id;
			ProcessMeshes(ai_scene_->mRootNode, id);
		}
		if (ai_scene_->HasMaterials())
		{
			ProcessMaterials(ai_scene_->mMaterials);
		}

		Math::Identity(this->local_matrix_);
	}

	void Model::LoadFile( std::string file_name, CompleteCallBack callback /*= nullptr*/ )
	{
		this->file_name_ = file_name;
		ResourceLoader::GetInstance().AddAsync(this, callback);
	}

	void Model::AddMesh( Mesh* mesh )
	{
		meshes_.push_back(mesh);
	}
	void Model::AddMaterial( Material* mat )
	{
		materials_.push_back(mat);
	}

	void Model::LoadShaderFile( std::string file_name )
	{

	}	

	void Model::POM( bool enable )
	{
		pom_enabled_ = enable;
	}

	void Model::ProcessMeshes(const aiNode* const & node , aiMatrix4x4 parent_mat)
	{

		aiMatrix4x4 mat = parent_mat * node->mTransformation;
		for(size_t i =0; i < node->mNumMeshes; ++i)
		{
			const aiMesh* mesh = ai_scene_->mMeshes[node->mMeshes[i]];
			CHECK_ASSERT(mesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE);

			if(!mesh->HasPositions())continue;

			bool hasNormals = mesh->HasNormals();
			bool hasTangents = mesh->HasTangentsAndBitangents();
			bool hasTexCoords = mesh->HasTextureCoords(0);

			if(!hasNormals)
				GenerateNormals(mesh);
			if(!hasTangents)
				GenerateTangentsAndBitangents(mesh);
			if(!hasTexCoords)
				GenerateTexCoord(mesh);

			uint32_t v_size = mesh->mNumVertices;
			uint32_t i_size = mesh->mNumFaces * 3;
			VertexType* vb = new VertexType[v_size];
			uint32_t* ib = new uint32_t[i_size];

			for(size_t j = 0; j< v_size; ++j)
			{
				aiVector3D pos = mesh->mVertices[j];
				aiVector3D normal = mesh->mNormals[j];
				aiVector3D tex = hasTexCoords?mesh->mTextureCoords[0][j]: aiVector3D(0,0,0);
				aiVector3D tangent = hasTangents? mesh->mTangents[j]:aiVector3D(0,0,0);
				aiVector3D bitangent = hasTangents? mesh->mBitangents[j]:aiVector3D(0,0,0);

				vb[j].position.x()  =  pos.x;		 vb[j].position.y()  = pos.y;		vb[j].position.z()	= pos.z;
				vb[j].normal.x()    =  normal.x;	 vb[j].normal.y()	 = normal.y;	vb[j].normal.z()	= normal.z;
				vb[j].uv.x()        = tex.x;		 vb[j].uv.y()		 = tex.y;
				vb[j].tangent.x()   = tangent.x;	 vb[j].tangent.y()	 = tangent.y;	vb[j].tangent.z()	= tangent.z;
				vb[j].bitangent.x() = bitangent.x;	 vb[j].bitangent.y() = bitangent.y; vb[j].bitangent.z() = bitangent.z;

			}
			size_t count = 0;
			for (size_t j = 0; j < mesh->mNumFaces; ++j){
				for (size_t k = 0; k < mesh->mFaces[j].mNumIndices; ++k)
				{			
					 ib[count++]=mesh->mFaces[j].mIndices[k];
				}
			}

			//call MakeRenderLayout
			RenderLayout* render_layout = Context::Instance().GetRenderFactory().MakeRenderLayout();
			//call MakeRenderBuffer(Vertex)
			InitData init_data;
			init_data.data = vb;
			init_data.row_pitch = 0;
			init_data.slice_pitch = 0;
			RenderBuffer* vertex_buffer = Context::Instance().GetRenderFactory().MakeRenderBuffer(init_data, AT_GPU_READ_WRITE, BU_VERTEX, v_size ,sizeof(VertexType));
			//delete[] vb;
			//call MakeRenderBuffer(Index)
			init_data.data = ib;
			init_data.row_pitch = 0;
			init_data.slice_pitch = 0;
			RenderBuffer* index_buffer = Context::Instance().GetRenderFactory().MakeRenderBuffer(init_data, AT_GPU_READ_WRITE, BU_INDEX, i_size, sizeof(uint32_t));
			//delete[] ib;
			//add VertexBuffer to renderlayout;
			render_layout->AddBuffer(vertex_buffer, sizeof(VertexType));
			//add IndexBuffer to renderlayout;
			render_layout->AddBuffer(index_buffer, i_size);
			//set Primitivetype of renderlayout;
			render_layout->SetPrimitive(PT_TRIANGLELIST);
			//set Input layout Semi
			std::vector<VertexUsage> inputlayout;
			inputlayout.push_back(VU_POSITION);
			//inputlayout.push_back(VU_COLOR);
			inputlayout.push_back(VU_NORMAL);
			inputlayout.push_back(VU_TEXCOORD);
			inputlayout.push_back(VU_TANGENT);
			inputlayout.push_back(VU_BINORMAL);
			render_layout->SetInputLayout(inputlayout);
			std::string name = node->mName.C_Str();
			name += " ";
			name += mesh->mName.C_Str();
			float4x4 my_mat;
			my_mat[0][0] = mat[0][0];my_mat[0][1] = mat[0][1];my_mat[0][2] = mat[0][2];my_mat[0][3] = mat[0][3];
			my_mat[1][0] = mat[1][0];my_mat[1][1] = mat[1][1];my_mat[1][2] = mat[1][2];my_mat[1][3] = mat[1][3];
			my_mat[2][0] = mat[2][0];my_mat[2][1] = mat[2][1];my_mat[2][2] = mat[2][2];my_mat[2][3] = mat[2][3];
			my_mat[3][0] = mat[3][0];my_mat[3][1] = mat[3][1];my_mat[3][2] = mat[3][2];my_mat[3][3] = mat[3][3];
			my_mat = Math::Transpose(my_mat);
			PRINT(name << ": Vertex count = " <<v_size);
			this->AddMesh(new vEngine::Mesh(name, render_layout, my_mat, vb, v_size, ib, mesh->mMaterialIndex));
		}

		for(size_t i = 0; i < node->mNumChildren; ++i)
			ProcessMeshes(node->mChildren[i], mat);
	}

	void Model::GenerateNormals( const aiMesh* const & mesh )
	{
		//throw std::exception("The method or operation is not implemented.");
	}

	void Model::GenerateTangentsAndBitangents( const aiMesh* const & mesh )
	{
		//throw std::exception("The method or operation is not implemented.");
	}

	void Model::GenerateTexCoord( const aiMesh* const & mesh )
	{
		//throw std::exception("The method or operation is not implemented.");
	}

	void Model::ProcessMaterials(aiMaterial** const materials )
	{
		for(size_t i = 0;i < ai_scene_->mNumMaterials; ++i)
		{
			Material* mat = new Material();
			const aiMaterial& ai_mat = *materials[i];

			aiColor4D color;
			if(AI_SUCCESS == ai_mat.Get(AI_MATKEY_COLOR_DIFFUSE, color))
			{
				mat->diffuse.x() = color.r;
				mat->diffuse.y() = color.g;
				mat->diffuse.z() = color.b;
				mat->diffuse.w() = color.a;
			}
			else
			{
				PRINT("no diffuse color");
			}

			if(AI_SUCCESS == ai_mat.Get(AI_MATKEY_COLOR_SPECULAR, color))
			{
				mat->specular.x() = color.r;
				mat->specular.y() = color.g;
				mat->specular.z() = color.b;
				mat->specular.w() = color.a;
			}
			else
			{
				PRINT("no spacular color");
			}

			if(AI_SUCCESS == ai_mat.Get(AI_MATKEY_COLOR_AMBIENT, color))
			{
				mat->ambient.x() = color.r;
				mat->ambient.y() = color.g;
				mat->ambient.z() = color.b;
				mat->ambient.w() = color.a;
			}
			else
			{
				PRINT("no ambient color");
			}

			if(AI_SUCCESS != ai_mat.Get(AI_MATKEY_SHININESS, mat->shininess))
			{
				PRINT("no shininess");
			}
			
			aiString szPath;
			if(AI_SUCCESS == ai_mat.Get(AI_MATKEY_TEXTURE_DIFFUSE(0), szPath))
			{
				Texture *tex = LoadTexture(szPath.C_Str());
				if(tex != nullptr)
				{
					textures_.push_back(tex);
					//tex_srvs_.push_back(Context::Instance().GetRenderFactory().MakeRenderBuffer(textures_.back(), AT_GPU_READ_WRITE,BU_SHADER_RES));
					//index 0 reserved for null
					mat->diffuse_tex = (uint32_t)textures_.size();
				}
			}
			else
			{
				PRINT("no difusse texture");
			}

			if(AI_SUCCESS == ai_mat.Get(AI_MATKEY_TEXTURE_NORMALS(0), szPath))
			{

				Texture *tex = LoadTexture(szPath.C_Str());
				//tex = LoadTexture(szPath.C_Str());
				if(tex != nullptr)
				{
					textures_.push_back(tex);
					//tex_srvs_.push_back(Context::Instance().GetRenderFactory().MakeRenderBuffer(textures_.back(), AT_GPU_READ_WRITE,BU_SHADER_RES));
					//index 0 reserved for null
					mat->normalmap_tex = (uint32_t)textures_.size();
				}
			}
			else
			{
				PRINT("no normal texture");
			}
			
			materials_.push_back(mat);
			
		}
	}

}