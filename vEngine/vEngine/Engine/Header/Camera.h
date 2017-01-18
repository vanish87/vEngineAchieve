#ifndef CAMERA_H_
#define CAMERA_H_

#pragma once

#include "Common\Header\CommonPreDec.h"
#include "Common\Header\Frustum.h"
#include "Common\Header\Vector.h"
#include "Common\Header\Matrix.h"
#include "Common\Header\Math.h"
#include "Engine\Header\SceneObject.h"

namespace vEngine
{
	class Camera : public SceneObject
	{
	public:
		Camera(void);
		Camera(float aspect);
		~Camera(void);

		DEBUG_CLASS_FILE_NAME

		void SetView(float3 eye, float3 at, float3 up);
		float4x4 GetViewMatirx(){return view_matrix_;};
		void SetProjection(float fovy, float aspect, float zn, float zf);
		float4x4 GetProjMatrix(){return proj_matrix_;};

		float3 GetPos(){return eye_;};
		float3 GetLookAt();
		float3 GetUp();

		// rotate on right vector
		void Pitch(float angle);
		// rotate on up vector
		void Yaw(float angle);   
		// rotate on look vector
		void Roll(float angle);  

	private:
		Frustum	frustum_;

		float fovy_;
		float aspect_;
		float zn_;
		float zf_;
		float4x4 proj_matrix_;

		float3 eye_;
		float3 at_;
		float3 up_;
		float4x4 view_matrix_;

	};

}

#endif

