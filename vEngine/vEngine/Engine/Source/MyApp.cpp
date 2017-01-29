#include "Engine\Header\MyApp.h"
#include "Engine\Header\Context.h"
#include "Common\Header\Timer.h"
#include "Engine\Header\RenderTools.h"

//#include "StartMenu.h"

#include "D3D11\D3DSkyDome.h"

using namespace vEngine;

MyApp::MyApp(void) : App("vEngine")
{

}


MyApp::~MyApp(void)
{
}

void MyApp::InitObjects()
{
	SceneObject testoject, testoject1;
	testoject.AddComponent(&testoject1);

	vEngine::UUID RefID = testoject1.id();
	vEngine::UUID nullID = GameObject::NullObject().id();
	//vEngine::UUID newID;
	assert(testoject1 == testoject.FindComponentByUUID(RefID));
	assert(testoject1 != testoject.FindComponentByUUID(nullID));
	assert(GameObject::NullObject() == testoject.FindComponentByUUID(nullID));
	assert(GameObject::NullObject() == testoject.FindComponentByUUID(testoject.id()));

	DebugTracking::GetInstance().PrintALL();

	//set up lights
	for(int i = 0; i < 0 ; ++i)
	{
		point_light_ = new PointLight();
		point_light_->SetPos(float3(50.f + i*20 ,10.f, 0.f));	
		point_light_->SetColor(float4((200 + i)/255.0f, (100 - i)/255.0f, (100 + i)/255.0f, 1.0f));
		point_light_->AddToScene();
	}

	spot_light_ = new SpotLight();
	spot_light_->SetPos(float3(0, 1, 1));
	spot_light_->SetDir(float3(0, 0, 0)- float3(0, 1, 1));
	spot_light_->SetInnerAngle(Math::PI / 6);
	spot_light_->SetOuterAngle(Math::PI / 4);
	spot_light_->AddToScene();

	float4x4 mat,trans;
	D3DModel *model = new D3DModel();
	//model->LoadFile("Media/sponza/sponza.sobj");
	//model->LoadFile("Media/dabrovic-sponza/sponza.sobj");
	model->LoadFile("Media/spacecraft_new.dae");
	model->LoadShaderFile("FxFiles/DeferredLighting.cso");
	Math::Scale(mat, 10);
	Math::Translate(trans, 0, 0.5f, 0);
	model->SetModelMatrix(mat*trans);
	ship_ = new SceneObject(model);
	ship_->AddToScene();

	model = new D3DModel();
	//model->LoadFile("Media/plane_ssdo.dae");
	model->LoadShaderFile("FxFiles/DeferredLighting.cso");
	Math::Scale(mat, 10);
	model->SetModelMatrix(mat);
	ship_ = new SceneObject(model);
	ship_->AddToScene();

	Mesh* newMesh = RenderTools::GetInstance().MakeFullScreenMesh();
	float4x4 rotaiton;
	Math::XRotation(rotaiton, Math::PI * 0.5);
	Math::Translate(trans, 0, 0, -1.0f);
	//newMesh->SetModelMatrix(rotaiton);
	newMesh->SetShaderObject(model->GetShaderObject());

	Material* meshMat = new Material();
	meshMat->diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
	meshMat->specular = float4(0.2f, 0.5f, 0.5f, 1.0f);
	meshMat->shininess = 10;

	D3DModel* meshModel = new D3DModel();
	meshModel->SetModelMatrix(trans * rotaiton);
	meshModel->AddMesh(newMesh);
	meshModel->AddMaterial(meshMat);
	meshModel->SetShaderObject(model->GetShaderObject());

	SceneObject* newModel = new SceneObject(meshModel);
	newModel->AddToScene();


	timer_ = new Timer();
	timer_->Retart();
	
	first_person_ = false;
	pitch_angle_ = 0;
	speed_ = 0.5;
	camera_ = new Camera(1280 / 800.0f);// Context::Instance().GetSceneManager().GetMainCamera();
	cam_pos_ = float3(82.2f, 270.87f, -67.49f);
	cam_look_ = float3(81.78f, 270.16f, -66.94f);
	cam_pos_ = float3(0, 2, -2);
	cam_look_ = float3(0, 0, 0);
	camera_->SetView(cam_pos_, cam_look_, float3(0,1,0));
	//camera->SetProjection(Math::PI/4, 1280.0f/800.0f,1,3000);
	camera_->AddToScene(true);

	DebugTracking::GetInstance().PrintALL();

	MyState* newstate = new MyState(this);
	Context::Instance().GetStateManager().ChangeState(newstate, SOP_PUSH);
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
	}
	//std::cout<<spot_light_->GetPos().x()<<"\r";
    spot_light_->SetDir(float3(0.f,-Math::Abs(Math::Sin(timer_->Timef()/5000.0f)),Math::Cos(timer_->Timef()/5000.0f)));
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
		app_->first_person_ = true;
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

	MyApp app;
	app.Initialize();
	app.Run();



	_CrtDumpMemoryLeaks();
	return 0;
}
#endif // !ENABLE_TEST


 