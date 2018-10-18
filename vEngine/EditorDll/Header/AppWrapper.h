// <author>Yuan Li</author>
// <email>yuan.li@nyu.edu</email>
// <date>2013-01-10</date>
// <summary>Picking and Assembling gameplay</summary>

#pragma once
#include "Engine/Header/App.h"
#include "Common/Header/Vector.h"
#include "Engine/Header/EnginePreDec.h"
#include "Engine/Header/GameState.h"
#include "D3D11\D3DModel.h"
#include <vector>


class MyApp :
	public vEngine::App
{
	friend class MyState;
public:
	MyApp(void);
	~MyApp(void);
	void InitObjects();
	void ReleaseObjects();
	void Update();

	void MakePlane();

	void SetSceneObject(vEngine::SceneObject* scene_object);

	static bool ToBeCalledFromLua(void* UserData);

private:
	static void LoadCallback(void* UserData);
	vEngine::PointLight* point_light_;
	vEngine::SpotLight* spot_light_;

	vEngine::SceneObject* test_scene_;

	vEngine::Timer* timer_;
	vEngine::Camera* camera_;

	bool first_person_;
	vEngine::float3 cam_pos_;
	vEngine::float3 cam_look_;

	float pitch_angle_;
	float speed_;

	vEngine::Text* newtext_;
	vEngine::GameStateSharedPtr newstate_;
};

//state is used for change between different game scene
//it is possible to implement a observer mode here
class MyState : public vEngine::GameState
{
public:
	MyState() {};
	~MyState() {};

	MyState(MyApp* app) { this->app_ = app; };

	MyApp* app_;
	virtual void Update() override;
	virtual void OnKeyDown(WPARAM key_para) override;
};

