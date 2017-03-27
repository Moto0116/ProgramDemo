/**
 * @file	Debugger.cpp
 * @brief	デバッグ機能実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "Debugger.h"

#include <stdio.h>
#include <stdarg.h>


namespace Lib
{
	namespace Debugger
	{
		//----------------------------------------------------------------------
		// Static Variables
		//----------------------------------------------------------------------
		static const int g_DebugLogMax = 512;									//!< デバッグログの最大文字数.
		static FILE*									g_pDebugLogFile = NULL;	//!< デバッグ用ファイルポインタ.
		static std::chrono::system_clock::time_point	g_StartTime;			//!< 計測開始時間.
		static std::chrono::system_clock::time_point	g_EndTime;				//!< 計測終了時間.
		static LONGLONG									g_DiffTime;				//!< 計測時間(msec単位).


		//----------------------------------------------------------------------
		// Functions
		//----------------------------------------------------------------------
		void CheckMemoryLeak()
		{
			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		}

		void OutputDebugLog(TCHAR* _pStr, ...)
		{
			TCHAR OutputStr[g_DebugLogMax];
			va_list ArgsList;

			va_start(ArgsList, _pStr);

			vsprintf_s(OutputStr, sizeof(OutputStr), _pStr, ArgsList);
			OutputDebugString(OutputStr);

			va_end(ArgsList);
		}

		void OpenLogFile(TCHAR* _pFilePath)
		{
			fopen_s(&g_pDebugLogFile, _pFilePath, "w");
		}

		void WriteLogFile(TCHAR* _pStr, ...)
		{
			TCHAR OutputStr[g_DebugLogMax];
			va_list ArgsList;

			va_start(ArgsList, _pStr);

			vsprintf_s(OutputStr, sizeof(OutputStr), _pStr, ArgsList);
			fprintf(g_pDebugLogFile, OutputStr);

			va_end(ArgsList);
		}

		void CloseLogFile()
		{
			fclose(g_pDebugLogFile);
		}

		void StartTimer()
		{
			g_StartTime = std::chrono::system_clock::now();
		}

		void EndTimer()
		{
			g_EndTime = std::chrono::system_clock::now();
			auto Diff = g_EndTime - g_StartTime;
			g_DiffTime = std::chrono::duration_cast<std::chrono::milliseconds>(Diff).count();
		}

		LONGLONG GetTime()
		{
			return g_DiffTime;
		}
	}
}

