/**
 * @file	LightUI.cpp
 * @brief	ライトUIクラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "LightUI.h"

#include "Debugger\Debugger.h"


//----------------------------------------------------------------------
// Constructor	Destructor
//----------------------------------------------------------------------
LightUI::LightUI()
{
}

LightUI::~LightUI()
{
}


//----------------------------------------------------------------------
// Public Functions
//----------------------------------------------------------------------
bool LightUI::Initialize()
{
	if (!CreateVertex2D())
	{
		OutputErrorLog("2D描画オブジェクトの生成に失敗しました");
		return false;
	}

	return true;
}

void LightUI::Finalize()
{
	ReleaseVertex2D();
}

void LightUI::Update()
{
}

void LightUI::Draw()
{
	Object2DBase::Draw();
}

