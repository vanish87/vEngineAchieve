#ifndef GAME_EVENT_HANDLER_H_
#define GAME_EVENT_HANDLER_H_

#pragma once
#include "Common/Header/CommonPreDec.h"
#include "Engine/Header/TypeAndFormat.h"


namespace vEngine
{
	class GameEventHanlder
	{
	public:
		GameEventHanlder(void);
		virtual ~GameEventHanlder(void);

		bool Handle(void* UserData, GameEventType Event);

		static GameEventHanlder& NullObject();
		
	};

	class NGameEventHanlder : public GameEventHanlder
	{
	public:
		NGameEventHanlder();
		~NGameEventHanlder();

	};

	

}

#endif

