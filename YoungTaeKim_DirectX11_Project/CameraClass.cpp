﻿#include "stdafx.h"
#include "cameraclass.h"


CameraClass::CameraClass()
{
	m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_lookAtVector = XMFLOAT3(0.0f, 0.0f, 1.0f);
}


CameraClass::CameraClass(const CameraClass& other)
{
}


CameraClass::~CameraClass()
{
}


void CameraClass::SetPosition(float x, float y, float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
}


void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotation.x = x;
	m_rotation.y = y;
	m_rotation.z = z;
}

void CameraClass::SetLookAtVector(XMFLOAT3 lookAt)
{
	m_lookAtVector = lookAt;
}


XMFLOAT3 CameraClass::GetPosition()
{
	return m_position;
}


XMFLOAT3 CameraClass::GetRotation()
{
	return m_rotation;
}

XMFLOAT3 CameraClass::GetLookAtVector()
{
	return m_lookAtVector;
}


void CameraClass::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;


	// 위쪽을 가리키는 벡터를 설정합니다.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// XMVECTOR 구조체에 로드한다.
	upVector = XMLoadFloat3(&up);

	// 3D월드에서 카메라의 위치를 ​​설정합니다.
	position = m_position;

	// XMVECTOR 구조체에 로드한다.
	positionVector = XMLoadFloat3(&position);

	// 기본적으로 카메라가 찾고있는 위치를 설정합니다.
	//lookAt.x = 0.0f;
	//lookAt.y = 0.0f;
	//lookAt.z = 1.0f;
	lookAt = GetLookAtVector();

	// XMVECTOR 구조체에 로드한다.
	lookAtVector = XMLoadFloat3(&lookAt);

	// yaw (Y 축), pitch (X 축) 및 roll (Z 축)의 회전값을 라디안 단위로 설정합니다.
	pitch = m_rotation.x * 0.0174532925f;
	yaw = m_rotation.y * 0.0174532925f;
	roll = m_rotation.z * 0.0174532925f;

	//  yaw, pitch, roll 값을 통해 회전 행렬을 만듭니다.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// lookAt 및 up 벡터를 회전 행렬로 변형하여 뷰가 원점에서 올바르게 회전되도록 합니다.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// 회전 된 카메라 위치를 뷰어 위치로 변환합니다.
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// 마지막으로 세 개의 업데이트 된 벡터에서 뷰 행렬을 만듭니다.
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}


void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}

const XMFLOAT3 CameraClass::GetCameraForwardDirection()
{
	XMFLOAT3 cameraPosition(m_position.x, m_position.y, m_position.z);
	XMVECTOR cameraPositionVector = XMLoadFloat3(&cameraPosition);
	XMVECTOR lookAtVector = XMLoadFloat3(&m_lookAtVector);
	XMVECTOR directionVector = XMVector3Normalize(lookAtVector - cameraPositionVector);
	XMStoreFloat3(&m_forwardDirection, directionVector);


	return m_forwardDirection;
}

const XMFLOAT3 CameraClass::GetCameraRightDirection()
{
	XMFLOAT3 upDirection(0, 1, 0);
	XMVECTOR upVector(XMLoadFloat3(&upDirection));
	XMVECTOR rightVector = XMVector3Normalize(XMVector3Cross(upVector, XMLoadFloat3(&m_forwardDirection)));
	XMStoreFloat3(&m_rightDirection, rightVector);

	return m_rightDirection;
}

void CameraClass::CameraMove(const unsigned int key)
{
	XMFLOAT3 cameraPosition = GetPosition();
	XMFLOAT3 lookAt = GetLookAtVector();
	XMFLOAT3 cameraForwardDirection = GetCameraForwardDirection();
	XMFLOAT3 cameraRightDirection = GetCameraRightDirection();

	switch (key) {
	case VK_UP: 
	{
		cameraPosition.x += cameraForwardDirection.x * 0.05f;
		cameraPosition.y += cameraForwardDirection.y * 0.05f;
		cameraPosition.z += cameraForwardDirection.z * 0.05f;
		lookAt.x += cameraForwardDirection.x * 0.05f;
		lookAt.y += cameraForwardDirection.y * 0.05f;
		lookAt.z += cameraForwardDirection.z * 0.05f;

		break;
	}
	
	case VK_DOWN: 
	{
		cameraPosition.x -= cameraForwardDirection.x * 0.05f;
		cameraPosition.y -= cameraForwardDirection.y * 0.05f;
		cameraPosition.z -= cameraForwardDirection.z * 0.05f;
		lookAt.x -= cameraForwardDirection.x * 0.05f;
		lookAt.y -= cameraForwardDirection.y * 0.05f;
		lookAt.z -= cameraForwardDirection.z * 0.05f;

		break;
	}
	/*
	case VK_RIGHT:
	{
		cameraPosition.x += cameraRightDirection.x * 0.02f;
		cameraPosition.y += cameraRightDirection.y * 0.02f;
		cameraPosition.z += cameraRightDirection.z * 0.02f;
		lookAt.x += cameraRightDirection.x * 0.02f;
		lookAt.y += cameraRightDirection.y * 0.02f;
		lookAt.z += cameraRightDirection.z * 0.02f;

		break;
	}

	case VK_LEFT:
	{
		cameraPosition.x -= cameraRightDirection.x * 0.02f;
		cameraPosition.y -= cameraRightDirection.y * 0.02f;
		cameraPosition.z -= cameraRightDirection.z * 0.02f;
		lookAt.x -= cameraRightDirection.x * 0.02f;
		lookAt.y -= cameraRightDirection.y * 0.02f;
		lookAt.z -= cameraRightDirection.z * 0.02f;

		break;
	}
	*/

	}

	SetPosition(cameraPosition.x, cameraPosition.y, cameraPosition.z);
	SetLookAtVector(lookAt);

}
