#include "D3DShaderobject.h"
#include "Engine\Header\Context.h"

namespace vEngine
{
	D3DShaderobject::D3DShaderobject(void)
	{
	}


	D3DShaderobject::~D3DShaderobject(void)
	{
	}

	void D3DShaderobject::LoadBinaryFile( std::string file_name )
	{
		std::ifstream fin(file_name, std::ios::binary);

		if (!fin)
		{
			PRINT_ERROR("Cannot open Fxo File ");
			return;
		}

		fin.seekg(0, std::ios_base::end);
		int size = (int)fin.tellg();
		fin.seekg(0, std::ios_base::beg);
		std::vector<char> compiledShader(size);

		fin.read(&compiledShader[0], size);
		fin.close();
		D3DRenderEngine* render_engine = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());
		HRESULT result = D3DX11CreateEffectFromMemory(&compiledShader[0], size,	0, render_engine->D3DDevice(), &fx_, NULL);
		if(FAILED(result))
			PRINT_ERROR("Cannot Load Effect File");
	}

	void D3DShaderobject::SetVariable( std::string name )
	{
		effect_variable_[name] = fx_->GetVariableByName(name.c_str());
	}

	void D3DShaderobject::SetMatrixVariable( std::string name )
	{
		matrix_variable_[name] = fx_->GetVariableByName(name.c_str())->AsMatrix();
	}

	void D3DShaderobject::SetBool( std::string name)
	{
		scale_variable_[name] = fx_->GetVariableByName(name.c_str())->AsScalar();
	}

	void D3DShaderobject::SetBool( std::string name, bool value )
	{
		ID3DX11EffectScalarVariable* bool_var = this->GetBoolVariable(name);
		HRESULT res = bool_var->SetBool(value);
		if(FAILED(res))
			PRINT_ERROR("Cannot SetBool");
	}
	void D3DShaderobject::SetMatrixVariable( std::string name, float4x4 & matrix )
	{
		//TODO: write a better solution for cast float4x4 to float*
		ID3DX11EffectMatrixVariable* mat_var = GetMatrixVariable(name);
		float *p = new float[matrix.size()];
		for(int i = 0; i< matrix.row(); i++)
			for(int j = 0; j < matrix.col(); j++)
				p[i*(int)matrix.row() + j] = matrix[i][j];
		HRESULT res = mat_var->SetMatrix(p);
		if(FAILED(res))
			PRINT_ERROR("Cannot set Shader Matrix");
		delete[] p;
	}

	void D3DShaderobject::SetVectorVariable( std::string name )
	{
		vector_variable_[name] = fx_->GetVariableByName(name.c_str())->AsVector();
	}

	void D3DShaderobject::SetVectorVariable( std::string name, float3 & vec3 )
	{
		ID3DX11EffectVectorVariable* vec3_var = GetVectorVariable(name);
		float data[3] = {vec3[0], vec3[1], vec3[2]};
		HRESULT res = vec3_var->SetFloatVector(data);
		if(FAILED(res))
			PRINT_ERROR("Cannot SetFloatVector");
	}

	void D3DShaderobject::SetVectorVariable( std::string name, float4 & vec4 )
	{
		ID3DX11EffectVectorVariable* vec4_var = GetVectorVariable(name);
		float data[4] = {vec4[0], vec4[1], vec4[2], vec4[3]};
		HRESULT res = vec4_var->SetFloatVector(data);
		if(FAILED(res))
			PRINT_ERROR("Fail to SetVectorVariable: " + name);
	}

	void D3DShaderobject::SetTechnique( std::string name )
	{
		tech_ = fx_->GetTechniqueByName(name.c_str());
	}

	size_t D3DShaderobject::GetPass()
	{ 
		D3DX11_TECHNIQUE_DESC tech_desc;
		HRESULT res = tech_->GetDesc( &tech_desc );
		if(FAILED(res))
			PRINT_ERROR("Cannot get pass");
		return tech_desc.Passes;
	}

	void D3DShaderobject::Apply( size_t pass_index )
	{
		D3DRenderEngine* d3d_render_engine = static_cast<D3DRenderEngine*>(&Context::Instance().GetRenderFactory().GetRenderEngine());
		HRESULT res = tech_->GetPassByIndex((UINT)pass_index)->Apply(0, d3d_render_engine->D3DDeviceImmContext());
		if(FAILED(res))
			PRINT_ERROR("Cannot apply pass");
	}

	void D3DShaderobject::SetRawData( std::string name, void* data, uint32_t size )
	{
		ID3DX11EffectVariable* var = this->GetVariable(name);
		HRESULT res = var->SetRawValue(data, 0, size);
		if(FAILED(res))
			PRINT_ERROR("Cannot set RawData");
	}

	void D3DShaderobject::SetReource( std::string name, Texture* data, uint32_t type )
	{

		ID3DX11EffectShaderResourceVariable* sr_var = this->GetShaderRourceVariable(name);
		if(data == nullptr)
		{
			HRESULT res = sr_var->SetResource(NULL);
			if(FAILED(res))
				PRINT_ERROR("Cannot set Resource");
			return;
		}
/*

		//TODO : Combine them together
		if(type == 0)//RenderBuffer
		{
			D3DTexture2D* d3d_tex = static_cast<D3DTexture2D*>(data);
			HRESULT res = sr_var->SetResource(d3d_tex->Re(1,0,TEXTURE2D));
			if(FAILED(res))
				PRINT("Cannot set Resource");
		}
		else//Shader Resource*/
		{
			D3DTexture2D* d3d_tex = static_cast<D3DTexture2D*>(data);
			HRESULT res = sr_var->SetResource(d3d_tex->GetShaderResourceView(1, 1, TEXTURE2D));
			if(FAILED(res))
				PRINT_ERROR("Cannot set Resource");
		}
	}

	void D3DShaderobject::SetShaderResourceVariable( std::string name )
	{
		bool valid = (bool)(fx_->GetVariableByName(name.c_str())->IsValid());
		if( !valid)
			PRINT_ERROR("Cannot find Variables");
		shader_resource_variable_[name] = fx_->GetVariableByName(name.c_str())->AsShaderResource();
	}




}
