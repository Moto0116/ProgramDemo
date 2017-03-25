/**
 * @file	SceneBase.h
 * @brief	シーンの基底クラス定義
 * @author	morimoto
 */
#ifndef LIB_SCENEBASE_H
#define LIB_SCENEBASE_H


namespace Lib
{
	/**
	 * シーンの基底クラス
	 */
	class SceneBase
	{
	public:
		/**
		 * シーンの状態列挙子
		 */
		enum STATE
		{
			INIT_STATE,		//!< 初期化状態
			UPDATE_STATE,	//!< 更新状態
			FINAL_STATE		//!< 終了状態
		};

		/**
		 * コンストラクタ
		 */
		SceneBase();

		/**
		 * デストラクタ
		 */
		virtual ~SceneBase();

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

		/**
		 * シーンの状態を取得する
		 * @return シーンの状態
		 */
		inline STATE GetState()
		{
			return m_State;
		}

		/**
		 * 次のシーンIDを取得する
		 * @return 次のシーンID
		 */
		inline int GetNextSceneID()
		{
			return m_NextSceneId;
		}


	protected:
		STATE	m_State;		//!< シーンの状態
		int		m_NextSceneId;	//!< 次のシーンのID

	};
}


#endif // LIB_SCENEBASE_H
