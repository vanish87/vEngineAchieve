#ifndef RENDERBUFFER_H_
#define RENDERBUFFER_H_

#pragma once

#include "EnginePreDec.h"
#include "TypeAndFormat.h"

namespace vEngine
{
	//TODO : Add Mapper class to make sure the Type definition when mapping resource
	class RenderBuffer
	{
	public:
		RenderBuffer(void);
		RenderBuffer(BufferUsage usage, AccessType access_type)
			:usage_(usage), access_type_(access_type)
		{
		
		};
		~RenderBuffer(void);
		BufferUsage GetUsage();


		//this only check BufferUsage and AccessType
		//you should make sure the return type is the same as you create it with
		//template <typename T>
		void* Map(AccessType access_type)
		{
			this->DoMap(access_type);
			assert(data_);
			return data_ ;
		};
		virtual void UnMap() = 0;
	protected:
		BufferUsage usage_;
		AccessType access_type_;

		//assign data_ with mapped resource
		virtual void DoMap(AccessType access_type) = 0;

		void* data_;
	};
}

#endif

