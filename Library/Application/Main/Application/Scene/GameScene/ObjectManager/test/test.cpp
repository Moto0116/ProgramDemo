/**
 * @file	test.cpp
 * @brief	ステージ背景クラス実装
 * @author	morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "test.h"

#include "DirectX11\TextureManager\Dx11TextureManager.h"


//----------------------------------------------------------------------
// Constructor	Destructor
//----------------------------------------------------------------------
test::test()
{
	m_Pos = D3DXVECTOR2(640, 360);
	m_Size = D3DXVECTOR2(1280, 720);
}

test::~test()
{
}


//----------------------------------------------------------------------
// Public Functions
//----------------------------------------------------------------------
bool test::Initialize()
{
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->AddTask(m_pUpdateTask);
	SINGLETON_INSTANCE(Lib::Draw2DTaskManager)->AddTask(m_pDrawTask);
	m_pDrawTask->SetPriority(5);

	if (!CreateVertex2D()) return false;

	if (!SINGLETON_INSTANCE(Lib::Dx11::TextureManager)->LoadTexture(
		"test.png",
		&m_TextureIndex))
	{
		return false;
	}

	return true;
}

void test::Finalize()
{
	SINGLETON_INSTANCE(Lib::Dx11::TextureManager)->ReleaseTexture(m_TextureIndex);

	ReleaseVertex2D();

	SINGLETON_INSTANCE(Lib::Draw2DTaskManager)->RemoveTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->RemoveTask(m_pUpdateTask);
}

void test::Update()
{
}

void test::Draw()
{
	m_pVertex->WriteConstantBuffer(&m_Pos);
	m_pVertex->SetTexture(
		SINGLETON_INSTANCE(Lib::Dx11::TextureManager)->GetTexture(m_TextureIndex));
	m_pVertex->DefaultDraw();
}
