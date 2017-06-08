#include "D3DModel.h"
#include "DirectXTex.h"
#include "D3DRenderEngine.h"

namespace vEngine
{
	D3DModel::D3DModel(void)
	{
	} 


	D3DModel::~D3DModel(void)
	{
	}
	
	
	Texture* D3DModel::LoadTexture( std::string file_name )
	{
		if(file_name.empty())return 0;
		//only for load d3d Texture
		//if I have a original texture file loader, remove it, do Texture loading on Model Class
		D3DRenderEngine* d3d_re = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());	
		ID3D11Resource* texture;
		//TODO: use a resource loader for search path
		std::ifstream ifs(file_name);
		if (!ifs.good())
		{
			std::ifstream ifs1("Media/textures/" + file_name);
			if (ifs1.good())
			{
				file_name = "Media/textures/" + file_name;
			}
			else
			{
				file_name = "Media/" + file_name;
			}
		}
		
		DirectX::TexMetadata metadata;
		DirectX::ScratchImage image;
		if (file_name.find(".tga") != std::string::npos)
		{
			HRESULT result = DirectX::LoadFromTGAFile(std::wstring(file_name.begin(), file_name.end()).c_str(), &metadata, image);
			if (FAILED(result))
				PRINT_ERROR("Cannot Load Texture File" + file_name);
		}
		else
		{
			HRESULT result = DirectX::LoadFromWICFile(std::wstring(file_name.begin(), file_name.end()).c_str(), NULL, &metadata, image);
			if (FAILED(result))
				PRINT_ERROR("Cannot Load Texture File" + file_name);
		}
		HRESULT result = DirectX::CreateTexture(d3d_re->D3DDevice(), image.GetImages(), image.GetImageCount(), metadata, &texture);
		if(FAILED(result))
			PRINT_ERROR("Cannot Load Texture File");

		D3D11_RESOURCE_DIMENSION dimension;
		texture->GetType(&dimension);
		CHECK_ASSERT(dimension == D3D11_RESOURCE_DIMENSION_TEXTURE2D);

		Texture* d3d_tex = Context::Instance().GetRenderFactory().MakeTexture2D(texture);
		
		return d3d_tex;
		
	}

	void D3DModel::LoadPomTexture( std::string file_name )
	{
		//TODO: write a add material fun to add pom texture
		pom_enabled_ = true;
		pom_texture_ = LoadTexture(file_name);
		//pom_srv_ = Context::Instance().GetRenderFactory().MakeRenderBuffer(pom_texture_, AT_GPU_READ_WRITE, BU_SHADER_RES);
	}


}
