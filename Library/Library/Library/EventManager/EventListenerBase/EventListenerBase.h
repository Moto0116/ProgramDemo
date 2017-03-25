/**
 * @file	EventListenerBase.h
 * @brief	イベント受信基底クラス定義
 * @author	morimoto
 */
#ifndef LIB_EVENTLISTENERBASE_H
#define LIB_EVENTLISTENERBASE_H


namespace Lib
{
	class EventBase;

	/**
	 * イベント受信基底クラス
	 */
	class EventListenerBase
	{
	public:
		/**
		 * コンストラクタ
		 */
		EventListenerBase();

		/**
		 * デストラクタ
		 */
		virtual ~EventListenerBase();

		/**
		 * イベントメッセージを受け取る関数
		 * @param[in] _pEvent 受け取るイベント
		 */
		virtual void EventMessage(EventBase* _pEvent);

		/**
		 * EventListenerのIDを取得する関数
		 * @return EventListenerのID
		 */
		inline unsigned int GetEventListenerID()
		{
			return m_EventListenerID;
		}


	private:
		static unsigned int m_EventListenerBaseCount;
		unsigned int		m_EventListenerID;

	};
}



#endif // LIB_EVENTLISTENERBASE_H
