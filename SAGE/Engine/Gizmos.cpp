#include "Gizmos.h"
#include <Effects.h>
#include <CommonStates.h>

std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> Gizmos::primitiveBatch = nullptr;
std::unique_ptr<DirectX::BasicEffect> Gizmos::basicEffect = nullptr;
ID3D11DeviceContext* Gizmos::deviceContext = nullptr;

void Gizmos::Initialize(ID3D11DeviceContext* context, ID3D11Device* device)
{
	deviceContext = context;
	primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(deviceContext);
	basicEffect = std::make_unique<DirectX::BasicEffect>(device);
	basicEffect->SetVertexColorEnabled(true);

	void const* shaderByteCode;
	size_t byteCodeLength;

	basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	ID3D11InputLayout* inputLayout;
	device->CreateInputLayout(DirectX::VertexPositionColor::InputElements,
							  DirectX::VertexPositionColor::InputElementCount,
							  shaderByteCode, byteCodeLength,
							  &inputLayout);

	deviceContext->IASetInputLayout(inputLayout);
	inputLayout->Release();
}

void Gizmos::Begin()
{
	basicEffect->Apply(deviceContext);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	primitiveBatch->Begin();
}

void Gizmos::End()
{
	primitiveBatch->End();
}

void Gizmos::DrawLine(const DirectX::XMVECTOR& start, const DirectX::XMVECTOR& end, const DirectX::XMVECTORF32& color, float thickness)
{
	// Note: Thickness is not natively supported in PrimitiveBatch, this is a placeholder for potential shader-based implementation.
	primitiveBatch->DrawLine(DirectX::VertexPositionColor(start, color), DirectX::VertexPositionColor(end, color));
}