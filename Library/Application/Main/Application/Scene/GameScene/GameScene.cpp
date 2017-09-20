/**
 * @file	GameScene.cpp
 * @brief	ゲームシーンクラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "GameScene.h"

#include "Debugger\Debugger.h"
#include "DirectX11\FbxFileManager\Dx11FbxFileManager.h"
#include "DirectX11\GraphicsDevice\Dx11GraphicsDevice.h"
#include "DirectX11\ShaderManager\Dx11ShaderManager.h"
#include "DirectX11\TextureManager\Dx11TextureManager.h"
#include "InputDeviceManager\InputDeviceManager.h"
#include "SoundDevice\SoundDevice.h"
#include "SoundManager\SoundManager.h"
#include "TaskManager\TaskBase\UpdateTask\UpdateTask.h"
#include "TaskManager\TaskBase\DrawTask\DrawTask.h"
#include "Task\DepthDrawTask\DepthDrawTask.h"
#include "Task\MapDrawTask\MapDrawTask.h"
#include "Task\CubeMapDrawTask\CubeMapDrawTask.h"
#include "Task\ReflectMapDrawTask\ReflectMapDrawTask.h"


//----------------------------------------------------------------------
// Private Functions
//----------------------------------------------------------------------
const D3DXVECTOR2 GameScene::m_DefaultFontSize = D3DXVECTOR2(16, 32);
const D3DXCOLOR	GameScene::m_DefaultFontColor = 0xffffffff;


//----------------------------------------------------------------------
// Constructor	Destructor
//----------------------------------------------------------------------
GameScene::GameScene(int _sceneId) :
	SceneBase(_sceneId),
	m_UpdateTime(0),
	m_DrawTime(0)
{
}

GameScene::~GameScene()
{
	Finalize();
}


//----------------------------------------------------------------------
// Public Functions
//----------------------------------------------------------------------
bool GameScene::Initialize()
{
	SINGLETON_CREATE(Lib::UpdateTaskManager);
	SINGLETON_CREATE(Lib::Draw2DTaskManager);
	SINGLETON_CREATE(Lib::Draw3DTaskManager);
	SINGLETON_CREATE(CubeMapDrawTaskManager);
	SINGLETON_CREATE(ReflectMapDrawTaskManager);
	SINGLETON_CREATE(DepthDrawTaskManager);
	SINGLETON_CREATE(MapDrawTaskManager);


	SINGLETON_CREATE(Lib::Dx11::FbxFileManager);
	if (!SINGLETON_INSTANCE(Lib::Dx11::FbxFileManager)->Initialize(SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)))
	{
		OutputErrorLog("Fbxモデル管理クラスの生成に失敗しました");
		return false;
	}

	SINGLETON_CREATE(Lib::Dx11::ShaderManager);
	if (!SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->Initialize(SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)))
	{
		OutputErrorLog("シェーダー管理クラスの生成に失敗しました");
		return false;
	}

	SINGLETON_CREATE(Lib::Dx11::TextureManager);
	if (!SINGLETON_INSTANCE(Lib::Dx11::TextureManager)->Initialize(SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)))
	{
		OutputErrorLog("テクスチャ管理クラスの生成に失敗しました");
		return false;
	}

	SINGLETON_CREATE(Lib::SoundDevice);
	if (!SINGLETON_INSTANCE(Lib::SoundDevice)->Initialize(
		SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetMainWindowHandle()))
	{
		OutputErrorLog("サウンドデバイスの生成に失敗しました");
		return false;
	}

	SINGLETON_CREATE(Lib::SoundManager);
	if (!SINGLETON_INSTANCE(Lib::SoundManager)->Initialize(SINGLETON_INSTANCE(Lib::SoundDevice)))
	{
		OutputErrorLog("サウンド管理クラスの生成に失敗しました");
		return false;
	}

	m_pObjectManager = new ObjectManager();
	if (!m_pObjectManager->Initialize())
	{
		OutputErrorLog("オブジェクト管理クラスの生成に失敗しました");
		return false;
	}

	m_pFont = new Lib::Dx11::Font();
	if (!m_pFont->Initialize(SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)))
	{
		OutputErrorLog("フォントオブジェクトの生成に失敗しました");
		return false;
	}

	if (!m_pFont->CreateVertexBuffer(&m_DefaultFontSize, &m_DefaultFontColor))
	{
		OutputErrorLog("フォントオブジェクトの頂点バッファの生成に失敗しました");
		return false;
	}

	m_State = UPDATE_STATE;

	return true;
}

void GameScene::Finalize()
{
	if (m_pFont != nullptr)
	{
		m_pFont->ReleaseVertexBuffer();
		m_pFont->Finalize();
		SafeDelete(m_pFont);
	}

	if (m_pObjectManager != nullptr)
	{
		m_pObjectManager->Finalize();
		SafeDelete(m_pObjectManager);
	}


	if (SINGLETON_INSTANCE(Lib::SoundManager) != nullptr)
	{
		SINGLETON_INSTANCE(Lib::SoundManager)->Finalize();
		SINGLETON_DELETE(Lib::SoundManager);
	}

	if (SINGLETON_INSTANCE(Lib::SoundDevice) != nullptr)
	{
		SINGLETON_INSTANCE(Lib::SoundDevice)->Finalize();
		SINGLETON_DELETE(Lib::SoundDevice);
	}

	if (SINGLETON_INSTANCE(Lib::Dx11::TextureManager) != nullptr)
	{
		SINGLETON_INSTANCE(Lib::Dx11::TextureManager)->Finalize();
		SINGLETON_DELETE(Lib::Dx11::TextureManager);
	}

	if (SINGLETON_INSTANCE(Lib::Dx11::ShaderManager) != nullptr)
	{
		SINGLETON_INSTANCE(Lib::Dx11::ShaderManager)->Finalize();
		SINGLETON_DELETE(Lib::Dx11::ShaderManager);
	}

	if (SINGLETON_INSTANCE(Lib::Dx11::FbxFileManager) != nullptr)
	{
		SINGLETON_INSTANCE(Lib::Dx11::FbxFileManager)->Finalize();
		SINGLETON_DELETE(Lib::Dx11::FbxFileManager);
	}

	SINGLETON_DELETE(MapDrawTaskManager);
	SINGLETON_DELETE(DepthDrawTaskManager);
	SINGLETON_DELETE(ReflectMapDrawTaskManager);
	SINGLETON_DELETE(CubeMapDrawTaskManager);
	SINGLETON_DELETE(Lib::Draw3DTaskManager);
	SINGLETON_DELETE(Lib::Draw2DTaskManager);
	SINGLETON_DELETE(Lib::UpdateTaskManager);
}

void GameScene::Update()
{
	// デバイスの入力チェック.
	SINGLETON_INSTANCE(Lib::InputDeviceManager)->KeyUpdate();
	SINGLETON_INSTANCE(Lib::InputDeviceManager)->KeyCheck(DIK_W);
	SINGLETON_INSTANCE(Lib::InputDeviceManager)->KeyCheck(DIK_A);
	SINGLETON_INSTANCE(Lib::InputDeviceManager)->KeyCheck(DIK_S);
	SINGLETON_INSTANCE(Lib::InputDeviceManager)->KeyCheck(DIK_D);
	SINGLETON_INSTANCE(Lib::InputDeviceManager)->KeyCheck(DIK_R);
	SINGLETON_INSTANCE(Lib::InputDeviceManager)->KeyCheck(DIK_T);
	SINGLETON_INSTANCE(Lib::InputDeviceManager)->MouseUpdate();


#ifdef _DEBUG

	Lib::Debugger::StartTimer();
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->Run();
	Lib::Debugger::EndTimer();
	m_UpdateTime = static_cast<int>(Lib::Debugger::GetTime());	// 計測した更新時間を取得.


	Lib::Debugger::StartTimer();
	SINGLETON_INSTANCE(MapDrawTaskManager)->Run();
	SINGLETON_INSTANCE(DepthDrawTaskManager)->Run();
	SINGLETON_INSTANCE(CubeMapDrawTaskManager)->Run();
	SINGLETON_INSTANCE(ReflectMapDrawTaskManager)->Run();

	SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->BeginScene(Lib::Dx11::GraphicsDevice::BACKBUFFER_TARGET);
	SINGLETON_INSTANCE(Lib::Draw3DTaskManager)->Run();
	SINGLETON_INSTANCE(Lib::Draw2DTaskManager)->Run();

	// 計測時間の描画.
	char UpdateStr[32];
	char DrawStr[32];
	sprintf_s(UpdateStr, 32, "Update : %dms", m_UpdateTime);
	sprintf_s(DrawStr, 32, "Draw   : %dms", m_DrawTime);

	m_pFont->Draw(&D3DXVECTOR2(1000, 50), UpdateStr);
	m_pFont->Draw(&D3DXVECTOR2(1000, 80), DrawStr);

	SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->EndScene();
	Lib::Debugger::EndTimer();
	m_DrawTime = static_cast<int>(Lib::Debugger::GetTime());	// 計測した描画時間を取得.

#else // _DEBUG

	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->Run();

	SINGLETON_INSTANCE(MapDrawTaskManager)->Run();
	SINGLETON_INSTANCE(DepthDrawTaskManager)->Run();
	SINGLETON_INSTANCE(CubeMapDrawTaskManager)->Run();
	SINGLETON_INSTANCE(ReflectMapDrawTaskManager)->Run();

	SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->BeginScene(Lib::Dx11::GraphicsDevice::BACKBUFFER_TARGET);
	SINGLETON_INSTANCE(Lib::Draw3DTaskManager)->Run();
	SINGLETON_INSTANCE(Lib::Draw2DTaskManager)->Run();
	SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->EndScene();

#endif // !_DEBUG

}
