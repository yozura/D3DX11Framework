// 전역 변수 및 상수 버퍼
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

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
};

PixelInputType LightVertexShader(VertexInputType input)
{
	PixelInputType output;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex;

	// 월드 행렬에 대해서만 정규 벡터를 연산합니다. (월드 좌표계에서 계산되기 때문에)
	output.normal = mul(input.normal, (float3x3)worldMatrix);

	// 연산한 벡터를 정규화 합니다.
	output.normal = normalize(output.normal);

	return output;
}