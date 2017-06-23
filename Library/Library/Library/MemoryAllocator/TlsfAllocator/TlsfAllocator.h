/**
 * @file	TlsfAllocator.h
 * @brief	TLSFメモリアロケータクラス定義
 * @author	morimoto
 */
#ifndef TLSFALLOCATOR_H
#define TLSFALLOCATOR_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include <Windows.h>


namespace Lib
{
	/**
	 * TLSFメモリアロケータクラス
	 */
	class TlsfAllocator
	{
	public:
		/**
		 * コンストラクタ
		 */
		TlsfAllocator();

		/**
		 * デストラクタ
		 */
		~TlsfAllocator();


	private:
		void* m_MemoryPool;

	};
}


#endif // TLSFALLOCATOR_H
