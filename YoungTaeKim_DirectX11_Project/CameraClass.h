#pragma once

class CameraClass : public AlignedAllocationPolicy<16>
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);
	void SetLookAtVector(XMFLOAT3 lookAt);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();
	XMFLOAT3 GetLookAtVector();

	void Render();
	void GetViewMatrix(XMMATRIX&);

	const XMFLOAT3 GetCameraForwardDirection();
	const XMFLOAT3 GetCameraRightDirection();

	void CameraMove(const unsigned int key);

private:
	XMFLOAT3 m_position;
	XMFLOAT3 m_rotation;
	XMMATRIX m_viewMatrix;

	XMFLOAT3 m_lookAtVector;
	XMFLOAT3 m_forwardDirection;
	XMFLOAT3 m_rightDirection;
};