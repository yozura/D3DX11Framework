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
	
	// 모델을 불러옵니다.
	result = LoadModel(modelPath);
	if (!result) return false;

	// 정점 버퍼와 인덱스 버퍼를 초기화합니다
	result = InitializeBuffers(device);
	if (!result) return false;

	// 텍스쳐를 불러옵니다.
	result = LoadTexture(device, texturePath);
	if (!result) return false;

	return true;
}

void ModelClass::Shutdown()
{
	// 텍스쳐를 해제합니다.
	ReleaseTexture();

	// 정점 버퍼와 인덱스 버퍼를 해제합니다.
	ShutdownBuffers();

	// 모델 파일을 해제합니다.
	ReleaseModel();
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// 정점 버퍼와 인덱스 버퍼를 그래픽스 파이프라인에 넣어 화면에 그릴 준비를 합니다.
	RenderBuffers(deviceContext);
}

// 모델의 인덱스 개수를 알려줍니다.
// 컬러 쉐이더에서 모델을 그리기 위해서는 이 정보가 필요합니다.
int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}

// 보통 데이터 파일로부터 모델의 정보를 읽어와서 버퍼를 만드는 일을 함.
bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	// 정점 배열의 길이를 설정합니다.
	//m_vertexCount = 4;

	// 인덱스 배열의 길이를 설정합니다.
	//m_indexCount = 6;

	// 정점 배열을 생성합니다.
	vertices = new VertexType[m_vertexCount];
	if (!vertices) return false;

	// 인덱스 배열을 생성합니다.
	indices = new unsigned long[m_indexCount];
	if (!indices) return false;

	// float alpha = 2.0f;
	
	// 정점 버퍼에 정점의 위치, 색상을 저장해두고
	// 인덱스 버퍼에서 그 정점의 인덱스를 저장해둔다.
	// 인덱스 버퍼는 0에서부터 세 개씩 읽기 때문에 시계 방향으로 세 개씩 삼각형의 정점을 만들어야 한다.

	/* !주의! 정점은 항상 시계 방향으로 만들어야 함 */
	// 정점 배열에 값을 넣습니다.
	//vertices[0].position = D3DXVECTOR3(1.0f * alpha, 1.0f * alpha, 0.0f);	// 오른쪽 위
	//vertices[0].texture = D3DXVECTOR2(1.0f, 0.0f);
	//vertices[0].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	//vertices[1].position = D3DXVECTOR3(1.0f * alpha, -1.0f * alpha, 0.0f);	// 오른쪽 아래
	//vertices[1].texture = D3DXVECTOR2(1.0f, 1.0f);
	//vertices[1].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	//
	//vertices[2].position = D3DXVECTOR3(-1.0f * alpha, -1.0f * alpha, 0.0f);	// 왼쪽 아래
	//vertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);
	//vertices[2].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	//vertices[3].position = D3DXVECTOR3(-1.0f * alpha, 1.0f * alpha, 0.0f);	// 왼쪽 위
	//vertices[3].texture = D3DXVECTOR2(0.0f, 0.0f);
	//vertices[3].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	//// 인덱스 배열에 값을 넣습니다.
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

	// 정점 버퍼의 description을 작성합니다.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 정점 데이터를 가리키는 보조 리소스 구조체를 작성합니다.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 정점 버퍼를 생성합니다.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result)) return false;

	// 인덱스 버퍼의 description을 작성합니다.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 인덱스 데이터를 가리키는 보조 리소스 구조체를 작성합니다.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 생성합니다.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result)) return false;

	// 생성되고 값이 할당된 정점 버퍼와 인덱스 버퍼를 해제합니다.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
	
	return true;
}

void ModelClass::ShutdownBuffers()
{
	// 인덱스 버퍼를 해제합니다.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// 정점 버퍼를 해제합니다.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}

// 생성한 정점 버퍼와 인덱스 버퍼를 GPU의 어셈블러 버퍼로서 활성화 시키는 함수
void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// 정점 버퍼의 단위와 오프셋을 설정합니다.
	stride = sizeof(VertexType);
	offset = 0;

	// Input assembler에 정점 버퍼를 활성화하여 그려질 수 있게 합니다.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Input assembler에 인덱스 버퍼를 활성화하여 그려질 수 있게 합니다.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 정점 버퍼로 그릴 기본형을 설정합니다. 여기서는 삼각형입니다.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool ModelClass::LoadTexture(ID3D11Device* device, WCHAR* texturePath)
{
	bool result;

	// 텍스쳐 객체 생성
	m_Texture = new TextureClass();
	if (!m_Texture) return false;

	// 텍스쳐 객체 초기화
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