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

#include "MemoryAllocator\TlsfAllocator\TlsfAllocator.h"
#include "Debugger\Debugger.h"
#include "SmartPointer\SmartPointer.h"

struct DATA
{
	int a1[5000];
};
void Test()
{
	// newとの速度比較テスト.
	Lib::Debugger::DebugTimer Timer;
	LONGLONG Time1 = 0;
	LONGLONG Time2 = 0;

	Lib::TlsfAllocator Tlsf;
	Tlsf.Initialize(200000000);

	int* pA[10000];

	Timer.StartTimer();
	for (int i = 0; i < 10000; i++)
	{
		pA[i] = new int;
		delete pA[i];
	}
	Timer.EndTimer();
	Time1 = Timer.GetMilliSecond();

	Timer.StartTimer();
	for (int i = 0; i < 10000; i++)
	{
		pA[i] = Tlsf.Allocate<int>();
	}
	for (int i = 0; i < 10000; i++)
		Tlsf.DeAllocate(pA[i]);

	Timer.EndTimer();
	Time2 = Timer.GetMilliSecond();

	Tlsf.Finalize();
}
void Test2()
{
	// メモリリークテスト.
	Lib::TlsfAllocator tlsf;
	tlsf.Initialize(1000000);

	int* pA[10];
	double* pB[10];
	DATA* pC[10];

	for (int i = 0; i < 10; i++)
	{
		pA[i] = tlsf.Allocate<int>();
		pB[i] = tlsf.Allocate<double>();
		pC[i] = tlsf.Allocate<DATA>();
	}

	for (int i = 0; i < 10; i++)
	{
		tlsf.DeAllocate(pC[i]);
		tlsf.DeAllocate(pB[i]);
		tlsf.DeAllocate(pA[i]);
	}

	tlsf.Finalize();
}

#include <functional>




template <typename Ret, typename ...Types>
class StdFunctionBase
{
public:
	using ReturnValue = Ret;
	using Type = Ret __stdcall (Types...);

	void Set(Type _pFunc)
	{
		m_pFunc = _pFunc;
	}

	ReturnValue DoCall(Types... Args)
	{
		return m_pFunc(Args...);
	}

protected:
	Type* m_pFunc;

};

template <typename Ret, typename ...Types>
class FunctionBase
{
public:
	using ReturnValue = Ret; 
	using Type = Ret __cdecl (Types...);

	void Set(Type _pFunc)
	{
		m_pFunc = _pFunc;
	}

	ReturnValue DoCall(Types... Args)
	{
		return m_pFunc(Args...);
	}

protected:
	Type* m_pFunc;

};


template <typename Ret, typename ...Types>
class FastFunctionBase
{
public:
	using ReturnValue = Ret;
	using Type = Ret __fastcall (Types...);

	void Set(Type _pFunc)
	{
		m_pFunc = _pFunc;
	}

	ReturnValue DoCall(Types... Args)
	{
		return m_pFunc(Args...);
	}

protected:
	Type* m_pFunc;

};


template <typename Ret, typename ClassType, typename ...Types>
class ClassFunctionBase
{
public:
	using ReturnValue = Ret;

	void Set(Ret(ClassType::*_pFunc)(Types...))
	{
		m_pFunc = _pFunc;
	}

	ReturnValue DoCall(ClassType* Type, Types... Args)
	{
		return (Type->*m_pFunc)(Args...);
	}

protected:
	ReturnValue(ClassType::*m_pFunc)(Types...);

};


template <typename Ret>
class FunctionImpl;

template <typename Ret, typename ...Types>
class FunctionImpl<Ret __stdcall(Types...)>
{
public:
	using type = StdFunctionBase<Ret, Types...>;
};

template <typename Ret, typename ...Types>
class FunctionImpl<Ret __cdecl(Types...)>
{
public:
	using type = FunctionBase<Ret, Types...>;
};

template <typename Ret, typename ...Types>
class FunctionImpl<Ret __fastcall(Types...)>
{
public:
	using type = FastFunctionBase<Ret, Types...>;
};

template <typename Ret, typename ClassType, typename ...Types>
class FunctionImpl<Ret(__thiscall ClassType::*)(Types...)>
{
public:
	using type = ClassFunctionBase<Ret, ClassType, Types...>;
};


template <typename Type>
class Function : public FunctionImpl<Type>::type
{
public:
	using ReturnValue = typename FunctionImpl<Type>::type::ReturnValue;

	template <typename ...Types>
	Function operator=(Types... _type)
	{
		Set(_type...);
		return (*this);
	}

	template <typename ...Types>
	ReturnValue operator()(Types... Args)
	{
		return DoCall(Args...);
	}
};




class Class
{
public:
	void TestPrint()
	{
		Lib::Debugger::OutputDebugLog("\nClass::TestPrint()\n");
	}

	void TestPrint2(int i, int i2)
	{
		Lib::Debugger::OutputDebugLog("\nClass::TestPrint2(int,int)\n");
	}
};



void __stdcall TestPrint()
{
	Lib::Debugger::OutputDebugLog("\nvoid TestPrint()\n");
}

void __cdecl TestPrint2()
{
	Lib::Debugger::OutputDebugLog("\nvoid TestPrint2()\n");
}

void __fastcall TestPrint3()
{
	Lib::Debugger::OutputDebugLog("\nvoid TestPrint3()\n");
}

int WINAPI WinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _szStr, INT _iCmdShow)
{
	Lib::Debugger::CheckMemoryLeak();

	Class Object;

	//std::function<void()> Func;
	//Func = std::bind(&Class::TestPrint, &Object);
	//Func();

	Function<void(Class::*)()> Func2;
	Func2 = &Class::TestPrint;
	Func2(&Object);

	Function<void __stdcall()> Func3;
	Func3 = &TestPrint;

	Function<void __cdecl()> Func4;
	Func4 = &TestPrint2;
	Func4();

	Function<void __fastcall()> Func5;
	Func5 = &TestPrint3;



	return 0;
}

