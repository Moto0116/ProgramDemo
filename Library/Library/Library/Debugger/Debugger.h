/**
 * @file	Debugger.h
 * @brief	デバッグ機能定義
 * @author	morimoto
 */
#ifndef LIB_DEBUGGER_H
#define LIB_DEBUGGER_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <chrono>
#include <crtdbg.h>
#include <Windows.h>


namespace Lib
{
	namespace Debugger
	{
		/**
		 * メモリリークをチェックする
		 */
		void CheckMemoryLeak();

		/**
		 * デバッグログの出力関数
		 * @param[in] _pStr 出力する文字列
		 */
		void OutputDebugLog(TCHAR* _pStr, ...);

		/**
		 * ファイルを開く(ファイルがなければ作成する)
		 * @param[in] _pFilePath 開くファイルのパス
		 */
		void OpenLogFile(TCHAR* _pFilePath);

		/**
		 * デバッグ情報をファイルに書き込む
		 */
		void WriteLogFile(TCHAR* _pStr, ...);

		/**
		 * ファイルを閉じる
		 */
		void CloseLogFile();

		/**
		 * 計測開始
		 */
		void StartTimer();

		/**
		* 計測終了
		*/
		void EndTimer();

		/**
		 * 計測時間取得
		 * @return 計測時間(msec単位)
		 */
		LONGLONG GetTime();
	}
}


#ifdef _DEBUG


#define MyAssert(_expression, _str)									\
if(!_expression)													\
{																	\
	Lib::Debugger::OutputDebugLog(									\
	"\nMyAssert: %s\nErrorLog: %s\nFileName: %s\nLine: %d\n\n",		\
	#_expression,													\
	_str,															\
	__FILE__,														\
	__LINE__);														\
	DebugBreak();													\
}

#define OutputErrorLog(_str)													\
	Lib::Debugger::OutputDebugLog("\n\n---------------Error---------------");	\
	Lib::Debugger::OutputDebugLog("\nErrorLog: %s\nFileName: %s\nLine: %d\n\n",	\
	_str,																		\
	__FILE__,																	\
	__LINE__);


#else

#define MyAssert(_expression, _str)

#define OutputErrorLog(_str)


#endif // _DEBUG





#endif // LIB_DEBUGGER_H
