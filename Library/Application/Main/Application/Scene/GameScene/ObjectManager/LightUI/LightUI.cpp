#include "LightUI.h"

#include "Debugger\Debugger.h"


LightUI::LightUI()
{
}

LightUI::~LightUI()
{
}

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

