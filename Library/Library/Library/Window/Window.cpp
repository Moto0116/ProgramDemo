/**
 * @file	Window.cpp
 * @brief	ウィンドウクラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "Window.h"


namespace Lib
{
	//----------------------------------------------------------------------
	// Global Function
	//----------------------------------------------------------------------
	LRESULT CALLBACK WindowProc(HWND _hwnd, UINT _message, WPARAM _wParam, LPARAM _lParam)
	{
		switch (_message)
		{
		case WM_CLOSE:
			DestroyWindow(_hwnd);
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
			break;
		case WM_KEYDOWN:
			switch (static_cast<CHAR>(_wParam))
			{
			case VK_ESCAPE:
				PostQuitMessage(0);
				return 0;
				break;
			}
			break;
		}

		return DefWindowProc(_hwnd, _message, _wParam, _lParam);
	}


	//----------------------------------------------------------------------
	// Constructor	Destructor
	//----------------------------------------------------------------------
	Window::Window() :
		m_hWnd(NULL),
		m_hWndParent(NULL)
	{
	}
	
	Window::~Window()
	{
		Finalize();
	}
	
	
	//----------------------------------------------------------------------
	// Public Functions
	//----------------------------------------------------------------------
	bool Window::Initialize(LPCTSTR _pWindowName, int _windowWidth, int _windowHeight, DWORD _style, HWND _hWndParent)
	{
		m_hWndParent = _hWndParent;
	
		WNDCLASSEX  WndClass;
		WndClass.cbSize = sizeof(WndClass);
		WndClass.style = CS_HREDRAW | CS_VREDRAW;
		WndClass.lpfnWndProc = WindowProc;
		WndClass.cbClsExtra = 0;
		WndClass.cbWndExtra = 0;
		WndClass.hInstance = GetModuleHandle(NULL);
		WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		WndClass.lpszMenuName = NULL;
		WndClass.lpszClassName = _pWindowName;
		WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
		RegisterClassEx(&WndClass);
	
		m_hWnd = CreateWindow(
			_pWindowName,
			_pWindowName,
			_style,
			0,
			0,
			_windowWidth,
			_windowHeight,
			_hWndParent,
			NULL,
			GetModuleHandle(NULL),
			NULL);
	
		if (m_hWnd == NULL)
		{
			return false;	// ウィンドウ作成に失敗
		}
	
		ShowWindow(m_hWnd, SW_SHOW);
		UpdateWindow(m_hWnd);
		SetWindowText(m_hWnd, _pWindowName);
	
		ZeroMemory(&m_WindowMsg, sizeof(m_WindowMsg));

		return true;
	}

	void Window::Finalize()
	{
		if (m_hWnd != NULL)
		{
			SendMessage(m_hWnd, WM_CLOSE, 0, 0);
		}
	}

	bool Window::Update()
	{
		if (m_WindowMsg.message != WM_QUIT)
		{
			if (PeekMessage(&m_WindowMsg, NULL, 0U, 0U, PM_REMOVE))
			{
				TranslateMessage(&m_WindowMsg);
				DispatchMessage(&m_WindowMsg);
			}

			return false;
		}

		m_hWnd = NULL;

		return true;
	}
}

