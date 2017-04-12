/**
 * @file	Application.h
 * @brief	アプリケーションクラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "Application.h"

#include "Debugger\Debugger.h"
#include "DirectX11\GraphicsDevice\GraphicsDevice.h"
#include "InputDeviceManager\InputDeviceManager.h"
#include "Scene\GameScene\GameScene.h"


//----------------------------------------------------------------------
// Static Private Variables
//----------------------------------------------------------------------
const int Application::m_WindowWidth = 1600;
const int Application::m_WindowHeight = 900;
const DWORD Application::m_WindowStyle = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX | WS_VISIBLE;


//----------------------------------------------------------------------
// Constructor	Destructor
//----------------------------------------------------------------------
Application::Application() :
	m_pMainWindow(NULL),
	m_pSceneManager(NULL),
	m_pGameScene(NULL)
{
}

Application::~Application()
{
}


//----------------------------------------------------------------------
// Public Functions
//----------------------------------------------------------------------
bool Application::Initialize()
{
	Lib::Debugger::CheckMemoryLeak();	// メモリリークチェック関数

	if (!CreateMainWindow())
	{
		return false;
	}

	if (!CreateGraphicsDevice())
	{
		ReleaseMainWindow();
		return false;
	}

	if (!CreateInputDevice())
	{
		ReleaseGraphicsDevice();
		ReleaseMainWindow();
		return false;
	}

	if (!CreateSceneManager())
	{
		ReleaseInputDevice();
		ReleaseGraphicsDevice();
		ReleaseMainWindow();
		return false;
	}

	return true;
}

void Application::Finalize()
{
	ReleaseSceneManager();
	ReleaseInputDevice();
	ReleaseGraphicsDevice();
	ReleaseMainWindow();
}

void Application::Run()
{
	while (1)
	{
		if (!m_pMainWindow->Update())
		{
			if (m_pSceneManager->Update())
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
}


//----------------------------------------------------------------------
// Private Functions
//----------------------------------------------------------------------
bool Application::CreateMainWindow()
{
	m_pMainWindow = new Lib::Window();
	if (!m_pMainWindow->Initialize(TEXT("ProgramDemo"), m_WindowWidth, m_WindowHeight, m_WindowStyle))
	{
		OutputErrorLog("メインウィンドウ生成に失敗しました");
		SafeDelete(m_pMainWindow);
		return false;
	}

	return true;
}

bool Application::CreateGraphicsDevice()
{
	SINGLETON_CREATE(Lib::GraphicsDevice);
	if (!SINGLETON_INSTANCE(Lib::GraphicsDevice)->Initialize(m_pMainWindow->GetWindowHandle()))
	{
		OutputErrorLog("グラフィックデバイス生成に失敗しました");
		SINGLETON_DELETE(Lib::GraphicsDevice);
		return false;
	}

	return true;
}

bool Application::CreateInputDevice()
{
	SINGLETON_CREATE(Lib::InputDeviceManager);
	if (!SINGLETON_INSTANCE(Lib::InputDeviceManager)->Initialize(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetMainWindowHandle()))
	{
		OutputErrorLog("入力デバイス管理クラス生成に失敗しました");
		SINGLETON_DELETE(Lib::InputDeviceManager);
		return false;
	}

	if (!SINGLETON_INSTANCE(Lib::InputDeviceManager)->CreateDevice(Lib::InputDeviceManager::KEYDEVICE_TYPE))
	{
		OutputErrorLog("キーデバイス生成に失敗しました");
		SINGLETON_DELETE(Lib::InputDeviceManager);
		return false;
	}

	if (!SINGLETON_INSTANCE(Lib::InputDeviceManager)->CreateDevice(Lib::InputDeviceManager::MOUSEDEVICE_TYPE))
	{
		OutputErrorLog("マウスデバイス生成に失敗しました");
		SINGLETON_DELETE(Lib::InputDeviceManager);
		return false;
	}

	return true;
}

bool Application::CreateSceneManager()
{
	m_pSceneManager = new Lib::SceneManager();
	if(!m_pSceneManager->Initialize())
	{
		SafeDelete(m_pSceneManager);
		return false;
	}

	m_pGameScene = new GameScene(GAME_SCENE_ID);

	m_pSceneManager->AddScene(m_pGameScene);		// 管理オブジェクトに追加
	m_pSceneManager->SetEntryScene(m_pGameScene);	// エントリシーンとして設定

	return true;
}

void Application::ReleaseMainWindow()
{
	if (m_pMainWindow != NULL)
	{
		m_pMainWindow->Finalize();
		SafeDelete(m_pMainWindow);
	}
}

void Application::ReleaseGraphicsDevice()
{
	if (SINGLETON_INSTANCE(Lib::GraphicsDevice) != NULL)
	{
		SINGLETON_INSTANCE(Lib::GraphicsDevice)->Finalize();
		SINGLETON_DELETE(Lib::GraphicsDevice);
	}
}

void Application::ReleaseInputDevice()
{
	if (SINGLETON_INSTANCE(Lib::InputDeviceManager) != NULL)
	{
		SINGLETON_INSTANCE(Lib::InputDeviceManager)->ReleaseDevice(Lib::InputDeviceManager::MOUSEDEVICE_TYPE);
		SINGLETON_INSTANCE(Lib::InputDeviceManager)->ReleaseDevice(Lib::InputDeviceManager::KEYDEVICE_TYPE);

		SINGLETON_INSTANCE(Lib::InputDeviceManager)->Finalize();
		SINGLETON_DELETE(Lib::InputDeviceManager);
	}
}

void Application::ReleaseSceneManager()
{
	if (m_pSceneManager != NULL)
	{
		m_pSceneManager->DeleteScene(m_pGameScene);
		SafeDelete(m_pGameScene);

		m_pSceneManager->Finalize();
		SafeDelete(m_pSceneManager);
	}
}

