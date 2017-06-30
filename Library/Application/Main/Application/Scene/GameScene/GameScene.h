/**
 * @file	GameScene.h
 * @brief	ゲームシーンクラス定義
 * @author	morimoto
 */
#ifndef GAMESCENE_H
#define GAMESCENE_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "SceneManager\SceneBase\SceneBase.h"

#include "ObjectManager\ObjectManager.h"
#include "DirectX11\Font\Font.h"


/**
 * ゲームシーンクラス
 */
class GameScene : public Lib::SceneBase
{
public:
	/**
	 * コンストラクタ
	 * @param[in] _sceneId シーンのID
	 */
	GameScene(int _sceneId);

	/**
	 * デストラクタ
	 */
	virtual ~GameScene();

	/**
	 * 初期化処理
	 * @return 初期化に成功したらtrue 失敗したらfalse
	 */
	virtual bool Initialize();

	/**
	 * 終了処理
	 */
	virtual void Finalize();

	/**
	 * シーンの更新
	 */
	virtual void Update();

private:
	static const D3DXVECTOR2	m_DefaultFontSize;	//!< フォントサイズ.
	static const D3DXCOLOR		m_DefaultFontColor;	//!< フォントカラー.

	ObjectManager*	m_pObjectManager;	//!< シーン内オブジェクト管理クラス.
	Lib::Font*		m_pFont;			//!< フォント描画オブジェクト.
	int				m_UpdateTime;		//!< 更新処理にかかった時間.
	int				m_DrawTime;			//!< 描画処理にかかった時間.


};


#endif // !GAMESCENE_H
