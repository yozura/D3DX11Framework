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

	// �������� ����� ���̴� �Ķ���͸� �����մϴ�.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture);
	if (!result) return false;

	// �������� ���̴��� ���۸� �غ��մϴ�.
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

	// ���� ���̴� �ڵ带 �������մϴ�.
	result = D3DX11CompileFromFile(vsPath, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		// ���̴� �����Ͽ� �����ϸ� ���� �޽����� ����ϰ�
		// ���� �޽����� ���ٸ� ������ ������ �ǹ��Ѵ�.
		if (errorMessage) OutputShaderErrorMessage(errorMessage, hWnd, vsPath);
		else MessageBox(hWnd, vsPath, L"Missing Shader File", MB_OK);
		return false;
	}

	// �ȼ� ���̴� �ڵ带 �������մϴ�.
	result = D3DX11CompileFromFile(psPath, NULL, NULL, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		// ���̴� �����Ͽ� �����ϸ� ���� �޽����� ����ϰ�
		// ���� �޽����� ���ٸ� ������ ������ �ǹ��Ѵ�.
		if (errorMessage) OutputShaderErrorMessage(errorMessage, hWnd, psPath);
		else MessageBox(hWnd, psPath, L"Missing Shader File", MB_OK);
		return false;
	}

	// ���۷κ��� ���� ���̴��� �����մϴ�.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result)) return false;

	// ���۷κ��� �ȼ� ���̴��� �����մϴ�.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result)) return false;

	// ���� �Է� ���̾ƿ� descrioption�� �ۼ��մϴ�.
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

	// ���̾ƿ��� ���� ������ �޽��ϴ�.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// ���� �Է� ���̾ƿ��� �����մϴ�.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result)) return false;

	// ���� �ʿ� ������ ���� ���۸� �����մϴ�.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// ���� ��Ʈ���� ��� ������ description�� �ۼ��մϴ�.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// ��� ���� �����͸� �����մϴ�.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result)) return false;

	// ���÷� ���� description�� �ۼ��մϴ�.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;// D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;	// UVW ��ǥ�� ������ 0.0f ~ 1.0f ������ �������� �ϴ� WRAP ����
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

	// ���� �޽��� �ؽ�Ʈ ���� �����͸� �����ɴϴ�.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// �޽��� ���̸� �����ɴϴ�.
	bufferSize = errorMessage->GetBufferSize();

	// �޽����� ����� ������ ���ϴ�.
	fout.open("shader-error.text");

	// �޽����� �ۼ��մϴ�.
	for (i = 0; i < bufferSize; ++i)
		fout << compileErrors[i];

	// ������ �ݽ��ϴ�.
	fout.close();

	// �޽����� �����մϴ�.
	errorMessage->Release();
	errorMessage = 0;

	// �޽����� �����쿡 ���ϴ�.
	MessageBox(hWnd, L"Error compiling shader. Check shader-error.txt for message.", path, MB_OK);
}

bool TextureShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection, ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	// ���̴����� ����� �� �ֵ��� ��ġ��ķ� ��ȯ�մϴ�.
	D3DXMatrixTranspose(&world, &world);
	D3DXMatrixTranspose(&view, &view);
	D3DXMatrixTranspose(&projection, &projection);

	// ��� ���ۿ� ���� �����ֵ��� ����ϴ�.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) return false;

	// ��� ������ �����͸� �����ɴϴ�.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// ��� ���ۿ� ����� �����մϴ�.
	dataPtr->world = world;
	dataPtr->view = view;
	dataPtr->projection = projection;

	// ��� ������ ����� �����մϴ�.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// ���� ���̴��� ������� �������� �ۼ��մϴ�.
	bufferNumber = 0;

	// ���ŵ� ���� ���� ���̴��� ��� ���ۿ� �����մϴ�.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void TextureShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// ���� �Է� ���̴��� �����մϴ�.
	deviceContext->IASetInputLayout(m_layout);

	// �ﰢ�� �׸��� ���� ����, �ȼ� ���̴��� �����մϴ�.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// �ȼ� ���̴��� ���÷� ���¸� �����մϴ�.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// �ﰢ���� �׸��ϴ�.
	deviceContext->DrawIndexed(indexCount, 0, 0);
}