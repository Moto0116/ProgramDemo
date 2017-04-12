#include "TessellationObject.h"

#include "Debugger\Debugger.h"
#include "DirectX11\FbxFileManager\FbxFileManager.h"
#include "DirectX11\GraphicsDevice\GraphicsDevice.h"
#include "DirectX11\ShaderManager\ShaderManager.h"


TessellationObject::TessellationObject()
{
	m_Pos = D3DXVECTOR2(200, 200);
	m_Size = D3DXVECTOR2(200, 200);
}

TessellationObject::~TessellationObject()
{
}

bool TessellationObject::Initialize()
{
	SINGLETON_INSTANCE(Lib::DrawTaskManager)->AddTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->AddTask(m_pUpdateTask);

	SINGLETON_INSTANCE(Lib::ShaderManager)->LoadVertexShader(TEXT("Resource\\Effect\\Tessellation.fx"), "VS", &m_TessellationVertexShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->LoadPixelShader(TEXT("Resource\\Effect\\Tessellation.fx"), "PS", &m_TessellationPixelShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->LoadHullShader(TEXT("Resource\\Effect\\Tessellation.fx"), "HS", &m_TessellationHullShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->LoadDomainShader(TEXT("Resource\\Effect\\Tessellation.fx"), "DS", &m_TessellationDomainShaderIndex);

	if (!CreateVertex2D())
	{
		return false;
	}

	return true;
}

void TessellationObject::Finalize()
{
	ReleaseVertex2D();

	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleaseDomainShader(m_TessellationDomainShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleaseHullShader(m_TessellationHullShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleasePixelShader(m_TessellationPixelShaderIndex);
	SINGLETON_INSTANCE(Lib::ShaderManager)->ReleaseVertexShader(m_TessellationVertexShaderIndex);

	SINGLETON_INSTANCE(Lib::DrawTaskManager)->RemoveTask(m_pDrawTask);
	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->RemoveTask(m_pUpdateTask);
}

void TessellationObject::Update()
{
}

void TessellationObject::Draw()
{
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->VSSetShader(
		SINGLETON_INSTANCE(Lib::ShaderManager)->GetVertexShader(m_TessellationVertexShaderIndex),
		NULL,
		0);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->PSSetShader(
		SINGLETON_INSTANCE(Lib::ShaderManager)->GetPixelShader(m_TessellationPixelShaderIndex),
		NULL,
		0);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->HSSetShader(
		SINGLETON_INSTANCE(Lib::ShaderManager)->GetHullShader(m_TessellationHullShaderIndex),
		NULL,
		0);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->DSSetShader(
		SINGLETON_INSTANCE(Lib::ShaderManager)->GetDomainShader(m_TessellationDomainShaderIndex),
		NULL,
		0);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);


	m_pVertex->WriteConstantBuffer(&m_Pos);
	m_pVertex->Draw();

	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->HSSetShader(
		NULL,
		NULL,
		0);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->DSSetShader(
		NULL,
		NULL,
		0);
}

