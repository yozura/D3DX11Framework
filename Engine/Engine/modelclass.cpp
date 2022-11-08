#include "modelclass.h"

ModelClass::ModelClass()
	: m_vertexCount(0)
	, m_indexCount(0)
	, m_Texture(0)
{}

ModelClass::ModelClass(const ModelClass& other) {}
ModelClass::~ModelClass() {}

bool ModelClass::Initialize(ID3D11Device* device, char* modelPath, WCHAR* texturePath)
{
	bool result;
	
	// ���� �ҷ��ɴϴ�.
	result = LoadModel(modelPath);
	if (!result) return false;

	// ���� ���ۿ� �ε��� ���۸� �ʱ�ȭ�մϴ�
	result = InitializeBuffers(device);
	if (!result) return false;

	// �ؽ��ĸ� �ҷ��ɴϴ�.
	result = LoadTexture(device, texturePath);
	if (!result) return false;

	return true;
}

void ModelClass::Shutdown()
{
	// �ؽ��ĸ� �����մϴ�.
	ReleaseTexture();

	// ���� ���ۿ� �ε��� ���۸� �����մϴ�.
	ShutdownBuffers();

	// �� ������ �����մϴ�.
	ReleaseModel();
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

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}

// ���� ������ ���Ϸκ��� ���� ������ �о�ͼ� ���۸� ����� ���� ��.
bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	// ���� �迭�� ���̸� �����մϴ�.
	//m_vertexCount = 4;

	// �ε��� �迭�� ���̸� �����մϴ�.
	//m_indexCount = 6;

	// ���� �迭�� �����մϴ�.
	vertices = new VertexType[m_vertexCount];
	if (!vertices) return false;

	// �ε��� �迭�� �����մϴ�.
	indices = new unsigned long[m_indexCount];
	if (!indices) return false;

	// float alpha = 2.0f;
	
	// ���� ���ۿ� ������ ��ġ, ������ �����صΰ�
	// �ε��� ���ۿ��� �� ������ �ε����� �����صд�.
	// �ε��� ���۴� 0�������� �� ���� �б� ������ �ð� �������� �� ���� �ﰢ���� ������ ������ �Ѵ�.

	/* !����! ������ �׻� �ð� �������� ������ �� */
	// ���� �迭�� ���� �ֽ��ϴ�.
	//vertices[0].position = D3DXVECTOR3(1.0f * alpha, 1.0f * alpha, 0.0f);	// ������ ��
	//vertices[0].texture = D3DXVECTOR2(1.0f, 0.0f);
	//vertices[0].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	//vertices[1].position = D3DXVECTOR3(1.0f * alpha, -1.0f * alpha, 0.0f);	// ������ �Ʒ�
	//vertices[1].texture = D3DXVECTOR2(1.0f, 1.0f);
	//vertices[1].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	//
	//vertices[2].position = D3DXVECTOR3(-1.0f * alpha, -1.0f * alpha, 0.0f);	// ���� �Ʒ�
	//vertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);
	//vertices[2].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	//vertices[3].position = D3DXVECTOR3(-1.0f * alpha, 1.0f * alpha, 0.0f);	// ���� ��
	//vertices[3].texture = D3DXVECTOR2(0.0f, 0.0f);
	//vertices[3].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	//// �ε��� �迭�� ���� �ֽ��ϴ�.
	//indices[0] = 1;
	//indices[1] = 2;
	//indices[2] = 3;
	//indices[3] = 0;
	//indices[4] = 1;
	//indices[5] = 3;

	for (i = 0; i < m_vertexCount; ++i)
	{
		vertices[i].position = D3DXVECTOR3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = D3DXVECTOR2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = D3DXVECTOR3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		indices[i] = i;
	}

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

bool ModelClass::LoadTexture(ID3D11Device* device, WCHAR* texturePath)
{
	bool result;

	// �ؽ��� ��ü ����
	m_Texture = new TextureClass();
	if (!m_Texture) return false;

	// �ؽ��� ��ü �ʱ�ȭ
	result = m_Texture->Initialize(device, texturePath);
	if (!result) return false;

	return true;
}

void ModelClass::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}
}

bool ModelClass::LoadModel(char* modelPath)
{
	ifstream fin;
	char input;
	int i;

	fin.open(modelPath);

	fin.get(input);
	while (input != ':')
		fin.get(input);

	fin >> m_vertexCount;
	m_indexCount = m_vertexCount;

	m_model = new ModelType[m_vertexCount];
	if (!m_model) return false;

	fin.get(input);
	while (input != ':')
		fin.get(input);
	
	for (i = 0; i < m_vertexCount; ++i) 
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	fin.close();
	return true;
}

void ModelClass::ReleaseModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}
}