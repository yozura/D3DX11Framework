#include "textureshaderclass.h"

TextureShaderClass::TextureShaderClass()
	: m_vertexShader(0)
	, m_pixelShader(0)
	, m_layout(0)
	, m_matrixBuffer(0)
	, m_sampleState(0)
{
}

TextureShaderClass::TextureShaderClass(const TextureShaderClass& other) {}
TextureShaderClass::~TextureShaderClass() {}

bool TextureShaderClass::Initialize(ID3D11Device* device, HWND hWnd)
{
	bool result;

	result = InitializeShader(device, hWnd, L"../Engine/texture.vs", L"../Engine/texture.ps");
	if (!result) return false;

	return true;
}

void TextureShaderClass::Shutdown()
{
	ShutdownShader();
}

bool TextureShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	bool result;

	// 렌더링에 사용할 쉐이더 파라미터를 세팅합니다.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture);
	if (!result) return false;

	// 렌더링할 쉐이더와 버퍼를 준비합니다.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool TextureShaderClass::InitializeShader(ID3D11Device* device, HWND hWnd, WCHAR* vsPath, WCHAR* psPath)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	errorMessage = vertexShaderBuffer = pixelShaderBuffer = 0;

	// 정점 쉐이더 코드를 컴파일합니다.
	result = D3DX11CompileFromFile(vsPath, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		// 쉐이더 컴파일에 실패하면 에러 메시지를 출력하고
		// 에러 메시지가 없다면 파일이 없음을 의미한다.
		if (errorMessage) OutputShaderErrorMessage(errorMessage, hWnd, vsPath);
		else MessageBox(hWnd, vsPath, L"Missing Shader File", MB_OK);
		return false;
	}

	// 픽셀 쉐이더 코드를 컴파일합니다.
	result = D3DX11CompileFromFile(psPath, NULL, NULL, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		// 쉐이더 컴파일에 실패하면 에러 메시지를 출력하고
		// 에러 메시지가 없다면 파일이 없음을 의미한다.
		if (errorMessage) OutputShaderErrorMessage(errorMessage, hWnd, psPath);
		else MessageBox(hWnd, psPath, L"Missing Shader File", MB_OK);
		return false;
	}

	// 버퍼로부터 정점 쉐이더를 생성합니다.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result)) return false;

	// 버퍼로부터 픽셀 쉐이더를 생성합니다.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result)) return false;

	// 정점 입력 레이아웃 descrioption을 작성합니다.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// 레이아웃의 원소 개수를 받습니다.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// 정점 입력 레이아웃을 생성합니다.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result)) return false;

	// 이제 필요 없어진 사용된 버퍼를 해제합니다.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// 동적 매트릭스 상수 버퍼의 description을 작성합니다.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 상수 버퍼 포인터를 생성합니다.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result)) return false;

	// 샘플러 상태 description을 작성합니다.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;// D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;	// UVW 좌표가 언제나 0.0f ~ 1.0f 범위를 가지도록 하는 WRAP 설정
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result)) return false;

	return true;
}

void TextureShaderClass::ShutdownShader()
{
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
}

void TextureShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hWnd, WCHAR* path)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;

	// 에러 메시지 텍스트 버퍼 포인터를 가져옵니다.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// 메시지 길이를 가져옵니다.
	bufferSize = errorMessage->GetBufferSize();

	// 메시지를 기록할 파일을 엽니다.
	fout.open("shader-error.text");

	// 메시지를 작성합니다.
	for (i = 0; i < bufferSize; ++i)
		fout << compileErrors[i];

	// 파일을 닫습니다.
	fout.close();

	// 메시지를 해제합니다.
	errorMessage->Release();
	errorMessage = 0;

	// 메시지를 윈도우에 띄웁니다.
	MessageBox(hWnd, L"Error compiling shader. Check shader-error.txt for message.", path, MB_OK);
}

bool TextureShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection, ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	// 쉐이더에서 사용할 수 있도록 전치행렬로 변환합니다.
	D3DXMatrixTranspose(&world, &world);
	D3DXMatrixTranspose(&view, &view);
	D3DXMatrixTranspose(&projection, &projection);

	// 상수 버퍼에 값을 쓸수있도록 잠굽니다.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) return false;

	// 상수 버퍼의 포인터를 가져옵니다.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// 상수 버퍼에 행렬을 복사합니다.
	dataPtr->world = world;
	dataPtr->view = view;
	dataPtr->projection = projection;

	// 상수 버퍼의 잠금을 해제합니다.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// 정점 쉐이더의 상수버퍼 포지션을 작성합니다.
	bufferNumber = 0;

	// 갱신된 값을 정점 쉐이더의 상수 버퍼에 세팅합니다.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void TextureShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// 정점 입력 쉐이더를 세팅합니다.
	deviceContext->IASetInputLayout(m_layout);

	// 삼각형 그리기 위해 정점, 픽셀 쉐이더를 세팅합니다.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// 픽셀 쉐이더의 샘플러 상태를 세팅합니다.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// 삼각형을 그립니다.
	deviceContext->DrawIndexed(indexCount, 0, 0);
}