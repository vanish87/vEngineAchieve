#include "Engine/Header/Camera.h"
#include "Engine/Header/Context.h"

namespace vEngine
{
	Camera::Camera(void)
	{
		this->SetView(float3(0,0,-1), float3(0,0,0), float3(0,1,0));
		this->SetProjection(Math::PI/4, 1 , 1, 1000.0f);
		//TODO : Init frustum here
		frustum_;
	}

	Camera::Camera(float aspect)
	{
		this->SetView(float3(0,0,-1), float3(0,0,0), float3(0,1,0));
		this->SetProjection(Math::PI/4, aspect , 1, 1000.0f);
		frustum_;
	}

	Camera::~Camera(void)
	{
	}

	void Camera::SetView( float3 eye, float3 at, float3 up )
	{
		eye_ = eye;
		at_ = at;
		up_ = up;

		view_matrix_ = Math::LookAtLH(eye_, at_, up_);
	}

	void Camera::SetProjection( float fovy, float aspect, float zn, float zf )
	{
		fovy_ = fovy;
		aspect_ = aspect;
		zn_ = zn;
		zf_ = zf;

		proj_matrix_ = Math::PerspectiveFovLH(fovy_, aspect_, zn_, zf_);
	}

	float3 Camera::GetPos()
	{
		return eye_;
	}

	float3 Camera::GetLookAt()
	{
		return at_;
	}

	float3 Camera::GetUp()
	{
		return up_;
	}

	float Camera::GetFovy()
	{
		return fovy_;
	}
	float Camera::GetAspect()
	{
		return aspect_;
	}
	float2 Camera::GetNearFar()
	{
		return float2(zn_,zf_);
	}

	void Camera::Pitch( float angle )
	{
		float3 dir =  at_ - eye_;
		float3 left = Math::Cross(dir,up_);
		left = Math::Normalize(left);

		float4x4 mat;
		Math::Identity(mat);
		Math::RotationAxis(mat, left, angle);

		up_ = Math::TransformVector(up_, mat);
		dir = Math::TransformVector(dir, mat);
		dir = Math::Normalize(dir);

		view_matrix_ = Math::LookAtLH(eye_, dir, up_);
	}

	void Camera::Yaw( float angle )
	{
		float3 dir =  at_ - eye_;
		float4x4 mat;
		Math::Identity(mat);
		Math::RotationAxis(mat, up_, angle);

		dir = Math::TransformVector(dir, mat);
		dir = Math::Normalize(dir);
		at_ = eye_ + dir;

		view_matrix_ = Math::LookAtLH(eye_, at_, up_);
	}

	void Camera::Roll( float angle )
	{
		float3 dir =  at_ - eye_;
		float4x4 mat;
		Math::Identity(mat);
		Math::RotationAxis(mat, dir, angle);

		up_ = Math::TransformVector(up_, mat);

		view_matrix_ = Math::LookAtLH(eye_, at_, up_);
	}


	void Camera::AddToScene(bool as_main /*= false*/)
	{
		Context::Instance().GetSceneManager().AddCamera(this, as_main);
	}

}
