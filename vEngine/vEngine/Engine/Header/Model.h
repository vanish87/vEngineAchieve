#ifndef MODEL_H_
#define MODEL_H_

#pragma once

#include <vector>

// #include <dae.h>
// #include <dom.h>
// #include <dom/domCOLLADA.h>
// #pragma comment(lib, "libcollada14dom22-d.lib")

#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Common\Header\CommonPreDec.h"

#include "Engine\Header\RenderElement.h"
#include "RenderLayout.h"
#include "TypeAndFormat.h"
#include "Mesh.h"
#include "Engine\Header\Texture.h"


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

namespace vEngine
{
	class Model : public RenderElement
	{
		
	public:
		Model(void);
		~Model(void);

		//virtual void AddToScene();

		virtual RenderLayout* GetRenderLayout();

		virtual void Render(int pass_index);
		virtual void SetRenderParameters();
		virtual void EndRender();

		virtual void LoadFile(std::string file_name);
		void AddMesh( Mesh* mesh );
		void AddMaterial( Material* mat );
		virtual void LoadShaderFile(std::string file_name);

		virtual Texture* LoadTexture(std::string file_name) = 0;

		void POM(bool enable);
		virtual void LoadPomTexture(std::string file_name_) = 0;

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
		std::vector<RenderBuffer*> tex_srvs_;

		const aiScene* ai_scene_;

		bool pom_enabled_;
		Texture* pom_texture_;
		RenderBuffer* pom_srv_;

// 		float4x4 processMatrix(daeElement* node);
// 		void processSource( DaeMesh* mesh, daeElement* sources );
// 		void processTriangles( DaeMesh* mesh, daeElement* triangles );
// 		Material* ProcessMaterial(DaeMesh* mesh, daeElement* material);
// 		daeElement* Model::GetSampler(daeElement* param);


	private:
		//Dae loader
		/*daeDatabase* db;*/
	};
}

#endif

