#ifndef H_FONT
#define H_FONT

#include <string>
#include "../../Engine/SageMath.h"
#include "SpriteFont.h"

using namespace std;

#define FC_BLACK  DirectX::Colors::Black
#define FC_RED    DirectX::Colors::Red
#define FC_GREEN  DirectX::Colors::Green
#define FC_BLUE   DirectX::Colors::Blue

using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Color;

class FontType
{
public:
	void PrintMessage(const int posX, const int posY, const wstring& message, DirectX::FXMVECTOR colour) { PrintMessage(posX, posY, message.c_str(), colour); }
	void PrintMessage(int posX, int posY, const wchar_t* message, DirectX::FXMVECTOR colour);
	void InitializeFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& fileName);

	FontType();
	FontType(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& fileName);
	~FontType();

	Vector2 MeasureString(const wchar_t* message) const;

private:
	DirectX::SpriteBatch* pBatch;
	DirectX::SpriteFont* pFont;
};

#endif