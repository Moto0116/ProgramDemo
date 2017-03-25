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
	/**
	 * イベント管理基底クラス
	 */
	template <typename Listener, typename Event>
	class EventManager : public SingletonBase<EventManager<Listener, Event>>
	{
	public:
		friend SingletonBase<EventManager<Listener, Event>>;

		/**
		 * EventListenerを追加する
		 * @param[in] _pEventListener 追加するEventListener
		 */
		void AddEventListener(Listener* _pEventListener);

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

		std::vector<Listener*> m_pEventListener;

	};
}


#include "EventManager_private.h"


#endif // LIB_EVENTMANAGER_H
