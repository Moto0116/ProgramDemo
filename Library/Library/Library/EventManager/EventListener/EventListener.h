/**
 * @file	EventListener.h
 * @brief	イベント受信クラス定義
 * @author	morimoto
 */
#ifndef LIB_EVENTLISTENER_H
#define LIB_EVENTLISTENER_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <functional>


namespace Lib
{
	class EventBase;

	/**
	 * イベント受信クラス
	 */
	class EventListener
	{
	public:
		/**
		 * コンストラクタ
		 */
		EventListener();

		/**
		 * デストラクタ
		 */
		virtual ~EventListener();

		/**
		 * コールバック関数の登録
		 */
		void RegistryFunc(std::function<void(EventBase*)>* _pFunc);

		/**
		 * イベントメッセージを受け取る関数
		 * @param[in] _pEvent 受け取るイベント
		 */
		virtual void EventMessage(EventBase* _pEvent);

		/**
		 * EventListenerのIDを取得する関数
		 * @return EventListenerのID
		 */
		inline unsigned int GetEventListenerID() const
		{
			return m_EventListenerID;
		}


	private:
		static unsigned int				m_EventListenerCount;	//!< イベントリスナオブジェクトのカウント変数.
		unsigned int					m_EventListenerID;		//!< イベントリスナのユニークID.
		std::function<void(EventBase*)>	m_Func;					//!< コールバック関数.

	};
}



#endif // !LIB_EVENTLISTENER_H
