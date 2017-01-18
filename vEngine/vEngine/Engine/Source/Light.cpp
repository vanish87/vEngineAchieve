#include "Engine\Header\Light.h"
#include "Engine\Header\Context.h"

namespace vEngine
{
	Light::~Light(void)
	{
	}

	void Light::SetType( LightType type )
	{
		this->light_type_ = type;
	}

	void Light::SetColor( float4 color )
	{
		this->light_color_ = color;
	}

	void Light::AddToScene()
	{
		Context::Instance().GetSceneManager().AddLight(this);
	}

	void Light::SetCamera( Camera* camera )
	{
		virtual_camera_ = camera;
	}

	float Light::GetRange()
	{
		return light_attrib_.w();
	}

	void Light::SetRange( float range )
	{
		light_attrib_.w() = range;
	}

	void Light::SetAttrib( float4 light_attrib )
	{
		light_attrib_ = light_attrib;
	}

	PointLight::PointLight( void )
		:Light(LT_POINT)
	{
		pos_ = float3(0, 0, 0);
	}

	PointLight::~PointLight( void )
	{

	}

	void PointLight::SetPos( float3 pos )
	{
		pos_ = pos;
	}

	void PointLight::UpdateCamera()
	{
		throw std::exception("The method or operation is not implemented.");
	}

	vEngine::float3 PointLight::GetPos()
	{
		return pos_;
	}



	SpotLight::SpotLight( void )
		:Light(LT_SPOT)
	{
		pos_ = float3(0, 0, 0);
		dir_ = float3(0, 0, -1);

		inner_angle_ = Math::PI / 6;
		outer_angle_ = Math::PI / 4;

		this->UpdateCamera();
	}

	SpotLight::~SpotLight( void )
	{

	}

	void SpotLight::SetPos( float3 pos )
	{
		pos_ = pos;
		this->UpdateCamera();
	}

	void SpotLight::SetDir( float3 dir )
	{
		dir_ = Math::Normalize(dir);
		this->UpdateCamera();
	}

	void SpotLight::SetOuterAngle( float outer_angle )
	{
		outer_angle_ = outer_angle;
		this->UpdateCamera();
	}

	void SpotLight::SetInnerAngle( float inner_angle )
	{
		inner_angle_ = inner_angle;
	}

	void SpotLight::UpdateCamera()
	{
		virtual_camera_->SetProjection(outer_angle_ * 2, 1280/800.0f, 1.0f, 1000.0f);
		//TODO : rotate up vector
		dir_ = Math::Normalize(dir_);
		virtual_camera_->SetView(pos_, pos_+ dir_,float3(0, 1, 0));
	}



}
