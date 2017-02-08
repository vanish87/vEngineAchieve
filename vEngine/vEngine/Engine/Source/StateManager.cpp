#include "Engine\Header\StateManager.h"
#include "Engine\Header\Context.h"
#include "Engine\Header\ScriptContext.h"

namespace vEngine
{
	StateManager::StateManager(void)
		:current_state_(nullptr)
	{
	}


	StateManager::~StateManager(void)
	{
	}

	void StateManager::Update()
	{
		ScriptContext& script = Context::Instance().GetScriptContext();
		script.RunFile("LuaScript/DebugUpdate.lua");

		if (current_state_ != nullptr)
		{
			//do state update
			current_state_->Update();
		}
	}

	void StateManager::ChangeState( GameState* game_state, StateOP op )
	{
		switch (op)
		{
		case SOP_PUSH:
		{
			game_state->SetParent(current_state_);
			current_state_ = game_state;
			break;
		}
		case SOP_POP:
		{
			CHECK_ASSERT(current_state_ == game_state);
			//TODO: memory leak
			current_state_ = current_state_->GetParent();
			break;
		}
		default:
			break;
		}
	}

	GameState* StateManager::CurrentState()
	{
		return current_state_;
	}

}
