#include "Simulation2D.h"
#include "Engine/Header/Context.h"
#include "Common/Header/Timer.h"
#include "Engine/Header/RenderTools.h"
#include "Engine/Header/ResourceLoader.h"
#include "Engine/Header/ScriptTest.h"
#include "Engine/Header/ScriptContext.h"
#include "Engine/Header/Text.h"
#include "Engine/Header/RenderTools.h"
#include "SnowSimulator.hpp"


using namespace vEngine;

static MyApp app;
static SnowSimulator snow_sims_;

static Text t1 = L"this is a text";
static Text t2 = L"this is a text1";

Mesh* test;

MyApp::MyApp(void) : App("vEngine")
{

}


MyApp::~MyApp(void)
{
}

void MyApp::InitObjects()
{
	
	//set up lights
	this->spot_light_ = new SpotLight();
	this->spot_light_->SetDir(float3(0,0,1));
	this->spot_light_->SetPos(float3(0, 0, -500));
	this->spot_light_->SetInnerAngle(Math::PI / 6);
	this->spot_light_->SetOuterAngle(Math::PI / 4);
	//this->spot_light_->AddToScene();

	this->point_light_ = new PointLight();
	this->point_light_->SetPos(float3(0, 0, -500));
	this->point_light_->SetRange(1000);
	this->point_light_->AddToScene();


	timer_ = new Timer();
	timer_->Retart();
	
	first_person_ = false;
	pitch_angle_ = 0;
	speed_ = 2.5;
	camera_   = new Camera(1280 / 720.0f);// Context::Instance().GetSceneManager().GetMainCamera();
	cam_pos_  = float3(0, 0, -500);
	cam_look_ = float3(0, 0, 0);
	camera_->SetView(cam_pos_, cam_look_, float3(0,1,0));
	//camera->SetProjection(Math::PI/4, 1280.0f/800.0f,1,3000);
	camera_->AddToScene(true);

	//text_.init with font name etc.
	newtext_ = new Text(L"test new");
	newtext_->SetRect(int4(50, 300, 0, 0));
	//newtext_->AddToScene();

	t1.SetRect(int4(50, 50, 0, 0));
	t2.SetRect(int4(100, 100, 0, 0));	

	snow_sims_.Start();
	
	newstate_ = std::make_shared<MyState>(this);
	Context::Instance().GetStateManager().ChangeState(newstate_, SOP_PUSH);
}


void MyApp::LoadCallback(void* UserData)
{
	
}

void MyApp::ReleaseObjects()
{
	delete point_light_;
	delete spot_light_;
	delete timer_;
	delete camera_;
}

void MyApp::Update()
{
	//std::cout<<timer_->Time()<<"\r";
	if(first_person_)
	{
		//Camera* camera = Context::Instance().GetSceneManager().GetMainCamera();
		camera_->SetView(cam_pos_, cam_look_, float3(0,1,0));

		float4x4 mat,trans, rotate;
		Math::Scale(mat, 30);
		Math::Translate(trans,0,10,0);
		Math::YRotation(rotate,Math::PI/2 *Math::Cos(timer_->Timef()/1000.0f));
		//ship_->GetRenderElement()->SetModelMatrix(rotate * trans * mat);

		//t1.Draw();
		//t2.Draw();
	}
	std::wstring Test = L"cam pos ";
	Test += std::to_wstring(cam_pos_.x()) + L" " + std::to_wstring(cam_pos_.y()) + L" " + std::to_wstring(cam_pos_.z());
	newtext_->SetContent(Test);
	newtext_->Draw();	
}



void MyApp::MakePlane()
{
	Mesh* newMesh = RenderTools::GetInstance().MakeFullScreenMesh();
	float4x4 rotaiton, trans;
	Math::XRotation(rotaiton, Math::PI * 0.5);
	Math::Translate(trans, 0, 0, -1.0f);
	//newMesh->SetModelMatrix(rotaiton);
	//newMesh->SetShaderObject(model->GetShaderObject());

	Material* meshMat = new Material();
	meshMat->diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
	meshMat->specular = float4(0.2f, 0.5f, 0.5f, 1.0f);
	meshMat->shininess = 10;

	D3DModel* meshModel = new D3DModel();
	meshModel->LoadShaderFile("DeferredLighting");
	//meshModel->SetModelMatrix(trans * rotaiton);
	meshModel->AddMesh(newMesh);
	meshModel->AddMaterial(meshMat);
	newMesh->SetShaderObject(meshModel->GetShaderObject());

	SceneObject* newModel = new SceneObject(meshModel);
	newModel->AddToScene();
}


void MyApp::SetSceneObject(SceneObject* scene_object)
{
	this->test_scene_ = scene_object;
}

bool MyApp::ToBeCalledFromLua(void* UserData)
{
	PRINT("Lua call this function");
	return true;
}

void MyState::Update()
{
	//throw std::logic_error("The method or operation is not implemented.");
}


void MyState::OnKeyDown(WPARAM key_para)
{
	switch (key_para)
	{
	case 'F':
		app_->first_person_ = !app_->first_person_;
		break;
	case '1':
		Context::Instance().GetRenderFactory().GetRenderEngine().GetDeferredRendering()->ToggleGbuffer(0);
		break;
	case '2':
		Context::Instance().GetRenderFactory().GetRenderEngine().GetDeferredRendering()->ToggleGbuffer(1);
		break;
	case '3':
		Context::Instance().GetRenderFactory().GetRenderEngine().GetDeferredRendering()->ToggleLighting();
		break;
	case '4':
		Context::Instance().GetRenderFactory().GetRenderEngine().GetDeferredRendering()->ToggleDepth();
		break;
	default:
		break;
	}
	if (app_->first_person_)
	{
		Camera* camera = app_->camera_;
		float3 up_ = camera->GetUp();
		float4x4 mat;

		float3 dir = app_->cam_look_ - app_->cam_pos_;
		float3 left = Math::Cross(dir, up_);
		switch (key_para)
		{
		case 'E':
			left = Math::Normalize(left);

			app_->pitch_angle_ = -Math::PI / 180;
			Math::Identity(mat);
			Math::RotationAxis(mat, left, app_->pitch_angle_);

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::TransformVector(dir, mat);
			dir = Math::Normalize(dir);
			app_->cam_look_ = dir + app_->cam_pos_;
			break;
		case 'C':
			left = Math::Normalize(left);

			app_->pitch_angle_ = Math::PI / 180;
			Math::Identity(mat);
			Math::RotationAxis(mat, left, app_->pitch_angle_);

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::TransformVector(dir, mat);
			dir = Math::Normalize(dir);
			app_->cam_look_ = dir + app_->cam_pos_;
			break;
		case 'X':
			left = float3(0, 1, 0);

			app_->pitch_angle_ = -Math::PI / 180;
			Math::Identity(mat);
			Math::RotationAxis(mat, left, app_->pitch_angle_);

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::TransformVector(dir, mat);
			dir = Math::Normalize(dir);
			app_->cam_look_ = dir + app_->cam_pos_;
			break;
		case 'Z':
			left = float3(0, 1, 0);

			app_->pitch_angle_ = Math::PI / 180;
			Math::Identity(mat);
			Math::RotationAxis(mat, left, app_->pitch_angle_);

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::TransformVector(dir, mat);
			dir = Math::Normalize(dir);
			app_->cam_look_ = dir + app_->cam_pos_;
			break;
		case 'W':

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::Normalize(dir);
			dir = dir*app_->speed_;
			app_->cam_look_ = app_->cam_look_ + dir;
			app_->cam_pos_ = app_->cam_pos_ + dir;
			break;
		case 'S':

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::Normalize(dir);
			dir = dir*app_->speed_;
			app_->cam_look_ = app_->cam_look_ - dir;
			app_->cam_pos_ = app_->cam_pos_ - dir;
			break;

		case 'A':

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::Normalize(left);
			dir = dir*app_->speed_;
			app_->cam_look_ = dir + app_->cam_look_;
			app_->cam_pos_ = app_->cam_pos_ + dir;
			break;
		case 'D':

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::Normalize(left);
			dir = dir*app_->speed_;
			app_->cam_look_ = app_->cam_look_ - dir;
			app_->cam_pos_ = app_->cam_pos_ - dir;
			break;
		case 'P':
			app_->speed_ += 0.1f;
			break;
		case 'O':
			app_->speed_ -= 0.1f;
			break;
		default:
			break;
		}
	}


}




#ifndef ENABLE_TEST

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	Configure MyConfig;
	MyConfig.LoadConfig("Configure/Config.xml");
	Context::Instance().Setup(MyConfig.GetContextSetting());

	app.Initialize();
	app.Run();



	_CrtDumpMemoryLeaks();
	return 0;
}
#endif // !ENABLE_TEST


 