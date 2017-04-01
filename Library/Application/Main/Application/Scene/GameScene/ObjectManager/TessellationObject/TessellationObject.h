#ifndef TESSELLATIONOBJECT_H
#define TESSELLATIONOBJECT_H

#include "Main\Object2DBase\Object2DBase.h"


class TessellationObject : public Object2DBase
{
public:
	/**
	 * コンストラクタ
	 */
	TessellationObject();

	/**
	 * デストラクタ
	 */
	virtual ~TessellationObject();

	/**
	 * 初期化処理
	 * @return 初期化に成功したか
	 */
	virtual bool Initialize();

	/**
	 * 終了処理
	 */
	virtual void Finalize();

	/**
	 * オブジェクトの更新
	 */
	virtual void Update();

	/**
	 * オブジェクトの描画
	 */
	virtual void Draw();

private:
	int	m_TessellationVertexShaderIndex;
	int	m_TessellationPixelShaderIndex;
	int m_TessellationHullShaderIndex;
	int m_TessellationDomainShaderIndex;


};


#endif
