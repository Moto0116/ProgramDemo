/**
 * @file	WaterDebugFont.h
 * @brief	水のデバッグフォントクラス定義
 * @author	morimoto
 */
#ifndef WATERDEBUGFONT_H
#define WATERDEBUGFONT_H

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "Main\Object2DBase\Object2DBase.h"
#include "DirectX11\Font\Dx11Font.h"


/**
 * 水のデバッグフォントクラス
 */
class WaterDebugFont : public Object2DBase
{
public:
	/**
	 * コンストラクタ
	 */
	WaterDebugFont();

	/**
	 * デストラクタ
	 */
	virtual ~WaterDebugFont();

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
	 * オブジェクトの描画
	 */
	virtual void Draw();

	/**
	 * キューブマップを使用しているかのフラグを設定
	 * @param[in] _isCubeMap キューブマップを使用しているか
	 */
	void SetIsCubeMap(bool _isCubeMap)
	{
		m_IsCubeMap = _isCubeMap;
	}

private:
	Lib::Dx11::Font*	m_pFont;	//!< フォント描画オブジェクト.
	bool				m_IsCubeMap;//!< キューブマップを使用しているかのフラグ.

};


#endif // !WATERDEBUGFONT_H
