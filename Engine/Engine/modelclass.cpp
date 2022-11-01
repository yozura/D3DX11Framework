#include "modelclass.h"

ModelClass::ModelClass()
{
	m_vertexCount = m_indexCount = 0;
}

ModelClass::ModelClass(const ModelClass& other) {}
ModelClass::~ModelClass() {}

bool ModelClass::Initialize(ID3D11Device* device)
{
	// ���� ���ۿ� �ε��� ���۸� �ʱ�ȭ�մϴ�.
	return InitializeBuffers(device);
}

void ModelClass::Shutdown()
{
	// ���� ���ۿ� �ε��� ���۸� �����մϴ�.
	ShutdownBuffers();
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// ���� ���ۿ� �ε��� ���۸� �׷��Ƚ� ���������ο� �־� ȭ�鿡 �׸� �غ� �մϴ�.
	RenderBuffers(deviceContext);
}

// ���� �ε��� ������ �˷��ݴϴ�.
// �÷� ���̴����� ���� �׸��� ���ؼ��� �� ������ �ʿ��մϴ�.
int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

// ���� ������ ���Ϸκ��� ���� ������ �о�ͼ� ���۸� ����� ���� ��.
bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// ���� �迭�� ���̸� �����մϴ�.
	m_vertexCount = 4;

	// �ε��� �迭�� ���̸� �����մϴ�.
	m_indexCount = 6;

	// ���� �迭�� �����մϴ�.
	vertices = new VertexType[m_vertexCount];
	if (!vertices) return false;

	// �ε��� �迭�� �����մϴ�.
	indices = new unsigned long[m_indexCount];
	if (!indices) return false;

	/* !����! ������ �׻� �ð� �������� ������ �� */
	// ���� �迭�� ���� �ֽ��ϴ�.
	vertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f); // ���� �Ʒ�
	vertices[0].color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);

	vertices[1].position = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);	// ���� ��
	vertices[1].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);	// ������ �Ʒ�
	vertices[2].color = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);

	// 2��° �ﰢ��
	//vertices[3].position = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);	// ���� ��
	//vertices[3].color = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);

	vertices[3].position = D3DXVECTOR3(1.0f, 1.0f, 0.0f);	// ������ ��(New)
	vertices[3].color = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);

	//vertices[5].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);	// ������ �Ʒ�
	//vertices[5].color = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);

	// �ε��� �迭�� ���� �ֽ��ϴ�.
	indices[0] = 0; // ���� �Ʒ�
	indices[1] = 1; // ���� ��
	indices[2] = 2; // ������ �Ʒ�
	indices[3] = 1; 
	indices[4] = 3; 
	indices[5] = 2; 

	// ���� ���ۿ� ������ ��ġ, ������ �����صΰ�
	// �ε��� ���ۿ��� �� ������ �ε����� �����صд�.
	// �ε��� ���۴� 0�������� �� ���� �б� ������ �ð� �������� �� ���� �ﰢ���� ������ ������ �Ѵ�.

	// ���� ������ description�� �ۼ��մϴ�.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// ���� �����͸� ����Ű�� ���� ���ҽ� ����ü�� �ۼ��մϴ�.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ���۸� �����մϴ�.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result)) return false;

	// �ε��� ������ description�� �ۼ��մϴ�.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// �ε��� �����͸� ����Ű�� ���� ���ҽ� ����ü�� �ۼ��մϴ�.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �ε��� ���۸� �����մϴ�.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result)) return false;

	// �����ǰ� ���� �Ҵ�� ���� ���ۿ� �ε��� ���۸� �����մϴ�.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
	
	return true;
}

void ModelClass::ShutdownBuffers()
{
	// �ε��� ���۸� �����մϴ�.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// ���� ���۸� �����մϴ�.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}

// ������ ���� ���ۿ� �ε��� ���۸� GPU�� ����� ���۷μ� Ȱ��ȭ ��Ű�� �Լ�
void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// ���� ������ ������ �������� �����մϴ�.
	stride = sizeof(VertexType);
	offset = 0;

	// Input assembler�� ���� ���۸� Ȱ��ȭ�Ͽ� �׷��� �� �ְ� �մϴ�.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Input assembler�� �ε��� ���۸� Ȱ��ȭ�Ͽ� �׷��� �� �ְ� �մϴ�.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// ���� ���۷� �׸� �⺻���� �����մϴ�. ���⼭�� �ﰢ���Դϴ�.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}