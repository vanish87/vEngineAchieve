//
//this is temporary solution for effect file in D3D11, it will be replace by engine's own render effect class.

#ifndef D3DSHADEROBJECT_H_
#define D3DSHADEROBJECT_H_

#pragma once
#include <map>
#include <fstream>

#include "Engine\Header\EnginePreDec.h"
#include "D3DPreDec.h"
#include "Engine\Header\Context.h"
#include "Engine\Header\ShaderObject.h"
#include "D3DRenderEngine.h"

//external header
#include "D3dx11effect.h"

//old libs from DXSDK
// #pragma comment(lib, "../External/Effects11.lib")
// #pragma comment(lib, "../External/d3dcompiler.lib")


namespace vEngine
{
	class D3DShaderobject : public ShaderObject
	{
	public:
		D3DShaderobject(void);
		~D3DShaderobject(void);

		void LoadFxoFile(std::string file_name);

		ID3DX11EffectTechnique* GetTechnique(){return tech_;};
		virtual void SetTechnique(std::string name);

		ID3DX11EffectVariable* GetVariable(std::string name){return effect_variable_.find(name)->second;};
		virtual void SetVariable(std::string name);

		ID3DX11EffectMatrixVariable* GetMatrixVariable(std::string name){return matrix_variable_.find(name)->second;};
		virtual void SetMatrixVariable(std::string name);
		virtual void SetMatrixVariable(std::string name, float4x4 & matrix);

		ID3DX11EffectVectorVariable* GetVectorVariable(std::string name){return vector_variable_.find(name)->second;};
		virtual void SetVectorVariable(std::string name);
		virtual void SetVectorVariable( std::string name, float3 & vec3 );
		virtual void SetVectorVariable( std::string name, float4 & vec4 );

		ID3DX11EffectScalarVariable* GetBoolVariable(std::string name){return scale_variable_.find(name)->second;};
		virtual void SetBool( std::string name);
		virtual void SetBool( std::string name, bool value );

		virtual size_t GetPass();
		virtual void Apply(size_t pass_index);

		virtual void SetRawData( std::string name, void* data, uint32_t size );

		ID3DX11EffectShaderResourceVariable* GetShaderRourceVariable(std::string name){return shader_resource_variable_.find(name)->second;};
		virtual void SetShaderResourceVariable(std::string name);
		//type: 0 for RenderBuffer; 1 for ShaderResource
		virtual void SetReource( std::string name, Texture* data, uint32_t type );





	private:

		ID3DX11Effect* fx_;
		ID3DX11EffectTechnique* tech_;

		std::map<std::string, ID3DX11EffectShaderResourceVariable*> shader_resource_variable_;
		std::map<std::string, ID3DX11EffectMatrixVariable*> matrix_variable_;
		std::map<std::string, ID3DX11EffectVectorVariable*> vector_variable_;
		std::map<std::string, ID3DX11EffectScalarVariable*> scale_variable_;
		std::map<std::string, ID3DX11EffectVariable*>		effect_variable_;
	};

}

#endif

