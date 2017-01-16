

#include "Engine\Header\Context.h"
#include "Common\Header\Timer.h"

//#include "StartMenu.h"

#include "D3D11\D3DSkyDome.h"
#include "Common\Header\Thread.h"

using namespace MocapGE;

class TestThread : public Thread
{
	ReturnCode Main(void* para)
	{
		PRINT("test");
		return Success();
	}
};

#ifdef ENABLE_TEST

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	Configure MyConfig;
	MyConfig.LoadConfig("Configure/Config.xml");
	Context::Instance().Setup(MyConfig.GetContextSetting());
	
	TestThread thread[10];
	for (int i = 0; i < 10; ++i)
	{
		thread[i].Create();
	}


	_CrtDumpMemoryLeaks();
	return 0;
}
#endif // !ENABLE_TEST


 