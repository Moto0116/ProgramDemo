/**
 * @file   MainCamera.cpp
 * @brief  MainCameraクラスの実装
 * @author kotani
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "MainCamera.h"

#include "Debugger\Debugger.h"
#include "DirectX11\GraphicsDevice\GraphicsDevice.h"
#include "InputDeviceManager\InputDeviceManager.h"


const float MainCamera::m_NearPoint = 1.f;
const float MainCamera::m_FarPoint = 700;
const float MainCamera::m_ViewAngle = 50.f;
const float MainCamera::m_MaxAngle = 70.f;
const float MainCamera::m_MinAngle = 10.f;
const float MainCamera::m_MaxLength = 200.f;
const float MainCamera::m_MinLength = 80.f;
const float MainCamera::m_MoveSpeedWeight = 0.015f;
const float MainCamera::m_ZoomSpeedWeight = 0.1f;
const float MainCamera::m_RotateSpeedWeight = 0.22f;


MainCamera::MainCamera() :
	m_LookPoint(D3DXVECTOR3(0.f, 0.f, 0.f)),
	m_MoveSpeed(0.f),
	m_ZoomSpeed(0.f),
	m_CameraAngle(0.f, 50.f),
	m_CameraLength(100.f),
	m_isCameraControl(false)
{
}

MainCamera::~MainCamera()
{
}

bool MainCamera::Initialize()
{
	m_pUpdateTask = new Lib::UpdateTask();
	m_pUpdateTask->SetUpdateObject(this);

	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->AddTask(m_pUpdateTask);

	const RECT* pWindowRect = SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetMainWindowRect();
	m_pCamera = new Lib::Camera(
		static_cast<float>(pWindowRect->right - pWindowRect->left),
		static_cast<float>(pWindowRect->bottom - pWindowRect->top),
		m_NearPoint,
		m_FarPoint);

	if (!CreateConstantBuffer())
	{
		OutputErrorLog("定数バッファ生成に失敗しました");
		return false;
	}

	RotateCalculate();	// 初期値で座標を計算する
	Transform();

	return true;
}

void MainCamera::Finalize()
{
	ReleaseConstantBuffer();
	delete m_pCamera;

	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->RemoveTask(m_pUpdateTask);
	delete m_pUpdateTask;
}

void MainCamera::Update()
{
	m_isCameraControl = false;
	m_MouseState = SINGLETON_INSTANCE(Lib::InputDeviceManager)->GetMouseState();
	m_pKeyState = SINGLETON_INSTANCE(Lib::InputDeviceManager)->GetKeyState();

	m_MoveSpeed = m_Pos.y * m_MoveSpeedWeight;
	m_ZoomSpeed = m_Pos.y * m_ZoomSpeedWeight;

	Move();
	Rotate();
	Zoom();
	
	if (m_isCameraControl)
	{
		Transform();	// カメラが動いたら定数バッファを更新する
	}
}

void MainCamera::Draw()
{
}

void MainCamera::Transform()
{
	WriteConstantBuffer();

	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->VSSetConstantBuffers(1, 1, &m_pConstantBuffer);
	SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->PSSetConstantBuffers(1, 1, &m_pConstantBuffer);
}

void MainCamera::Move()
{
	if (m_pKeyState[DIK_W] == Lib::KeyDevice::KEYSTATE::KEY_ON)
	{
		MoveFront();
		m_isCameraControl = true;
	}
	if (m_pKeyState[DIK_A] == Lib::KeyDevice::KEYSTATE::KEY_ON)
	{
		MoveLeft();
		m_isCameraControl = true;
	}
	if (m_pKeyState[DIK_S] == Lib::KeyDevice::KEYSTATE::KEY_ON)
	{
		MoveBack();
		m_isCameraControl = true;
	}
	if (m_pKeyState[DIK_D] == Lib::KeyDevice::KEYSTATE::KEY_ON)
	{
		MoveRight();
		m_isCameraControl = true;
	}
}

void MainCamera::Rotate()
{
	if (m_MouseState.Buttons[1] == Lib::MouseDevice::MOUSEBUTTONSTATE::MOUSEBUTTON_ON)
	{
		m_CameraAngle.x += static_cast<float>(m_MouseState.lX * m_RotateSpeedWeight);
		m_CameraAngle.y += static_cast<float>(m_MouseState.lY * m_RotateSpeedWeight);

		// 360を超えたら0に戻す(差分は気にならないレベルなので無視)
		if (fabs(m_CameraAngle.x) >= 360)
		{
			m_CameraAngle.x = 0;
		}
		
		// カメラの角度制限
		if (m_CameraAngle.y >= m_MaxAngle)
		{
			m_CameraAngle.y = m_MaxAngle;
		}
		else if (m_CameraAngle.y <= m_MinAngle)
		{
			m_CameraAngle.y = m_MinAngle;
		}

		RotateCalculate();
		m_isCameraControl = true;
	}
}

void MainCamera::RotateCalculate()
{
	m_Pos.x = m_LookPoint.x + -m_CameraLength * sin(static_cast<float>(D3DXToRadian(m_CameraAngle.x)));
	m_Pos.z = m_LookPoint.z + -m_CameraLength * cos(static_cast<float>(D3DXToRadian(m_CameraAngle.x)));
	m_Pos.y = m_LookPoint.y + -m_CameraLength * sin(static_cast<float>(D3DXToRadian(-m_CameraAngle.y)));
}

void MainCamera::Zoom()
{
	if (m_MouseState.lZ > 0)
	{
		m_CameraLength -= m_ZoomSpeed;
		if (m_CameraLength < m_MinLength)
		{
			m_CameraLength = m_MinLength;
		}
		RotateCalculate();
		m_isCameraControl = true;
	}
	else if (m_MouseState.lZ < 0)
	{
		m_CameraLength += m_ZoomSpeed;
		if (m_CameraLength > m_MaxLength)
		{
			m_CameraLength = m_MaxLength;
		}
		RotateCalculate();
		m_isCameraControl = true;
	}
}

void MainCamera::MoveFront()
{
	float moveX = m_MoveSpeed * sin(static_cast<float>(D3DXToRadian(m_CameraAngle.x)));
	float moveZ = m_MoveSpeed * cos(static_cast<float>(D3DXToRadian(m_CameraAngle.x)));

	m_Pos.x += moveX;
	m_Pos.z += moveZ;
	m_LookPoint.x += moveX;
	m_LookPoint.z += moveZ;
}

void MainCamera::MoveLeft()
{
	float moveX = m_MoveSpeed * sin(static_cast<float>(D3DXToRadian(m_CameraAngle.x - 90)));
	float moveZ = m_MoveSpeed * cos(static_cast<float>(D3DXToRadian(m_CameraAngle.x - 90)));

	m_Pos.x += moveX;
	m_Pos.z += moveZ;
	m_LookPoint.x += moveX;
	m_LookPoint.z += moveZ;
}

void MainCamera::MoveBack()
{
	float moveX = m_MoveSpeed * sin(static_cast<float>(D3DXToRadian(m_CameraAngle.x + 180)));
	float moveZ = m_MoveSpeed * cos(static_cast<float>(D3DXToRadian(m_CameraAngle.x + 180)));

	m_Pos.x += moveX;
	m_Pos.z += moveZ;
	m_LookPoint.x += moveX;
	m_LookPoint.z += moveZ;
}

void MainCamera::MoveRight()
{
	float moveX = m_MoveSpeed * sin(static_cast<float>(D3DXToRadian(m_CameraAngle.x + 90)));
	float moveZ = m_MoveSpeed * cos(static_cast<float>(D3DXToRadian(m_CameraAngle.x + 90)));

	m_Pos.x += moveX;
	m_Pos.z += moveZ;
	m_LookPoint.x += moveX;
	m_LookPoint.z += moveZ;
}

bool MainCamera::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC ConstantBufferDesc;
	ConstantBufferDesc.ByteWidth = sizeof(CAMERA_CONSTANT_BUFFER);
	ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ConstantBufferDesc.MiscFlags = 0;
	ConstantBufferDesc.StructureByteStride = 0;

	if (FAILED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDevice()->CreateBuffer(&ConstantBufferDesc, NULL, &m_pConstantBuffer)))
	{
		OutputErrorLog("定数バッファ生成に失敗しました");
		return false;
	}

	return true;
}

void MainCamera::ReleaseConstantBuffer()
{
	if (m_pConstantBuffer != NULL)
	{
		m_pConstantBuffer->Release();
		m_pConstantBuffer = NULL;
	}
}

void MainCamera::WriteConstantBuffer()
{
	m_pCamera->TransformView(&m_Pos, &m_LookPoint, m_ViewAngle);

	D3D11_MAPPED_SUBRESOURCE SubResourceData;
	if (SUCCEEDED(SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResourceData)))
	{
		CAMERA_CONSTANT_BUFFER ConstantBuffer;
		ConstantBuffer.Proj = m_pCamera->GetProjectionMatrix();
		ConstantBuffer.View = m_pCamera->GetViewMatrix();

		D3DXMatrixTranspose(&ConstantBuffer.View, &ConstantBuffer.View);
		D3DXMatrixTranspose(&ConstantBuffer.Proj, &ConstantBuffer.Proj);

		memcpy_s(
			SubResourceData.pData, 
			SubResourceData.RowPitch, 
			reinterpret_cast<void*>(&ConstantBuffer), 
			sizeof(ConstantBuffer));

		SINGLETON_INSTANCE(Lib::GraphicsDevice)->GetDeviceContext()->Unmap(m_pConstantBuffer, 0);
	}
}
