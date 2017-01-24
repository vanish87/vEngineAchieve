// <author>Yuan Li</author>
// <email>yuan.li@nyu.edu</email>
// <date>2013-01-10</date>
// <summary>Picking and Assembling gameplay</summary>

#pragma once
#include "App.h"
#include "Common\Header\Vector.h"
#include "D3D11\D3DModel.h"
#include <vector>


class MyApp :
	public vEngine:: App
{
public:
	MyApp(void);
	~MyApp(void);
	void InitObjects();
	void ReleaseObjects();
	void Update();

private:
	vEngine::PointLight* point_light_;
	vEngine::SpotLight* spot_light_;

	vEngine::SceneObject* ship_;

	vEngine::Timer* timer_;
	vEngine::Camera* camera_;

	bool first_person_;
	vEngine::float3 cam_pos_;
	vEngine::float3 cam_look_;

	float pitch_angle_;
	float speed_;


};

