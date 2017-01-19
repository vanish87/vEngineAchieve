#ifndef SCENEMANAGER_H_
#define SCENEMANAGER_H_

#pragma once
#include <vector>
#include "Common\Header\CommonPreDec.h"


#include "Engine\Header\RenderElement.h"
#include "Engine\Header\SceneObject.h"
#include "Engine\Header\Light.h"

namespace vEngine
{
	class SceneManager
	{
	public:
		SceneManager(void);
		~SceneManager(void);

		void AddSceneObject(SceneObject* scene_object);
		void AddLight(Light* light);

		void Flush();
		void Update();
		std::vector<Light*> GetLights();
		std::vector<RenderElement*> GetRenderList(){return render_list_;};
	protected:
		std::vector<RenderElement*> render_list_;
		std::vector<SceneObject*> scene_object_list;

		std::vector<Light*> lights_;
	};

}
#endif

