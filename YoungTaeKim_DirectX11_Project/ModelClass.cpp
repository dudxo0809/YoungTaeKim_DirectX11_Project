#include "stdafx.h"
#include "modelclass.h"
#include "TextureClass.h"

#include <fstream>
using namespace std;

ModelClass::ModelClass()
{
}


ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
{
}


bool ModelClass::Initialize(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename)
{
	// Load Model data
	if (!LoadModel(modelFilename)) {
		return false;
	}

	// ���� �� �ε��� ���۸� �ʱ�ȭ�մϴ�.
	if (!InitializeBuffers(device)) {
		return false;
	}
	return LoadTexture(device, textureFilename);
}


void ModelClass::Shutdown()
{
	// Release Model Texture
	ReleaseTexture();

	// ���ؽ� �� �ε��� ���۸� �����մϴ�.
	ShutdownBuffers();

	// Release Model data
	ReleaseModel();
}


void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// �׸��⸦ �غ��ϱ� ���� �׷��� ������ ���ο� �������� �ε��� ���۸� �����ϴ�.
	RenderBuffers(deviceContext);
}


int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}


bool ModelClass::InitializeBuffers(ID3D11Device* device)
{	
	// Make vertex array
	VertexType* vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Make index array
	unsigned long* indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	// initialize vertex & index from model data
	for (int i = 0; i < m_vertexCount; i++) {
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		indices[i] = i;
	}

	// ���� ���� ������ ����ü�� �����մϴ�.
	D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource ������ ���� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ���� ���۸� ����ϴ�.
	if(FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		return false;
	}

	// ���� �ε��� ������ ����ü�� �����մϴ�.
	D3D11_BUFFER_DESC indexBufferDesc;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// �ε��� �����͸� ����Ű�� ���� ���ҽ� ����ü�� �ۼ��մϴ�.
	D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �ε��� ���۸� �����մϴ�.
	if(FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	// �����ǰ� ���� �Ҵ�� ���� ���ۿ� �ε��� ���۸� �����մϴ�.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}


void ModelClass::ShutdownBuffers()
{
	// �ε��� ���۸� �����մϴ�.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// ���� ���۸� �����մϴ�.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}


void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	// ���� ������ ������ �������� �����մϴ�.
	unsigned int stride = sizeof(VertexType); 
	unsigned int offset = 0;
    
	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // ���� ���۷� �׸� �⺻���� �����մϴ�. ���⼭�� �ﰢ������ �����մϴ�.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool ModelClass::LoadTexture(ID3D11Device* device, WCHAR* textureFilename)
{
	m_Texture = new TextureClass;
	if (!m_Texture) {
		return false;
	}
	return m_Texture->Initialize(device, textureFilename);
}

void ModelClass::ReleaseTexture()
{
	if (m_Texture) {
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

}


bool ModelClass::LoadModel(char* modelFilename)
{
	// Open model file
	ifstream fin;
	fin.open(modelFilename);

	if (fin.fail()) {
		return false;
	}

	// Read to vertex count value
	char input = 0;
	fin.get(input);
	while (input != ':') {
		fin.get(input);
	}

	// Read vrtex count
	fin >> m_vertexCount;

	// Set indexcount to same with vertex count
	m_indexCount = m_vertexCount;

	// Make model reading model data file
	m_model = new ModelType[m_vertexCount];
	if (!m_model) {
		return false;
	}

	fin.get(input);
	while (input != ':') {
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Read vertex data
	for (int i = 0; i < m_vertexCount; i++) {
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	// Close model data file
	fin.close();

	return true;


}

void ModelClass::ReleaseModel()
{
	if (m_model) {
		delete[] m_model;
		m_model = 0;
	}
}

void ModelClass::Rotate(float rotate)
{



}
