#ifndef SHADEROBJECT_H_
#define SHADEROBJECT_H_

#pragma once
#include <string>
#include "Common/Header/CommonPreDec.h"
#include "Engine/Header/EnginePreDec.h"

namespace vEngine
{
	class ShaderObject
	{
	public:
		ShaderObject(void);
		virtual ~ShaderObject(void);

		virtual void SetVariable(std::string name) = 0;
		virtual void SetVectorVariable(std::string) = 0;
		virtual void SetVectorVariable(std::string name, float3 & vec3) = 0;
		virtual void SetVectorVariable(std::string name, float4 & vec4) = 0;
		virtual void SetMatrixVariable(std::string name) = 0;
		virtual void SetMatrixVariable(std::string name, float4x4 & matrix) = 0;
		virtual void SetRawData(std::string name, void* data, uint32_t size) = 0;
		virtual void SetShaderResourceVariable(std::string name) = 0;
		//virtual void SetReource(std::string name, RenderBuffer* data, uint32_t size) = 0;
		virtual void SetReource(std::string name, Texture* data) = 0;

		virtual void SetBool(std::string) = 0;
		virtual void SetBool(std::string, bool value) = 0;

		virtual void SetTechnique(std::string name) = 0;
		virtual size_t GetPass() = 0;
		virtual void Apply(size_t pass_index) = 0;
		
		virtual void LoadBinaryFile(std::string file_name);

		static ShaderObject* FindShaderByName(const std::string shader_name);
		void SetupShaderVariable(const std::string shader_name);
		void SetupCommonVariable();
	};
}

#endif
