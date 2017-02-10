#ifndef ENGINE_PREDEC_H_
#define ENGINE_PREDEC_H_

//shared pointer
#include <memory>

namespace vEngine
{
	#define CLASS_AND_SHARED_POINTER(name) \
	class name;\
	typedef std::shared_ptr<name> name##SharedPtr;

	#define STRUCT_AND_SHARED_POINTER(name) \
	struct name;\
	typedef std::shared_ptr<name> name##SharedPtr;

	CLASS_AND_SHARED_POINTER(App)
	class Window;

	class Texture;
	class RenderState;
	CLASS_AND_SHARED_POINTER(RenderFactory)
	class RenderEngine;
	class RenderView;
	class RenderLayout;
	class RenderElement;
	class RenderBuffer;
	CLASS_AND_SHARED_POINTER(FrameBuffer)
	class Mesh;
	class Model;
	class DeferredRendering;
	class ShaderObject;
	class PostProcess;

	CLASS_AND_SHARED_POINTER(GameObject)
	class SceneObject;
	class SceneManager;

	CLASS_AND_SHARED_POINTER(GameState)
	class StateManager;

	class Viewport;
	class Camera;
	class Frustum;
	
	class Context;

	class PointLight;
	class SpotLight;

	class Configure;

	CLASS_AND_SHARED_POINTER(ScriptContext)
	STRUCT_AND_SHARED_POINTER(ScriptFuctionDescription)

	#undef CLASS_AND_SHARED_POINTER
	#undef STRUCT_AND_SHARED_POINTER
}

#endif