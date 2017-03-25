/**
 * @file	SceneManager.h
 * @brief	シーン管理クラス定義
 * @author	morimoto
 */
#ifndef LIB_SCENEMANAGER_H
#define LIB_SCENEMANAGER_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <vector>


namespace Lib
{
	class SceneBase;

	/**
	 * シーン管理クラス
	 */
	class SceneManager
	{
	public:
		/**
		 * コンストラクタ
		 */
		SceneManager();

		/**
		 * デストラクタ
		 */
		~SceneManager();

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
		 * シーンを更新する
		 */
		void Update();

		/**
		 * シーンの追加
		 * @param[in] _pScene 登録するシーンオブジェクト
		 * @param[in] _sceneId シーンオブジェクトのID(IDはユニークで0以上の整数である必要がある)
		 * @return シーンの追加に成功したらtrue 失敗したらfalse
		 */
		bool AddScene(SceneBase* _pScene, int _sceneId);

		/**
		 * シーンの削除
		 */
		void DeleteScene(int _sceneId);

		/**
		 * エントリシーンを設定する
		 * @param[in] _sceneId エントリシーンのID
		 */
		void SetEntryScene(int _sceneId);

	private:
		/**
		 * シーンのデータ構造体
		 */
		struct SCENE_DATA
		{
			SCENE_DATA(SceneBase* _pScene, int _sceneId) :
				pScene(_pScene),
				SceneID(_sceneId)
			{
			}

			SceneBase* pScene;
			int SceneID;
		};


		static const int m_EmptySceneID;	//!< 空シーンのID

		std::vector<SCENE_DATA*>	m_pSceneData;
		SceneBase*					m_pCurrentScene;	//!< 現在のシーン

	};
}


#endif // LIB_SCENEMANAGER_H
