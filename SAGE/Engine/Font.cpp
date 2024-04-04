#include <string>
#include "Font.h"

using namespace std;

// ReSharper disable once CppMemberFunctionMayBeConst
void FontType::PrintMessage(const int posX, const int posY, const wchar_t* message, DirectX::FXMVECTOR colour)
{
	DirectX::XMFLOAT2 pos;
	pos.x = static_cast<float>(posX);
	pos.y = static_cast<float>(posY);

	pBatch->Begin();
	pFont->DrawString(pBatch, message, pos, colour);
	pBatch->End();
}

void FontType::InitializeFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& fileName)
{
	pBatch = new DirectX::SpriteBatch(pContext);
	pFont = new DirectX::SpriteFont(pDevice, fileName.c_str());
}

Vector2 FontType::MeasureString(const wchar_t* message) const
{
	return Vector2(pFont->MeasureString(message));
}

FontType::FontType(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& fileName)
{
	InitializeFont(pDevice, pContext, fileName);
}

FontType::FontType()
{
	pBatch = nullptr;
	pFont = nullptr;
}

FontType::~FontType()
{
	delete pBatch;
	delete pFont;
}