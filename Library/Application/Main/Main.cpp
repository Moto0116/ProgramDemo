/**
 * @file	Main.cpp
 * @brief	エントリポイント
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <Windows.h>
#include "Application\Application.h"

#include "Debugger\Debugger.h"
#include "SmartPointer\SmartPointer.h"

struct Base
{
	int A;
};

struct A : public Base
{
	int B;
};

int WINAPI WinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _szStr, INT _iCmdShow)
{
	Lib::Debugger::CheckMemoryLeak();

	{
		SharedPtr<Base> SBase1 = SharedPtr<Base>();
		SharedPtr<A> SA1 = SharedPtr<A>();
		SharedPtr<Base> SBase2 = SharedPtr<A>();
		SharedPtr<A> SA2;
		SA2 = SA1;
		SBase1 = SA1;

		UniquePtr<Base> UBase1 = UniquePtr<Base>();
		UniquePtr<Base> UBase2 = UniquePtr<A>();
		UniquePtr<Base> UBase3 = Lib::CreateUniquePtr<A>();

		WeakPtr<A> WA1 = SA1;

		{
			SharedPtr<A> SA3 = SA1;
			SharedPtr<Base> SBase3 = SA1;
		}
	}

	return 0;
}

