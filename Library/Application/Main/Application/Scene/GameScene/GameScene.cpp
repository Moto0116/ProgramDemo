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
#include "DirectX11\FbxFileManager\FbxFileManager.h"
#include "DirectX11\GraphicsDevice\GraphicsDevice.h"
#include "DirectX11\ShaderManager\ShaderManager.h"
#include "DirectX11\TextureManager\TextureManager.h"
#include "InputDeviceManager\InputDeviceManager.h"
#include "TaskManager\TaskBase\UpdateTask\UpdateTask.h"
#include "TaskManager\TaskBase\DrawTask\DrawTask.h"
#include "DepthDrawTask\DepthDrawTask.h"



GameScene::GameScene()
{
}

GameScene::~GameScene()
{
}

bool GameScene::Initialize()
{
	SINGLETON_CREATE(Lib::UpdateTaskManager);
	SINGLETON_CREATE(Lib::DrawTaskManager);
	SINGLETON_CREATE(DepthDrawTaskManager);


	SINGLETON_CREATE(Lib::FbxFileManager);
	if (!SINGLETON_INSTANCE(Lib::FbxFileManager)->Initialize(SINGLETON_INSTANCE(Lib::GraphicsDevice)))
	{
		OutputErrorLog("Fbxモデル管理クラスの生成に失敗しました");

		SINGLETON_DELETE(Lib::ShaderManager);

		return false;
	}


	SINGLETON_CREATE(Lib::ShaderManager);
	if (!SINGLETON_INSTANCE(Lib::ShaderManager)->Initialize(SINGLETON_INSTANCE(Lib::GraphicsDevice)))
	{
		OutputErrorLog("シェーダー管理クラスの生成に失敗しました");

		SINGLETON_DELETE(Lib::ShaderManager);

		SINGLETON_INSTANCE(Lib::FbxFileManager)->Finalize();
		SINGLETON_DELETE(Lib::ShaderManager);

		return false;
	}


	SINGLETON_CREATE(Lib::TextureManager);
	if (!SINGLETON_INSTANCE(Lib::TextureManager)->Initialize(SINGLETON_INSTANCE(Lib::GraphicsDevice)))
	{
		OutputErrorLog("テクスチャ管理クラスの生成に失敗しました");

		SINGLETON_DELETE(Lib::TextureManager);

		SINGLETON_INSTANCE(Lib::ShaderManager)->Finalize();
		SINGLETON_DELETE(Lib::ShaderManager);

		SINGLETON_INSTANCE(Lib::FbxFileManager)->Finalize();
		SINGLETON_DELETE(Lib::ShaderManager);

		return false;
	}


	m_pObjectManager = new ObjectManager();
	if (!m_pObjectManager->Initialize())
	{
		OutputErrorLog("グラウンドの生成に失敗しました");

		delete m_pObjectManager;

		SINGLETON_INSTANCE(Lib::TextureManager)->Finalize();
		SINGLETON_DELETE(Lib::TextureManager);

		SINGLETON_INSTANCE(Lib::ShaderManager)->Finalize();
		SINGLETON_DELETE(Lib::ShaderManager);

		SINGLETON_INSTANCE(Lib::FbxFileManager)->Finalize();
		SINGLETON_DELETE(Lib::ShaderManager);

		return false;
	}


	m_State = UPDATE_STATE;

	return true;
}

void GameScene::Finalize()
{
	m_pObjectManager->Finalize();
	delete m_pObjectManager;

	SINGLETON_INSTANCE(Lib::TextureManager)->Finalize();
	SINGLETON_DELETE(Lib::TextureManager);

	SINGLETON_INSTANCE(Lib::ShaderManager)->Finalize();
	SINGLETON_DELETE(Lib::ShaderManager);

	SINGLETON_INSTANCE(Lib::FbxFileManager)->Finalize();
	SINGLETON_DELETE(Lib::FbxFileManager);

	SINGLETON_DELETE(DepthDrawTaskManager);
	SINGLETON_DELETE(Lib::DrawTaskManager);
	SINGLETON_DELETE(Lib::UpdateTaskManager);
}

void GameScene::Update()
{
	SINGLETON_INSTANCE(Lib::InputDeviceManager)->KeyUpdate();
	SINGLETON_INSTANCE(Lib::InputDeviceManager)->KeyCheck(DIK_W);
	SINGLETON_INSTANCE(Lib::InputDeviceManager)->KeyCheck(DIK_A);
	SINGLETON_INSTANCE(Lib::InputDeviceManager)->KeyCheck(DIK_S);
	SINGLETON_INSTANCE(Lib::InputDeviceManager)->KeyCheck(DIK_D);

	SINGLETON_INSTANCE(Lib::InputDeviceManager)->MouseUpdate();

	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->Run();
	SINGLETON_INSTANCE(DepthDrawTaskManager)->Run();

	SINGLETON_INSTANCE(Lib::GraphicsDevice)->BeginScene();
	SINGLETON_INSTANCE(Lib::DrawTaskManager)->Run();
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->EndScene();
}
