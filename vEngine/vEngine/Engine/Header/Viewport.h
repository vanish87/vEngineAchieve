#ifndef VIEWPORT_H_
#define VIEWPORT_H_

#pragma once

#include "Common/Header/CommonPreDec.h"
#include "Engine/Header/EnginePreDec.h"
#include "Engine/Header/Camera.h"

namespace vEngine
{
	class Viewport
	{
	public:
		Viewport(void);
		Viewport(uint32_t left, uint32_t top, uint32_t width, uint32_t height);
		~Viewport(void);

		int Top(){return top_;};
		int Left(){return left_;};
		int Width(){return width_;};
		int Height(){return height_;};


		void SetWidth(uint32_t width) { this->width_ = width; };
		void SetHeight(uint32_t height) { this->height_ = height; };

		Camera& GetCamera(){return camera_;};
		void SetCamera(Camera* camera);


	protected:
		int left_;
		int top_;
		int width_;
		int height_;

		Camera camera_;
	};
}

#endif
