#ifndef POSTPROCESS_H_
#define POSTPROCESS_H_

#pragma once

#include <vector>
#include "EnginePreDec.h"

namespace vEngine
{
	class PostProcess
	{
	public:
		PostProcess(void);
		~PostProcess(void);

		void LoadPPFile(std::string file_name);
		//temp solution for d3d shader object
		void SetPPShader(ShaderObject* shander_object);
		void SetInput(Texture* tex, size_t index);
		void SetOutput(Texture* tex, size_t index);
		void SetCamera(Camera* camera);
		void Apply();

	private:
		Mesh* fullscreen_mesh_;
		//for Render Target
		FrameBuffer* output_buffer_;
		//for Shader Resource
		std::vector<RenderBuffer*> input_srv_;
	};
}
#endif

