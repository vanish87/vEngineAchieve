#ifndef GAMESTATE_H_
#define GAMESTATE_H_

#pragma once
#include "Engine/Header/EnginePreDec.h"
#include <windows.h>

namespace vEngine
{
	class GameState
	{
	public:
		GameState(void);
		virtual ~GameState(void);

		void SetParent(GameStateSharedPtr parent){parent_ = parent;};
		GameStateSharedPtr GetParent(){return parent_;};

		virtual void Update() = 0;

		virtual void OnKeyDown(WPARAM key_para)
		{
		}
		virtual void OnKeyUp(WPARAM key_para)
		{
		}
		virtual void OnMouseDown(WPARAM mouse_para,  int x, int y)
		{
		}
		virtual void OnMouseUp(WPARAM mouse_para,  int x, int y)
		{
		}
		virtual void OnMouseMove(WPARAM mouse_para,  int x, int y)
		{
		}

	private:
		GameStateSharedPtr parent_;
	};



}

#endif

