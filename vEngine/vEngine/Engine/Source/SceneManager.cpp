#include "SceneManager.h"
#include "Context.h"

//TODO: bad!! Write a independent shader object loader
#include "D3D11\D3DShaderobject.h"
#include "PostProcess.h"

namespace MocapGE
{

	SceneManager::SceneManager(void)
	{
	}


	SceneManager::~SceneManager(void)
	{
	}

	void SceneManager::Flush()
	{
		if(render_list_.empty()) 
		{
			Context::Instance().GetRenderFactory().GetRenderEngine().RenderFrameBegin();
			Context::Instance().GetRenderFactory().GetRenderEngine().RenderFrameEnd();
			Context::Instance().GetRenderFactory().GetRenderEngine().SwapBuffers();
			return;
		}
		RenderEngine* render_engine = &Context::Instance().GetRenderFactory().GetRenderEngine();
		if(render_engine->GetRenderSetting().deferred_rendering)
		{	
			Context::Instance().GetRenderFactory().GetRenderEngine().GetDeferredRendering()->Update();		
		}	
		else
		{
			//do FR
			//Forward Rendering
			//=======================================================================================================================
			//set lights parameters
			/*std::vector<Light*> lights = Context::Instance().GetSceneManager().GetLights();
			RenderBuffer* lights_buffer = Context::Instance().GetRenderFactory().GetRenderEngine().GetLightsBuufer();
			LightStruct* l = static_cast<LightStruct*>(lights_buffer->Map(AT_CPU_WRITE));
			for (size_t i =0; i< lights.size(); i++)
			{
				l[i].color = lights[i]->GetColor();
				l[i].position = static_cast<PointLight*>(lights[i])->GetPos();
			}
			lights_buffer->UnMap();

			render_engine->BindFrameBuffer(render_engine->CurrentFrameBuffer());
			Context::Instance().GetRenderFactory().GetRenderEngine().RenderFrameBegin();
			std::vector<RenderElement*>::iterator re;
			for(re = render_list_.begin() ; re < render_list_.end(); re++)
			{
				(*re)->SetRenderParameters();
				//(*re)->GetShaderObject()->Apply(0);
				(*re)->Render(0);
				(*re)->EndRender();
			}
			Context::Instance().GetRenderFactory().GetRenderEngine().RenderFrameEnd();
			Context::Instance().GetRenderFactory().GetRenderEngine().SwapBuffers();*/
			//Forward Rendering End
			//========================================================================================================================
		}

	}

	void SceneManager::Update()
		//do scene cull here
	{
		render_list_.clear();
		std::vector<SceneObject*>::iterator so;
		for(so = scene_object_list.begin() ; so < scene_object_list.end(); so++)
		{
			(*so)->Update();//do update aabb and set model_matrix equals to its render_element_'s
			if((*so)->Visiable())
				render_list_.push_back((*so)->GetRenderElement());
		}
	}

	void SceneManager::AddSceneObject( SceneObject* scene_object )
	{
		scene_object_list.push_back(scene_object);
	}

	std::vector<Light*> SceneManager::GetLights()
	{
		return lights_;
	}

	void SceneManager::AddLight( Light* light )
	{
		lights_.push_back(light);
	}

}
