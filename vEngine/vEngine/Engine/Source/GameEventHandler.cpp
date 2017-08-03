#include "Engine/Header/GameEventHandler.h"

namespace vEngine
{
	GameEventHanlder::GameEventHanlder(void)
	{
	}

	GameEventHanlder::~GameEventHanlder(void)
	{
	}

	bool GameEventHanlder::Handle(void* UserData, GameEventType Event)
	{
		return false;
	}

	GameEventHanlder& GameEventHanlder::NullObject()
	{
		static NGameEventHanlder Instance;
		return Instance;
	}

	NGameEventHanlder::NGameEventHanlder()
		:GameEventHanlder()
	{

	}

	NGameEventHanlder::~NGameEventHanlder()
	{

	}

}
