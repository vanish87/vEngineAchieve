#include "Engine\Header\MyApp.h"
#include "Engine\Header\Context.h"
#include "Common\Header\Timer.h"

//#include "StartMenu.h"

#include "D3D11\D3DSkyDome.h"

using namespace MocapGE;
MyApp::MyApp(void) : App("The CentBebop Design")
{

}


MyApp::~MyApp(void)
{
}

void MyApp::InitObjects()
{
	//set up lights
	for(int i = -30; i < 30 ; ++i)
	{
		point_light_ = new PointLight();
		point_light_->SetPos(float3(50.f + i*20 ,10.f, 0.f));	
		point_light_->SetColor(float4((200 + i)/255.0f, (100 - i)/255.0f, (100 + i)/255.0f, 1.0f));
		point_light_->AddToScene();
	}

	spot_light_ = new SpotLight();
	spot_light_->SetPos(float3(50, 200, 0));
	spot_light_->SetDir(float3(0,10,0) - float3(50, 500, 0));
	spot_light_->SetInnerAngle(Math::PI / 6);
	spot_light_->SetOuterAngle(Math::PI / 4);
	spot_light_->AddToScene();

	float4x4 mat,trans;
	D3DModel *model = new D3DModel();
	model->LoadFile("../Media/sponza.dae");
	model->LoadShaderFile("FxFiles/DeferredLighting.cso");
	Math::Scale(mat, 0.5);
	model->SetModelMatrix(mat);
	ship_ = new SceneObject(model);
	ship_->AddToScene();

	model = new D3DModel();
	model->LoadFile("../Media/spacecraft_new.dae");
	model->LoadShaderFile("FxFiles/DeferredLighting.cso");
	Math::Scale(mat, 10);
	Math::Translate(trans,0,10,0);
	model->SetModelMatrix(trans * mat);
	ship_ = new SceneObject(model);
	ship_->AddToScene();

	timer_ = new Timer();
	timer_->Retart();
	
	first_person_ = false;
	pitch_angle_ = 0;
	speed_ = 2.5;
	Camera* camera = new Camera();// = Context::Instance().AppInstance().GetCamera();
	cam_pos_ = float3(82.2f,270.87f,-67.49f);
	cam_look_ = float3(81.78f,270.16f,-66.94f);
	camera->SetView(cam_pos_, cam_look_, float3(0,1,0));
	//camera->SetProjection(Math::PI/4, 1280.0f/800.0f,1,3000);
}

void MyApp::ReleaseObjects()
{
	delete point_light_;
	delete spot_light_;
	delete timer_;
}

void MyApp::Update()
{
	//std::cout<<timer_->Time()<<"\r";
	if(first_person_)
	{
		Camera* camera = new Camera();// = Context::Instance().AppInstance().GetCamera();
		camera->SetView(cam_pos_, cam_look_, float3(0,1,0));

		float4x4 mat,trans, rotate;
		Math::Scale(mat, 30);
		Math::Translate(trans,0,10,0);
		Math::YRotation(rotate,Math::PI/2 *Math::Cos(timer_->Timef()/1000.0f));
		//ship_->GetRenderElement()->SetModelMatrix(rotate * trans * mat);
	}
	//std::cout<<spot_light_->GetPos().x()<<"\r";
    spot_light_->SetDir(float3(0.f,-Math::Abs(Math::Sin(timer_->Timef()/10000.0f)),Math::Cos(timer_->Timef()/10000.0f)));
}

/*void MyApp::OnKeyDown( WPARAM key_para )
{
	switch (key_para)
	{
	case 'F':
		first_person_ = true;
		break;
	default:
		break;
	}
	if(first_person_)
	{
		Camera* camera = Context::Instance().AppInstance().GetCamera();
		float3 up_ = camera->GetUp();
		float4x4 mat;

		float3 dir =  cam_look_ - cam_pos_;
		float3 left = Math::Cross(dir,up_);
		switch (key_para)
		{
		case 'E':
			left = Math::Normalize(left);

			pitch_angle_ = -Math::PI/180;
			Math::Identity(mat);
			Math::RotationAxis(mat, left, pitch_angle_);

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::TransformNormal(dir, mat);
			dir = Math::Normalize(dir);
			cam_look_ = dir + cam_pos_;
			break;
		case 'C':
			left = Math::Normalize(left);

			pitch_angle_ = Math::PI/180;
			Math::Identity(mat);
			Math::RotationAxis(mat, left, pitch_angle_);

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::TransformNormal(dir, mat);
			dir = Math::Normalize(dir);
			cam_look_ = dir + cam_pos_;
			break;
		case 'X':
			left = float3(0,1,0);

			pitch_angle_ = - Math::PI/180;
			Math::Identity(mat);
			Math::RotationAxis(mat, left, pitch_angle_);

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::TransformNormal(dir, mat);
			dir = Math::Normalize(dir);
			cam_look_ = dir + cam_pos_;
			break;
		case 'Z':
			left = float3(0,1,0);

			pitch_angle_ = Math::PI/180;
			Math::Identity(mat);
			Math::RotationAxis(mat, left, pitch_angle_);

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::TransformNormal(dir, mat);
			dir = Math::Normalize(dir);
			cam_look_ = dir + cam_pos_;
			break;
		case 'W':

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::Normalize(dir);
			dir = dir*speed_;
			cam_look_ = cam_look_ + dir;
			cam_pos_ = cam_pos_ + dir;
			break;
		case 'S':

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::Normalize(dir);
			dir = dir*speed_;
			cam_look_ = cam_look_ - dir;
			cam_pos_ = cam_pos_ - dir;
			break;

		case 'A':

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::Normalize(left);
			dir = dir*speed_;
			cam_look_ = dir + cam_look_;
			cam_pos_ = cam_pos_ + dir;
			break;
		case 'D':

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::Normalize(left);
			dir = dir*speed_;
			cam_look_ = cam_look_ - dir;
			cam_pos_ = cam_pos_ - dir;
			break;
		case 'P':
			speed_ +=0.1;
			break;		
		case 'O':
			speed_ -=0.1;
			break;

		default:
			break;
		}
	}
	
}*/



int main()
{
	 _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	Configure MyConfig;
	MyConfig.LoadConfig("Configure/Config.xml");
	Context::Instance().Setup(MyConfig.GetContextSetting());

	MyApp app;
	app.Initialize();
	app.Run();



	_CrtDumpMemoryLeaks();
	return 0;
}

 