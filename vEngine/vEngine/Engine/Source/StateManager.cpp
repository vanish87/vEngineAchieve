#include "StateManager.h"

namespace MocapGE
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
		if(current_state_ != nullptr)
			//do state update
			current_state_->Update();
	}

	void StateManager::ChangeState( GameState* game_state, StateOP op )
	{
		if(op == SOP_PUSH)
		{
			game_state->SetParent(current_state_);
			current_state_ = game_state;
		}
		else if (op == SOP_POP && current_state_ == game_state)
		{
			assert(current_state_ == game_state);
			//TODO: memory leak
			current_state_ = current_state_->GetParent();
		}
	}

	GameState* StateManager::CurrentState()
	{
		return current_state_;
	}

}
