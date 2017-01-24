#ifndef LIGHT_H_
#define LIGHT_H_

#pragma once
#include "Common\Header\CommonPreDec.h"
#include "Camera.h"
#include "TypeAndFormat.h"

namespace vEngine
{
	class Light
	{
	public:
		~Light(void);

		explicit Light(LightType light_type)
			:light_type_(light_type)
		{
			light_color_ = float4(1, 1, 1, 1);
			//virtual_camera_ = new Camera();

			switch (light_type)
			{
			case LT_POINT:
				light_attrib_ = float4(0,0.1f,0,300);
				break;
			case LT_SPOT:
				light_attrib_ = float4(1,0,0,500);
				break;
			case LT_DERECTIONAL:
				break;
			default:
				break;
			}
		}

		void SetType(LightType type);
		LightType GetType(){return light_type_;};
		void SetColor(float4 color);
		float4 GetColor(){return light_color_;};

		void SetAttrib(float4 light_attrib);
		float4 GetAttrib(){return light_attrib_;};

		float GetRange();;
		void SetRange(float range);

		Camera* GetCamera(){return &virtual_camera_;};

		void AddToScene();

	private:
		virtual void UpdateCamera() = 0;

	protected:
		float4 light_color_;
		LightType light_type_;

		float4 light_attrib_;

		Camera virtual_camera_;
	};

	class PointLight : public Light
	{
	public:
		PointLight(void);
		~PointLight(void);

		float3 GetPos();;
		void SetPos(float3 pos);

		virtual void UpdateCamera();

	private:
		float3 pos_;
	};

	class SpotLight : public Light
	{
	public:
		SpotLight(void);
		~SpotLight(void);

		float3 GetPos(){return pos_;};
		void SetPos(float3 pos);

		float3 GetDir(){return dir_;};
		void SetDir(float3 dir);

		float GetOuterAngle(){return outer_angle_;};
		void SetOuterAngle(float outer_angle);

		float GetInnerAngle(){return inner_angle_;};
		void SetInnerAngle(float inner_angle);

		virtual void UpdateCamera();

	private:
		float3 pos_;
		float3 dir_;
		
		float outer_angle_;
		float inner_angle_;


	};
}

#endif
