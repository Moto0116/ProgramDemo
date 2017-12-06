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
#include "TypeTraits\TypeTraits.h"
#include <functional>
#include <type_traits>

#include "Factory\Factory.h"
#include "Hash\Hash.h"


#define TEST6

#ifdef TEST


// FuncBaseを基底にしたImplクラスを作成する
// FuncクラスはFuncを保持して状況に合ったImplを作成する

template<typename Return, typename ...Args>
class FunctionBase
{
public:
	FunctionBase(){}
	virtual ~FunctionBase() = 0;
	virtual Return DoCall(Args... _args) = 0;
};

template <typename Return, typename ...Args>
class FunctionImpl : public FunctionBase<Return, Args...>
{
public:
	FunctionImpl(){}
	virtual ~FunctionImpl(){}
	virtual Return DoCall(Args... _args){}
};


template <typename Type>
class GetFunctionImpl;

template <typename Return, typename ...Args>
class GetFunctionImpl<Return __cdecl(Args...)>
{
public:
};

template <typename Return, typename ...Args>
class GetFunctionImpl<Return __stdcall(Args...)>
{
public:
};

template <typename Return, typename ...Args>
class GetFunctionImpl<Return __fastcall(Args...)>
{
public:
};

template <typename Type>
class Function
{
public:

};

int WINAPI WinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _szStr, INT _iCmdShow)
{
}


#endif
#ifdef TEST2
//----------------------------------------------------------------------
// FunctionBase
//----------------------------------------------------------------------

template <typename Ret>
class FunctionBase;

template <typename Ret, typename ...Types>
class FunctionBase<Ret __stdcall(Types...)>
{
public:
	using ReturnValue = Ret;
	using Type = ReturnValue __stdcall (Types...);

	void Set(Type _pFunc) { m_pFunc = _pFunc; }
	ReturnValue DoCall(Types... Args) { return m_pFunc(Args...); }

protected:
	Type* m_pFunc;
};

template <typename Ret, typename ...Types>
class FunctionBase<Ret __cdecl(Types...)>
{
public:
	using ReturnValue = Ret; 
	using Type = ReturnValue __cdecl (Types...);

	void Set(Type _pFunc) { m_pFunc = _pFunc; }
	ReturnValue DoCall(Types... Args) { return m_pFunc(Args...); }

protected:
	Type* m_pFunc;
};

template <typename Ret, typename ...Types>
class FunctionBase<Ret __fastcall(Types...)>
{
public:
	using ReturnValue = Ret;
	using Type = ReturnValue __fastcall (Types...);

	void Set(Type _pFunc) { m_pFunc = _pFunc; }
	ReturnValue DoCall(Types... Args) { return m_pFunc(Args...); }

protected:
	Type* m_pFunc;
};

template <typename Ret, typename ClassType, typename ...Types>
class FunctionBase<Ret(__thiscall ClassType::*) (Types...)>
{
public:
	using ReturnValue = Ret;
	using Type = ReturnValue(__thiscall ClassType::*) (Types...);

	void Set(Ret(ClassType::*_pFunc)(Types...))	{ m_pFunc = _pFunc; }
	ReturnValue DoCall(ClassType* Type, Types... Args) { return (Type->*m_pFunc)(Args...); }

protected:
	Type m_pFunc;
};


//----------------------------------------------------------------------
// FunctionImpl
//----------------------------------------------------------------------

template <typename Ret>
class FunctionImpl;

template <typename Ret, typename ...Types>
class FunctionImpl<Ret __stdcall(Types...)>
{
public:
	using type = FunctionBase<Ret __stdcall(Types...)>;
};

template <typename Ret, typename ...Types>
class FunctionImpl<Ret __cdecl(Types...)>
{
public:
	using type = FunctionBase<Ret __cdecl(Types...)>;
};

template <typename Ret, typename ...Types>
class FunctionImpl<Ret __fastcall(Types...)>
{
public:
	using type = FunctionBase<Ret __fastcall(Types...)>;
};

template <typename Ret, typename ClassType, typename ...Types>
class FunctionImpl<Ret(__thiscall ClassType::*)(Types...)>
{
public:
	using type = FunctionBase<Ret(__thiscall ClassType::*)(Types...)>;
};


//----------------------------------------------------------------------
// Function
//----------------------------------------------------------------------

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

private:
	class HolderBase
	{
	public:
		HolderBase(){}
		~HolderBase(){}
	};

	template <typename T>
	class Holder : public HolderBase
	{
	public:
		Holder(T _data) : m_Data(_data) {}
		~Holder() {}

	private:
		T m_Data;
	};

	HolderBase* m_pHolders;
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

void __stdcall TestPrint(int a, int b)
{
	Lib::Debugger::OutputDebugLog("\nvoid __stdcall TestPrint(a = %d, b = %d)\n", a, b);
}

void __cdecl TestPrint2(int a, int b)
{
	Lib::Debugger::OutputDebugLog("\nvoid __cdecl TestPrint2(a = %d, b = %d)\n", a, b);
}

void __fastcall TestPrint3(int a, int b)
{
	Lib::Debugger::OutputDebugLog("\nvoid __fastcall TestPrint3(a = %d, b = %d)\n", a, b);
}

int WINAPI WinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _szStr, INT _iCmdShow)
{
	// std.
	std::function<void (int, int)> Func;
	Func = TestPrint;
	Func(1, 2);
	Func = TestPrint2;
	Func(1, 2);
	Func = TestPrint3;
	Func(1, 2);


	// 自作.
	Class Object;
	Function<void(Class::*)()> Func2;
	Func2 = &Class::TestPrint;
	Func2(&Object);

	Function<void __stdcall(int, int)> Func3;
	Func3 = &TestPrint;
	Func3(1, 2);

	Function<void __cdecl(int, int)> Func4;
	Func4 = &TestPrint2;
	Func4(1, 2);

	Function<void __fastcall(int, int)> Func5;
	Func5 = &TestPrint3;
	Func5(1, 2);

	return 0;
}

#endif
#ifdef TEST3

struct DATA
{
	int a1[5000];
};

void Test()
{
	Lib::TlsfAllocator tlsf;
	tlsf.Initialize(200000000);

	// newとの速度比較テスト.
	Lib::Debugger::DebugTimer Timer;
	LONGLONG Time1 = 0;
	LONGLONG Time2 = 0;

	DATA* p[20000];

	Timer.StartTimer();
	for (int i = 0; i < 20000; i++)
	{
		p[i] = new DATA;
		delete p[i];
	}
	Timer.EndTimer();
	Time1 = Timer.GetMilliSecond();

	Timer.StartTimer();
	for (int i = 0; i < 20000; i++)
	{
		p[i] = tlsf.Allocate<DATA>();
		tlsf.DeAllocate(p[i]);
	}
	Timer.EndTimer();
	Time2 = Timer.GetMilliSecond();

	Lib::Debugger::OutputDebugLog("\n");
	Lib::Debugger::OutputDebugLog("new delete - %dms\n", Time1);
	Lib::Debugger::OutputDebugLog("Allocate - %dms\n", Time2);

	tlsf.Finalize();
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

int WINAPI WinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _szStr, INT _iCmdShow)
{
	Lib::Debugger::CheckMemoryLeak();

	Test();

	while (1);
}

#endif
#ifdef TEST4

int WINAPI WinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _szStr, INT _iCmdShow)
{
	// イテレータがあるかないかのチェック.
	StaticMyAssert(Lib::TypeTraits::HasIterator<std::vector<int>>::Value == false, "Error : iterator is not Found");
	//StaticMyAssert(Lib::TypeTraits::HasIterator<int>::Value == false, "Error : iterator is not Found");

	// ポインタ型かのチェック.
	StaticMyAssert(Lib::TypeTraits::IsPointer<int*>::Value == false, "Error : Type Not Pointer");
	//StaticMyAssert(Lib::TypeTraits::IsPointer<int>::Value == false, "Error : Type Not Pointer");

}

#endif 
#ifdef TEST5
int WINAPI WinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _szStr, INT _iCmdShow)
{
	Application App;

	if (!App.Initialize())
	{
		return -1;
	}

	App.Run();
	App.Finalize();

	return 0;
}
#endif 
#ifdef TEST6 


#include <stdio.h>
#include <stdint.h>

class Base
{
public:
	Base(){}
	~Base(){}
	virtual int GetID(){ return 0; }
};


int A_ID = 1;

class A : public Base
{
public:
	A(){}
	~A(){}
	int GetID(){ return A_ID; }
};

Base* CreateA()
{
	return new A();
}


int B_ID = 2;

class B : public Base
{
public:
	B(){}
	~B(){}
	int GetID(){ return B_ID; }
};

Base* CreateB()
{
	return new B();
}



int C_ID = 3;

class C : public Base
{
public:
	C(){}
	~C(){}
	int GetID(){ return C_ID; }
};

Base* CreateC()
{
	return new C();
}


bool IsRegisterA = Lib::Factory<Base, int>::GetInstance()->Register(&CreateA, A_ID);
bool IsRegisterB = Lib::Factory<Base, int>::GetInstance()->Register(&CreateB, B_ID);
bool IsRegisterC = Lib::Factory<Base, int>::GetInstance()->Register(&CreateC, C_ID);

void main()
{
	Base* pBaseA = Lib::Factory<Base, int>::GetInstance()->Create(A_ID);
	Base* pBaseB = Lib::Factory<Base, int>::GetInstance()->Create(B_ID);
	Base* pBaseC = Lib::Factory<Base, int>::GetInstance()->Create(C_ID);

	printf("%d\n", pBaseA->GetID());
	printf("%d\n", pBaseB->GetID());
	printf("%d\n", pBaseC->GetID());

	delete pBaseC;
	delete pBaseB;
	delete pBaseA;

	while (1);
}


#endif
