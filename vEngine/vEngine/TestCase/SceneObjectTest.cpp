#include "Engine\Header\Context.h"

using namespace vEngine;

#ifdef ENABLE_SCENE_TEST
int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	Configure MyConfig;
	MyConfig.LoadConfig("Configure/Config.xml");
	Context::Instance().Setup(MyConfig.GetContextSetting());

	SceneObject testoject, testoject1;
	testoject.AddComponent(&testoject1);

	vEngine::UUID RefID = testoject1.id();
	vEngine::UUID nullID = GameObject::NullObject().id();
	//vEngine::UUID newID;//error here
	CHECK_ASSERT(testoject1 == testoject.FindComponentByUUID(RefID));
	CHECK_ASSERT(testoject1 != testoject.FindComponentByUUID(nullID));
	CHECK_ASSERT(GameObject::NullObject() == testoject.FindComponentByUUID(nullID));
	CHECK_ASSERT(GameObject::NullObject() == testoject.FindComponentByUUID(testoject.id()));

	DebugTracking::GetInstance().PrintALL();
	_CrtDumpMemoryLeaks();
	return 0;
}
#endif // !ENABLE_TEST


 