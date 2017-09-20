/**
 * @file   MainCamera.cpp
 * @brief  MainCameraクラスの実装
 * @author morimoto
 */

//----------------------------------------------------------------------
// Include
//----------------------------------------------------------------------
#include "MainCamera.h"

#include "Debugger\Debugger.h"
#include "TaskManager\TaskBase\UpdateTask\UpdateTask.h"
#include "DirectX11\GraphicsDevice\Dx11GraphicsDevice.h"
#include "InputDeviceManager\InputDeviceManager.h"


//----------------------------------------------------------------------
// Static Private Variables
//----------------------------------------------------------------------
const float MainCamera::m_NearPoint = 1.f;
const float MainCamera::m_FarPoint = 700;
const float MainCamera::m_ViewAngle = 50.f;
const float MainCamera::m_ReflectViewAngle = 80.f;
const float MainCamera::m_MaxAngle = 70.f;
const float MainCamera::m_MinAngle = -20.f;
const float MainCamera::m_MaxLength = 170.f;
const float MainCamera::m_MinLength = 30.f;
const float MainCamera::m_MoveSpeedWeight = 0.025f;
const float MainCamera::m_ZoomSpeedWeight = 0.1f;
const float MainCamera::m_RotateSpeedWeight = 0.22f;


//----------------------------------------------------------------------
// Constructor	Destructor
//----------------------------------------------------------------------
MainCamera::MainCamera() :
	m_Pos(D3DXVECTOR3(0, 80, -70)),
	m_LookPoint(D3DXVECTOR3(0.f, 0.f, 0.f)),
	m_UpVec(D3DXVECTOR3(0.f, 1.f, 0.f)),
	m_CameraAngle(0.f, 50.f),
	m_MoveSpeed(0.f),
	m_ZoomSpeed(0.f),
	m_CameraLength(70.f),
	m_IsCameraControl(false),
	m_pConstantBuffer(nullptr)
{
}

MainCamera::~MainCamera()
{
}


//----------------------------------------------------------------------
// Public Functions
//----------------------------------------------------------------------
bool MainCamera::Initialize()
{
	m_pUpdateTask = new Lib::UpdateTask();
	m_pUpdateTask->SetObject(this);

	SINGLETON_INSTANCE(Lib::UpdateTaskManager)->AddTask(m_pUpdateTask);

	const RECT* pWindowRect = SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetMainWindowRect();
	m_pCamera = new Lib::Dx11::Camera(
		static_cast<float>(pWindowRect->right - pWindowRect->left),
		static_cast<float>(pWindowRect->bottom - pWindowRect->top),
		m_NearPoint,
		m_FarPoint);

	if (!CreateConstantBuffer()) return false;

	RotateCalculate();	// 初期値で座標を計算する.
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
	m_IsCameraControl = false;
	m_MouseState = SINGLETON_INSTANCE(Lib::InputDeviceManager)->GetMouseState();
	m_pKeyState = SINGLETON_INSTANCE(Lib::InputDeviceManager)->GetKeyState();

	m_MoveSpeed = m_Pos.y * m_MoveSpeedWeight;
	m_ZoomSpeed = m_Pos.y * m_ZoomSpeedWeight;

	Move();
	Rotate();
	Zoom();
	
	if (m_IsCameraControl)
	{
		Transform();	// カメラが動いたら定数バッファを更新する.
	}
}

void MainCamera::GetBillBoardRotation(D3DXVECTOR3* _pBillPos, D3DXMATRIX* _pRotation)
{
	D3DXMatrixIdentity(_pRotation);
	D3DXMatrixLookAtLH(_pRotation, &m_Pos, _pBillPos, &D3DXVECTOR3(0, 1, 0));
	D3DXMatrixInverse(_pRotation, nullptr, _pRotation);

	// 座標移動部分は消しておく.
	_pRotation->_41 = 0.0f;
	_pRotation->_42 = 0.0f;
	_pRotation->_43 = 0.0f;
}


//----------------------------------------------------------------------
// Private Functions
//----------------------------------------------------------------------
void MainCamera::Move()
{
	if (m_pKeyState[DIK_W] == Lib::KeyDevice::KEYSTATE::KEY_ON)
	{
		MoveFront();
		m_IsCameraControl = true;
	}
	if (m_pKeyState[DIK_A] == Lib::KeyDevice::KEYSTATE::KEY_ON)
	{
		MoveLeft();
		m_IsCameraControl = true;
	}
	if (m_pKeyState[DIK_S] == Lib::KeyDevice::KEYSTATE::KEY_ON)
	{
		MoveBack();
		m_IsCameraControl = true;
	}
	if (m_pKeyState[DIK_D] == Lib::KeyDevice::KEYSTATE::KEY_ON)
	{
		MoveRight();
		m_IsCameraControl = true;
	}
}

void MainCamera::Rotate()
{
	if (m_MouseState.Buttons[1] == Lib::MouseDevice::MOUSEBUTTONSTATE::MOUSEBUTTON_ON)
	{
		m_CameraAngle.x += static_cast<float>(m_MouseState.lX * m_RotateSpeedWeight);
		m_CameraAngle.y += static_cast<float>(m_MouseState.lY * m_RotateSpeedWeight);

		// 360を超えたら0に戻す(差分は気にならないレベルなので無視).
		if (fabs(m_CameraAngle.x) >= 360)
		{
			m_CameraAngle.x = 0;
		}
		
		// カメラの角度制限.
		if (m_CameraAngle.y >= m_MaxAngle)
		{
			m_CameraAngle.y = m_MaxAngle;
		}
		else if (m_CameraAngle.y <= m_MinAngle)
		{
			m_CameraAngle.y = m_MinAngle;
		}

		RotateCalculate();
		m_IsCameraControl = true;
	}
}

void MainCamera::RotateCalculate()
{
	m_LookPoint.x = m_Pos.x + m_CameraLength * sin(static_cast<float>(D3DXToRadian(m_CameraAngle.x)));
	m_LookPoint.z = m_Pos.z + m_CameraLength * cos(static_cast<float>(D3DXToRadian(m_CameraAngle.x)));
	m_LookPoint.y = m_Pos.y + m_CameraLength * sin(static_cast<float>(D3DXToRadian(-m_CameraAngle.y)));
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

		m_Pos.x = m_LookPoint.x + -m_CameraLength * sin(static_cast<float>(D3DXToRadian(m_CameraAngle.x)));
		m_Pos.z = m_LookPoint.z + -m_CameraLength * cos(static_cast<float>(D3DXToRadian(m_CameraAngle.x)));
		m_Pos.y = m_LookPoint.y + -m_CameraLength * sin(static_cast<float>(D3DXToRadian(-m_CameraAngle.y)));

		m_IsCameraControl = true;
	}
	else if (m_MouseState.lZ < 0)
	{
		m_CameraLength += m_ZoomSpeed;
		if (m_CameraLength > m_MaxLength)
		{
			m_CameraLength = m_MaxLength;
		}

		m_Pos.x = m_LookPoint.x + -m_CameraLength * sin(static_cast<float>(D3DXToRadian(m_CameraAngle.x)));
		m_Pos.z = m_LookPoint.z + -m_CameraLength * cos(static_cast<float>(D3DXToRadian(m_CameraAngle.x)));
		m_Pos.y = m_LookPoint.y + -m_CameraLength * sin(static_cast<float>(D3DXToRadian(-m_CameraAngle.y)));

		m_IsCameraControl = true;
	}
}

void MainCamera::MoveFront()
{
	float MoveX = m_MoveSpeed * sin(static_cast<float>(D3DXToRadian(m_CameraAngle.x)));
	float MoveZ = m_MoveSpeed * cos(static_cast<float>(D3DXToRadian(m_CameraAngle.x)));

	m_Pos.x += MoveX;
	m_Pos.z += MoveZ;
	m_LookPoint.x += MoveX;
	m_LookPoint.z += MoveZ;
}

void MainCamera::MoveLeft()
{
	float MoveX = m_MoveSpeed * sin(static_cast<float>(D3DXToRadian(m_CameraAngle.x - 90)));
	float MoveZ = m_MoveSpeed * cos(static_cast<float>(D3DXToRadian(m_CameraAngle.x - 90)));

	m_Pos.x += MoveX;
	m_Pos.z += MoveZ;
	m_LookPoint.x += MoveX;
	m_LookPoint.z += MoveZ;
}

void MainCamera::MoveBack()
{
	float MoveX = m_MoveSpeed * sin(static_cast<float>(D3DXToRadian(m_CameraAngle.x + 180)));
	float MoveZ = m_MoveSpeed * cos(static_cast<float>(D3DXToRadian(m_CameraAngle.x + 180)));

	m_Pos.x += MoveX;
	m_Pos.z += MoveZ;
	m_LookPoint.x += MoveX;
	m_LookPoint.z += MoveZ;
}

void MainCamera::MoveRight()
{
	float MoveX = m_MoveSpeed * sin(static_cast<float>(D3DXToRadian(m_CameraAngle.x + 90)));
	float MoveZ = m_MoveSpeed * cos(static_cast<float>(D3DXToRadian(m_CameraAngle.x + 90)));

	m_Pos.x += MoveX;
	m_Pos.z += MoveZ;
	m_LookPoint.x += MoveX;
	m_LookPoint.z += MoveZ;
}

void MainCamera::Transform()
{
	ID3D11DeviceContext* pDeviceContext = SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext();

	WriteConstantBuffer();

	pDeviceContext->VSSetConstantBuffers(1, 1, &m_pConstantBuffer);
	pDeviceContext->GSSetConstantBuffers(1, 1, &m_pConstantBuffer);
	pDeviceContext->HSSetConstantBuffers(1, 1, &m_pConstantBuffer);
	pDeviceContext->DSSetConstantBuffers(1, 1, &m_pConstantBuffer);
	pDeviceContext->PSSetConstantBuffers(1, 1, &m_pConstantBuffer);
}

bool MainCamera::CreateConstantBuffer()
{
	Lib::Dx11::GraphicsDevice* pGraphicsDevice = SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice);

	// カメラの定数バッファ生成.
	D3D11_BUFFER_DESC ConstantBufferDesc;
	ConstantBufferDesc.ByteWidth = sizeof(CAMERA_CONSTANT_BUFFER);
	ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ConstantBufferDesc.MiscFlags = 0;
	ConstantBufferDesc.StructureByteStride = 0;

	if (FAILED(pGraphicsDevice->GetDevice()->CreateBuffer(
		&ConstantBufferDesc, 
		nullptr,
		&m_pConstantBuffer)))
	{
		OutputErrorLog("定数バッファ生成に失敗しました");
		return false;
	}

	return true;
}

void MainCamera::ReleaseConstantBuffer()
{
	SafeRelease(m_pConstantBuffer);
}

bool MainCamera::WriteConstantBuffer()
{
	D3D11_MAPPED_SUBRESOURCE SubResourceData;
	if (SUCCEEDED(SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext()->Map(
		m_pConstantBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0, 
		&SubResourceData)))
	{
		m_pCamera->TransformView(&m_Pos, &m_LookPoint, &m_UpVec, m_ViewAngle);

		CAMERA_CONSTANT_BUFFER ConstantBuffer;
		ConstantBuffer.Proj = m_pCamera->GetProjectionMatrix();
		ConstantBuffer.View = m_pCamera->GetViewMatrix();
		ConstantBuffer.CameraPos = D3DXVECTOR4(m_Pos.x, m_Pos.y, m_Pos.z, 1.0f);

		D3DXVECTOR3 CameraDir;
		D3DXVec3Normalize(&CameraDir, &D3DXVECTOR3(m_LookPoint - m_Pos));
		ConstantBuffer.CameraDir = D3DXVECTOR4(CameraDir.x, CameraDir.y, CameraDir.z, 1.0f);

		ConstantBuffer.Aspect = D3DXVECTOR4(1600, 900, 0, 0);

		m_pCamera->TransformView(
			&D3DXVECTOR3(m_Pos.x , -m_Pos.y, m_Pos.z),
			&D3DXVECTOR3(m_LookPoint.x, -m_LookPoint.y, m_LookPoint.z),
			&D3DXVECTOR3(m_UpVec.x, m_UpVec.y, m_UpVec.z),
			m_ReflectViewAngle);
		ConstantBuffer.ReflectView = m_pCamera->GetViewMatrix();
		ConstantBuffer.ReflectProj = m_pCamera->GetProjectionMatrix();

		D3DXMatrixTranspose(&ConstantBuffer.View, &ConstantBuffer.View);
		D3DXMatrixTranspose(&ConstantBuffer.Proj, &ConstantBuffer.Proj);
		D3DXMatrixTranspose(&ConstantBuffer.ReflectView, &ConstantBuffer.ReflectView);
		D3DXMatrixTranspose(&ConstantBuffer.ReflectProj, &ConstantBuffer.ReflectProj);

		memcpy_s(
			SubResourceData.pData, 
			SubResourceData.RowPitch, 
			reinterpret_cast<void*>(&ConstantBuffer), 
			sizeof(ConstantBuffer));

		SINGLETON_INSTANCE(Lib::Dx11::GraphicsDevice)->GetDeviceContext()->Unmap(m_pConstantBuffer, 0);

		return true;
	}

	return false;
}
