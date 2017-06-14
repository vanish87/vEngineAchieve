#ifndef MODEL_H_
#define MODEL_H_

#pragma once

#include <vector>

#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Common/Header/CommonPreDec.h"

#include "Engine/Header/RenderElement.h"
#include "Engine/Header/RenderLayout.h"
#include "Engine/Header/TypeAndFormat.h"
#include "Engine/Header/Mesh.h"
#include "Engine/Header/Texture.h"

/*

#ifdef _DEBUG
#define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
#define DEBUG_CLIENTBLOCK
#endif
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif
*/

namespace vEngine
{
	class Model : public RenderElement
	{
		
	public:
		Model(void);
		~Model(void);

		DEBUG_CLASS_NAME("Model")
		//virtual void AddToScene();

		virtual RenderLayout* GetRenderLayout();

		virtual void SetRenderParameters(const float4x4& parent);
		virtual void Render(int pass_index);
		virtual void EndRender();

		virtual void LoadFile(std::string file_name, CompleteCallBack callback = nullptr);
		virtual void Load();
		void AddMesh( Mesh* mesh );
		void AddMaterial( Material* mat );
		virtual void LoadShaderFile(std::string file_name);

		virtual Texture* LoadTexture(std::string file_name) { return nullptr; };

		void POM(bool enable);
		virtual void LoadPomTexture(std::string file_name_) {};

		std::vector<Mesh*> GetMesh(){return meshes_;};
		void ProcessMeshes( const aiNode* const & node , aiMatrix4x4 parent_mat);
		void GenerateNormals( const aiMesh* const & mesh );
		void GenerateTangentsAndBitangents( const aiMesh* const & mesh );
		void GenerateTexCoord( const aiMesh* const & mesh );

		void ProcessMaterials(aiMaterial** const mMaterials );
	protected:
		std::vector<Mesh*> meshes_;
		std::vector<Material*> materials_;
		std::vector<Texture*> textures_;
		//std::vector<RenderBuffer*> tex_srvs_;

		const aiScene* ai_scene_;

		bool pom_enabled_;
		Texture* pom_texture_;
		//RenderBuffer* pom_srv_;

		std::string file_name_;
	};
}

#endif

