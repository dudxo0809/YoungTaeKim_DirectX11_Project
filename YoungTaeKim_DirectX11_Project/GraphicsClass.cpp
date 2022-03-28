#include "stdafx.h"
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "LightClass.h"
#include "LightShaderClass.h"
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	rotationY = 0.0f;
	rotationX = 0.0f;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	// Direct3D 객체 생성
	m_Direct3D = new D3DClass;
	if(!m_Direct3D)
	{
		return false;
	}

	// Direct3D 객체 초기화
	if(!m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// m_Camera 객체 생성
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// 카메라 포지션 설정
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	// m_Model 객체 생성
	m_Model = new ModelClass;
	if (!m_Model)
	{
		return false;
	}

	// m_Model 객체 초기화

	WCHAR textureFilepath[] = L"../YoungTaeKim_DirectX11_Project/data/seafloor.dds";
	char modelFilepath[] = "../YoungTaeKim_DirectX11_Project/data/cube.txt";
	if (!m_Model->Initialize(m_Direct3D->GetDevice(), modelFilepath, textureFilepath))
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Create & Initialize m_LightShader Object
	m_LightShader = new LightShaderClass;
	if (!m_LightShader) {
		return false;
	}
	if (!m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error!", MB_OK);
		return false;
	}

	// Create & Initialize m_Light Object
	m_Light = new LightClass;
	if (!m_Light) {
		return false;
	}
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);

	return true;
}


void GraphicsClass::Shutdown()
{
	// Release m_Light & m_LightShader
	if (m_Light) {
		delete m_Light;
		m_Light = 0;
	}
	if (m_LightShader) {
		delete m_LightShader;
		m_LightShader = 0;
	}

	// m_Model 객체 반환
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// m_Camera 객체 반환
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Direct3D 객체 반환
	if(m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}
}


bool GraphicsClass::Frame()
{

	// 그래픽 랜더링 처리
	return Render(rotationY, rotationX);
}

void GraphicsClass::MoveCamera(unsigned int key)
{
	m_Camera->CameraMove(key);
}

void GraphicsClass::Rotate(const unsigned int key)
{
	switch (key) {
		case VK_LEFT:
		{
			rotationY -= 0.03f;
			break;
		}
		case VK_RIGHT: {
			rotationY += 0.03f;
			break;
		}
		case VK_UP: {
			rotationX -= 0.03f;
			break;
		}
		case VK_DOWN: {
			rotationX += 0.03f;
			break;
		}
	}

	
};


bool GraphicsClass::Render(float rotationY, float rotationX)
{

	// 씬을 그리기 위해 버퍼를 지웁니다
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 삼각형이 회전 할 수 있도록 회전 값으로 월드 행렬을 회전합니다.
	worldMatrix = XMMatrixRotationY(rotationY);
	//worldMatrix = XMMatrixRotationX(rotationX);
	

	// 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비합니다.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// Render model using Light shader
	if (!m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(),
		worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor())) 
	{
		return false;
	}

	// 버퍼의 내용을 화면에 출력합니다
	m_Direct3D->EndScene();

	return true;
}