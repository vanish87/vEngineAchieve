#include "MyApp.h"
#include "Engine\Header\Context.h"
#include "Common\Header\Timer.h"
#include "Engine\Header\RenderTools.h"
#include "Engine\Header\ResourceLoader.h"
#include "Engine\Header\ScriptTest.h"
#include "Engine\Header\ScriptContext.h"
#include "Engine\Header\Text.h"

#include "D3D11\D3DSkyDome.h"

#include "TestCase/TestCase.h"

using namespace vEngine;

static MyApp app;

static Text t1 = L"this is a text";
static Text t2 = L"this is a text1";

MyApp::MyApp(void) : App("vEngine")
{

}


MyApp::~MyApp(void)
{
}

void MyApp::InitObjects()
{
	//ScriptTest::GetInstance().Run();
	ScriptContext& script = Context::Instance().GetScriptContext();
	ScriptFuctionDescription dec;
	dec.name_ = "ThisIsAcppFuction";
	dec.fuction_ = MyApp::ToBeCalledFromLua;
	script.RegisterCppFunction(dec);
	script.RunFile("LuaScript/HelloWorld.lua");

	script.StartMonitorPath("LuaScript/");

	SceneObject testoject, testoject1;
	testoject.AddComponent(&testoject1);

	vEngine::UUID RefID = testoject1.id();
	vEngine::UUID nullID = GameObject::NullObject().id();
	//vEngine::UUID newID;
	CHECK_ASSERT(testoject1 == testoject.FindComponentByUUID(RefID));
	CHECK_ASSERT(testoject1 != testoject.FindComponentByUUID(nullID));
	CHECK_ASSERT(GameObject::NullObject() == testoject.FindComponentByUUID(nullID));
	CHECK_ASSERT(GameObject::NullObject() == testoject.FindComponentByUUID(testoject.id()));

	DebugTracking::GetInstance().PrintALL();

	RunFontTest();

	//set up lights
	for(int i = 0; i < 1 ; ++i)
	{
		point_light_ = new PointLight();
		point_light_->SetPos(float3(50.f + i*20 ,10.f, 0.f));	
		point_light_->SetColor(float4((200 + i)/255.0f, (100 - i)/255.0f, (100 + i)/255.0f, 1.0f));
		point_light_->AddToScene();
	}

	spot_light_ = new SpotLight();
	spot_light_->SetPos(float3(0, 50, 0));
	spot_light_->SetDir(float3(0, 10, 0) - float3(0, 50, 0));

	spot_light_->SetInnerAngle(Math::PI / 6);
	spot_light_->SetOuterAngle(Math::PI / 4);
	//spot_light_->AddToScene();

	float4x4 mat,trans;

	D3DModel* model = new D3DModel();
	//model->LoadFile("Media/suitcase_obj/suitcase_02.obj");
	//model->LoadFile("Media/mitsuba/mitsuba.obj");
	//model->LoadShaderFile("DeferredLighting");

	//Math::Translate(trans, 0, 0.2f, 0);
	//Math::Scale(mat, 10);
	//model->SetModelMatrix( mat* trans);
	//ship_ = new SceneObject(model);
	//ship_->AddToScene();


	//model = new D3DModel();
	model->LoadFile("Media/sponza/sponza.sobj", &MyApp::LoadCallback);
	//model->LoadFile("Media/dabrovic-sponza/sponza.sobj");
	//model->LoadFile("Media/spacecraft_new.dae");
	//model->LoadShaderFile("DeferredLighting");
	//Math::Scale(mat, 0.5);
	//Math::Translate(trans, 0, 0.2f, 0);
	//model->SetModelMatrix(mat);
	//ship_ = new SceneObject(model);
	//ship_->AddToScene();

	//this->MakePlane();

	timer_ = new Timer();
	timer_->Retart();
	
	first_person_ = false;
	pitch_angle_ = 0;
	speed_ = 2.5;
	camera_   = new Camera(1280 / 800.0f);// Context::Instance().GetSceneManager().GetMainCamera();
	cam_pos_  = float3(82.2f, 270.87f, -67.49f);
	cam_look_ = float3(81.78f, 270.16f, -66.94f);
	cam_pos_  = float3(205, 152, -73);
	cam_look_ = float3(204, 151, -73);
	camera_->SetView(cam_pos_, cam_look_, float3(0,1,0));
	//camera->SetProjection(Math::PI/4, 1280.0f/800.0f,1,3000);
	camera_->AddToScene(true);

	DebugTracking::GetInstance().PrintALL();

	//text_.init with font name etc.
	newtext_ = new Text(L"test new");
	newtext_->SetRect(int4(50, 300, 0, 0));
	//newtext_->AddToScene();

	t1.SetRect(int4(50, 50, 0, 0));
	t2.SetRect(int4(100, 100, 0, 0));

	newstate_ = std::make_shared<MyState>(this);
	Context::Instance().GetStateManager().ChangeState(newstate_, SOP_PUSH);
}


void MyApp::LoadCallback(void* UserData)
{
	D3DModel* model = static_cast<D3DModel*>(UserData);
	model->LoadShaderFile("DeferredLighting");
	float4x4 mat;
	Math::Scale(mat, 0.5);
	//Math::Translate(trans, 0, 0.2f, 0);
	model->SetModelMatrix(mat);
	SceneObject* s = new SceneObject(model);
	app.SetSceneObject(s);
	s->AddToScene();

	SceneObject* news = Context::Instance().GetSceneManager().FindOjectByUUID(s->id());
	CHECK_AND_ASSERT(news == s, "Check scene objects");
	CHECK_AND_ASSERT(*news == *s, "Check scene objects");
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
	Test += std::to_wstring(cam_pos_.x()) + L" " + std::to_wstring(cam_pos_.y());
	newtext_->SetContent(Test);
	newtext_->Draw();
	//std::cout<<spot_light_->GetPos().x()<<"\r";
    //spot_light_->SetDir(float3(0.f,-Math::Abs(Math::Sin(timer_->Timef()/5000.0f)),Math::Cos(timer_->Timef()/5000.0f)));

	//text.setcontent("xxxx");
	//text.setposition();


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


 