#pragma once

#include <DirectXMath.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <Effects.h>
#include <memory>

class Gizmos
{
public:
	static void Initialize(ID3D11DeviceContext* context, ID3D11Device* device);
	static void DrawLine(const DirectX::XMVECTOR& start, const DirectX::XMVECTOR& end, const DirectX::XMVECTORF32& color, float thickness = 1.0f);
	static void Begin();
	static void End();
	
private:
	static std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> primitiveBatch;
	static std::unique_ptr<DirectX::BasicEffect> basicEffect;
	static ID3D11DeviceContext* deviceContext;
};
