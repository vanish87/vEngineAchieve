

#include "Engine\Header\Context.h"
#include "Common\Header\Timer.h"

//#include "StartMenu.h"

#include "D3D11\D3DSkyDome.h"
#include "Common\Header\Thread.h"

using namespace vEngine;

class TestThread : public Thread
{
public:
	struct Para
	{
		int num;
	};
	ReturnCode Main(void* para)
	{
		Para* p = static_cast<Para*>(para);
		PRINT("test");
		PRINT(p->num);
		return RCSuccess();
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
	TestThread::Para p[10];
	for (int i = 0; i < 10; ++i)
	{
		p[i].num = i;
		thread[i].Create(&p[i]);
	}

	for (int i = 0; i < 10; ++i)
	{
		thread[i].Join();
	}
	_CrtDumpMemoryLeaks();
	return 0;
}
#endif // !ENABLE_TEST


 