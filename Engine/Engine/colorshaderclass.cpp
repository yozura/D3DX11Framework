#include "colorshaderclass.h"

ColorShaderClass::ColorShaderClass() 
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass& other) {}
ColorShaderClass::~ColorShaderClass() {}

bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hWnd)
{
	// ���� ���̴��� �ȼ� ���̴��� �ʱ�ȭ�մϴ�.
	return InitializeShader(device, hWnd, L"../Engine/color.vs", L"../Engine/color.ps");
}

void ColorShaderClass::Shutdown()
{
	// ���� ���̴��� �ȼ� ���̴� �� �׿� ���õ� �͵��� ��ȯ�Ѵ�.
	ShutdownShader();
}

bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	bool result;

	// �������� ����� ���̴��� ���ڸ� �Է��մϴ�.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result) return false;

	// ���̴��� �̿��Ͽ� �غ�� ���۸� �׸��ϴ�.
	RenderShader(deviceContext, indexCount);

	return true;
}

// �� �Լ��� ���̴� ������ �ҷ����� DirectX�� GPU���� ��� �����ϵ��� �մϴ�.
// ���� ���̾ƿ��� �����ϰ� ��� ���� ������ �����Ͱ� GPU���� ���Ǵ��� �� �� �ֽ��ϴ�.
bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hWnd, WCHAR* vsPath, WCHAR* psPath)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	// �� �Լ����� ����ϴ� �����͵��� null�� �����մϴ�.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// ���� ���̴��� �������մϴ�.
	result = D3DX11CompileFromFile(vsPath, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);
	if (FAILED(result)) 
	{
		if (errorMessage) OutputShaderErrorMessage(errorMessage, hWnd, vsPath); // ���̴��� �����Ͽ� �����ϸ� ���� �޽����� ����մϴ�.
		else MessageBox(hWnd, vsPath, L"Missing Shader File", MB_OK); // ���� �޽����� ���ٸ� ���̴� ������ ã�� ���� ���Դϴ�.
		return false;
	}

	// �ȼ� ���̴��� �������մϴ�.
	result = D3DX11CompileFromFile(psPath, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMessage, NULL);
	if (FAILED(result)) 
	{
		if (errorMessage) OutputShaderErrorMessage(errorMessage, hWnd, psPath); // ���̴��� �����Ͽ� �����ϸ� ���� �޽����� ����մϴ�.
		else MessageBox(hWnd, psPath, L"Missing Shader File", MB_OK); // ���� �޽����� ���ٸ� ���̴� ������ ã�� ���� ���Դϴ�.
		return false;
	}

	// ���۷� ���� ���� ���̴��� �����մϴ�.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result)) return false;

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader); 
	if (FAILED(result)) return false;

	// ���� �Է� ���̾ƿ� description�� �ۼ��մϴ�.
	// �� ������ ModelClass�� ���̴��� �ִ� VertexType�� ��ġ�ؾ� �մϴ�.
	polygonLayout[0].SemanticName = "POSITION";					// 1. �߿�
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;		// 2. �߿�
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;						// 3. �߿�
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// ���̾ƿ��� ��� ������ �����ɴϴ�.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	
	// ���� �Է� ���̾ƿ��� �����մϴ�.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result)) return false;

	// �� �̻� ������ �ʴ� ���� ���̴� ���ۿ� �ȼ� ���̴� ���۸� �����մϴ�.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// ���� ���̴��� �ִ� ��� ��� ������ description�� �ۼ��մϴ�.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// ��� ���� �����͸� ����� �� Ŭ�������� ���� ���̴� ��� ���ۿ� ������ �� �ְ� �Ѵ�.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result)) return false;

	return true;
}

void ColorShaderClass::ShutdownShader()
{
	// ��� ���۸� �����մϴ�.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// �Է� ���̾ƿ��� �����մϴ�.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// �ȼ� ���̴��� �����մϴ�.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_matrixBuffer = 0;
	}

	// ���� ���̴��� �����մϴ�.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
}

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hWnd, WCHAR* path)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;

	// ���� �޽����� ��� �ִ� ���ڿ� ������ �����͸� �����ɴϴ�.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// �޽����� ���̸� �����ɴϴ�.
	bufferSize = errorMessage->GetBufferSize();

	// ������ ���� �ȿ� �޽����� ����մϴ�.
	fout.open("shader-error.txt");

	// ���� �޽����� ���ϴ�.
	for (i = 0; i < bufferSize; ++i)
		fout << compileErrors[i];

	// ������ �ݽ��ϴ�.
	fout.close();

	// ���� �޽����� ��ȯ�մϴ�.
	errorMessage->Release();
	errorMessage = 0;

	// ������ ������ ������ �˾� �޽����� �˷��ݴϴ�.
	MessageBox(hWnd, L"Error compiling shader. Check shader-error.txt for message", path, MB_OK);
}

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	// ����� transpose�Ͽ� ���̴����� ����� �� �ְ� �մϴ�.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	// ��� ������ ������ �� �� �ֵ��� ��޴ϴ�.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) return false;

	// ��� ������ �����Ϳ� ���� �����͸� �����ɴϴ�.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// ��� ���ۿ� ����� �����մϴ�.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// ��� ������ ����� Ǳ�ϴ�.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// ���� ���̴������� ��� ������ ��ġ�� �����մϴ�.
	bufferNumber = 0;
	
	// ���������� ���� ���̴��� ��� ���۸� �ٲ� ������ �ٲߴϴ�.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// ���� �Է� ���̾ƿ��� �����մϴ�.
	deviceContext->IASetInputLayout(m_layout);

	// �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� �����մϴ�.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// �ﰢ���� �׸��ϴ�.
	deviceContext->DrawIndexed(indexCount, 0, 0);
}