#ifndef APPLICATION_H
#define APPLICATION_H

#include "Window\Window.h"
#include "SceneManager\SceneManager.h"

class GameScene;
class TitleScene;


class Application
{
public:
	/**
	 * コンストラクタ
	 */
	Application();

	/**
	 * デストラクタ
	 */
	~Application();

	/**
	 * 初期化処理
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	bool Initialize();

	/**
	 * 終了処理
	 */
	void Finalize();

	/**
	 * アプリケーションを起動
	 */
	void Run();


private:
	/**
	 * シーンのID列挙子
	 */
	enum SCENE_ID
	{
		TITLE_SCENE_ID,	//!< タイトルシーンID
		GAME_SCENE_ID	//!< ゲームシーンID
	};

	static const int m_WindowWidth;
	static const int m_WindowHeight;
	static const DWORD m_WindowStyle;

	bool CreateMainWindow();
	bool CreateDX11Manager();
	bool CreateInputDevice();
	bool CreateSceneManager();
	void ReleaseMainWindow();
	void ReleaseDX11Manager();
	void ReleaseInputDevice();
	void ReleaseSceneManager();

	Lib::Window*		m_pMainWindow;
	Lib::SceneManager*	m_pSceneManager;
	GameScene*			m_pGameScene;

};


#endif
