#ifndef GAMESTATE_H_
#define GAMESTATE_H_

#pragma once
#include "Common\Header\CommonPreDec.h"
#include <windows.h>

namespace MocapGE
{
	class GameState
	{
	public:
		GameState(void);
		~GameState(void);

		void SetParent(GameState* parent){parent_ = parent;};
		GameState* GetParent(){return parent_;};

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
		GameState* parent_;
	};



}

#endif

