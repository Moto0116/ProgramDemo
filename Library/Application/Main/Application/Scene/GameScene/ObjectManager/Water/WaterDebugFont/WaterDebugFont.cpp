/**
 * @file	WaterDebugFont.cpp
 * @brief	水のデバッグフォントクラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "WaterDebugFont.h"

#include "Debugger\Debugger.h"


//----------------------------------------------------------------------
// Constructor	Destructor
//----------------------------------------------------------------------
WaterDebugFont::WaterDebugFont() : 
	m_IsCubeMap(true)
{
}

WaterDebugFont::~WaterDebugFont()
{
}


//----------------------------------------------------------------------
// Public Functions
//----------------------------------------------------------------------
bool WaterDebugFont::Initialize()
{
	SINGLETON_INSTANCE(Lib::Draw2DTaskManager)->AddTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->AddTask(m_pUpdateTask);

	m_pFont = new Lib::Dx11::Font();
	if (!m_pFont->Initialize(SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)))
	{
		return false;
	}

	if (!m_pFont->CreateVertexBuffer(&D3DXVECTOR2(16, 32), &D3DXCOLOR(0xffffffff)))
	{
		return false;
	}

	return true;
}

void WaterDebugFont::Finalize()
{
	m_pFont->ReleaseVertexBuffer();
	m_pFont->Finalize();
	delete m_pFont;

	SINGLETON_INSTANCE(Lib::Draw2DTaskManager)->RemoveTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->RemoveTask(m_pUpdateTask);
}


void WaterDebugFont::Draw()
{
	if (m_IsCubeMap)
	{
		m_pFont->Draw(&D3DXVECTOR2(25, 50), "Water : CubeMap");
		m_pFont->Draw(&D3DXVECTOR2(D3DXVECTOR2(25, 50).x + 320, D3DXVECTOR2(25, 50).y), "T key");
	}
	else
	{
		m_pFont->Draw(&D3DXVECTOR2(25, 50), "Water : ReflectMap");
		m_pFont->Draw(&D3DXVECTOR2(D3DXVECTOR2(25, 50).x + 320, D3DXVECTOR2(25, 50).y), "T key");
	}
}
