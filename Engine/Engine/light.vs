// 전역 변수 및 상수 버퍼
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer CameraBuffer
{
	float3 cameraPosition;
	float padding;
}

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
};

PixelInputType LightVertexShader(VertexInputType input)
{
	PixelInputType output;
	float4 worldPosition;

	input.position.w = 1.0f;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex;

	// 월드 행렬에 대해서만 정규 벡터를 연산합니다. (월드 좌표계에서 계산되기 때문에)
	output.normal = mul(input.normal, (float3x3)worldMatrix);

	// 연산한 벡터를 정규화 합니다.
	output.normal = normalize(output.normal);

	// 월드에서의 정점의 위치를 계산합니다.
	worldPosition = mul(input.position, worldMatrix);

	// 카메라의 위치에서 정점의 위치를빼면 정점에서 카메라를 바라보는 방향이 나옵니다.
	output.viewDirection = cameraPosition.xyz - worldPosition.xyz;

	// 그 방향의 크기를 정규화 시켜서 방향을 유지한 채로 크기를 1로 만듭니다.
	output.viewDirection = normalize(output.viewDirection);

	return output;
}