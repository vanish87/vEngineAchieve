#ifndef STATEMANAGER_H_
#define STATEMANAGER_H_

#pragma once
#include <vector>
#include "Engine/Header/EnginePreDec.h"
#include "TypeAndFormat.h"

namespace vEngine
{
	class StateManager
	{
	public:
		StateManager(void);
		virtual ~StateManager(void);

		void Update();
		void ChangeState(GameStateSharedPtr game_state, StateOP op);
		GameStateSharedPtr CurrentState();

	private:
		std::vector<GameStateSharedPtr> states_;
		GameStateSharedPtr current_state_;

	};

}
#endif
