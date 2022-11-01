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
	// 정점 쉐이더와 픽셀 쉐이더를 초기화합니다.
	return InitializeShader(device, hWnd, L"../Engine/color.vs", L"../Engine/color.ps");
}

void ColorShaderClass::Shutdown()
{
	// 정점 쉐이더와 픽셀 쉐이더 및 그와 관련된 것들을 반환한다.
	ShutdownShader();
}

bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	bool result;

	// 렌더링에 사용할 쉐이더의 인자를 입력합니다.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result) return false;

	// 쉐이더를 이용하여 준비된 버퍼를 그립니다.
	RenderShader(deviceContext, indexCount);

	return true;
}

// 이 함수는 쉐이더 파일을 불러오고 DirectX와 GPU에서 사용 가능하도록 합니다.
// 또한 레이아웃을 세팅하고 어떻게 정점 버퍼의 데이터가 GPU에서 사용되는지 볼 수 있습니다.
bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hWnd, WCHAR* vsPath, WCHAR* psPath)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	// 이 함수에서 사용하는 포인터들을 null로 설정합니다.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// 정점 쉐이더를 컴파일합니다.
	result = D3DX11CompileFromFile(vsPath, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);
	if (FAILED(result)) 
	{
		if (errorMessage) OutputShaderErrorMessage(errorMessage, hWnd, vsPath); // 쉐이더가 컴파일에 실패하면 에러 메시지를 기록합니다.
		else MessageBox(hWnd, vsPath, L"Missing Shader File", MB_OK); // 에러 메시지가 없다면 쉐이더 파일을 찾지 못한 것입니다.
		return false;
	}

	// 픽셀 쉐이더를 컴파일합니다.
	result = D3DX11CompileFromFile(psPath, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMessage, NULL);
	if (FAILED(result)) 
	{
		if (errorMessage) OutputShaderErrorMessage(errorMessage, hWnd, psPath); // 쉐이더가 컴파일에 실패하면 에러 메시지를 기록합니다.
		else MessageBox(hWnd, psPath, L"Missing Shader File", MB_OK); // 에러 메시지가 없다면 쉐이더 파일을 찾지 못한 것입니다.
		return false;
	}

	// 버퍼로 부터 정점 쉐이더를 생성합니다.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result)) return false;

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader); 
	if (FAILED(result)) return false;

	// 정점 입력 레이아웃 description을 작성합니다.
	// 이 설정은 ModelClass와 쉐이더에 있는 VertexType과 일치해야 합니다.
	polygonLayout[0].SemanticName = "POSITION";					// 1. 중요
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;		// 2. 중요
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;						// 3. 중요
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// 레이아웃의 요소 개수를 가져옵니다.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	
	// 정점 입력 레이아웃을 생성합니다.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result)) return false;

	// 더 이상 사용되지 않는 정점 쉐이더 버퍼와 픽셀 쉐이더 버퍼를 해제합니다.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// 정점 쉐이더에 있는 행렬 상수 버퍼의 description을 작성합니다.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 상수 버퍼 포인터를 만들어 이 클래스에서 정점 쉐이더 상수 버퍼에 접근할 수 있게 한다.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result)) return false;

	return true;
}

void ColorShaderClass::ShutdownShader()
{
	// 상수 버퍼를 해제합니다.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// 입력 레이아웃을 해제합니다.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// 픽셀 쉐이더를 해제합니다.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_matrixBuffer = 0;
	}

	// 정점 쉐이더를 해제합니다.
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

	// 에러 메시지를 담고 있는 문자열 버퍼의 포인터를 가져옵니다.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// 메시지의 길이를 가져옵니다.
	bufferSize = errorMessage->GetBufferSize();

	// 파일을 열고 안에 메시지를 기록합니다.
	fout.open("shader-error.txt");

	// 에러 메시지를 씁니다.
	for (i = 0; i < bufferSize; ++i)
		fout << compileErrors[i];

	// 파일을 닫습니다.
	fout.close();

	// 에러 메시지를 반환합니다.
	errorMessage->Release();
	errorMessage = 0;

	// 컴파일 에러가 있음을 팝업 메시지로 알려줍니다.
	MessageBox(hWnd, L"Error compiling shader. Check shader-error.txt for message", path, MB_OK);
}

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	// 행렬을 transpose하여 쉐이더에서 사용할 수 있게 합니다.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	// 상수 버퍼의 내용을 쓸 수 있도록 잠급니다.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) return false;

	// 상수 버퍼의 데이터에 대한 포인터를 가져옵니다.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// 상수 버퍼에 행렬을 복사합니다.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// 상수 버퍼의 잠금을 풉니다.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// 정점 쉐이더에서의 상수 버퍼의 위치를 설정합니다.
	bufferNumber = 0;
	
	// 마지막으로 정점 쉐이더의 상수 버퍼를 바뀐 값으로 바꿉니다.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// 정점 입력 레이아웃을 설정합니다.
	deviceContext->IASetInputLayout(m_layout);

	// 삼각형을 그릴 정점 쉐이더와 픽셀 쉐이더를 설정합니다.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// 삼각형을 그립니다.
	deviceContext->DrawIndexed(indexCount, 0, 0);
}