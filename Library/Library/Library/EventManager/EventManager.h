/**
 * @file	EventManager.h
 * @brief	イベント管理基底クラス定義
 * @author	morimoto
 */
#ifndef LIB_EVENTMANAGER_H
#define LIB_EVENTMANAGER_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <vector>

#include "..\SingletonBase\SingletonBase.h"


namespace Lib
{
	class EventListener;

	/**
	 * イベント管理基底クラス
	 * @tparam Event 通知を受けるEventクラス
	 */
	template <typename Event>
	class EventManager : public SingletonBase<EventManager<Event>>
	{
	public:
		friend SingletonBase<EventManager<Event>>;

		/**
		 * EventListenerを追加する
		 * @param[in] _pEventListener 追加するEventListener
		 */
		void AddEventListener(EventListener* _pEventListener);

		/**
		 * EventListenerを削除する
		 * @param[in] _pEventListener 削除するEventListener
		 */
		void RemoveEventListener(EventListener* _pEventListener);

		/**
		 * イベントを通知する
		 * @param[in] _pEvent 通知するイベント
		 */
		void SendEventMessage(Event* _pEvent);

	private:
		/**
		 * コンストラクタ
		 */
		EventManager();

		/**
		 * デストラクタ
		 */
		virtual ~EventManager();


		std::vector<EventListener*> m_pEventListener;	//!< イベントリスナを格納するコンテナ.

	};
}


#include "EventManager_private.inl"


#endif // !LIB_EVENTMANAGER_H
