/**
 * @file	EventBase.h
 * @brief	イベント基底クラス定義
 * @author	morimoto
 */
#ifndef LIB_EVENTBASE_H
#define LIB_EVENTBASE_H


namespace Lib
{
	/**
	 * イベント基底クラス
	 */
	class EventBase
	{
	public:
		/**
		 * コンストラクタ
		 */
		EventBase();

		/**
		 * デストラクタ
		 */
		virtual ~EventBase();

		/**
		 * EventのIDを取得する関数
		 * @return EventのID
		 */
		inline unsigned int GetEventID() const
		{
			return m_EventID;
		}


	private:
		static unsigned int m_EventBaseCount;	//!< イベントオブジェクトのカウント変数.
		unsigned int		m_EventID;			//!< イベントオブジェクトのユニークID.

	};
}


#endif // LIB_EVENTBASE_H
