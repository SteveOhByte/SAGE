#include "TextureType.h"
#include "DirectX.h"
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

// Constructor
TextureType::TextureType()
{
	desc = D3D11_TEXTURE2D_DESC();
	pView = nullptr;
	pTexture = nullptr;
}

bool TextureType::Load(ID3D11Device* device, const wchar_t* fileName)
{
	// If we're already loaded, unload the previous
	if (pTexture != nullptr)
		Unload();

	filePath = fileName;
	HRESULT result;

	// Check if it's a dds file or not
	if (filePath.find(L".dds") != std::wstring::npos)
		result = DirectX::CreateDDSTextureFromFile(device, fileName, reinterpret_cast<ID3D11Resource**>(&pTexture), &pView);
	else
		result = DirectX::CreateWICTextureFromFile(device, fileName, reinterpret_cast<ID3D11Resource**>(&pTexture), &pView);

	// Check if we loaded the file
	if (result != S_OK)
		return false;

	// Get the information about the texture
	pTexture->GetDesc(&desc);

	return true;
}

void TextureType::Draw(ID3D11DeviceContext* device, ID3D11Texture2D* drawTo, const int destX, const int destY) const
{
	// If we aren't loaded
	if (pTexture == nullptr) return;

	// Get the width and height of what we are drawing to
	D3D11_TEXTURE2D_DESC toDesc;
	drawTo->GetDesc(&toDesc);

	// Width of the surface we are drawing to
	const int toWidth = static_cast<int>(toDesc.Width);
	const int toHeight = static_cast<int>(toDesc.Height);

	// If we are offscreen, don't draw
	if (destX >= toWidth || destY >= toHeight || destX <= -toWidth || destY <= -toHeight)
		return;

	int width = desc.Width;
	int height = desc.Height;
	int left = 0;
	int top = 0;

	// If we are offscreen, reduce our width
	if (destX < 0)
	{
		left = -destX;
		width = width + destX;
	}
	if (destY < 0)
	{
		top = -destY;
		height = height + destY;
	}

	// Check if we are off the edge of the right screen
	if (destX + width > static_cast<int>(toDesc.Width))
	{
		width = static_cast<int>(toDesc.Width) - destX;
	}

	if (destY + height > static_cast<int>(toDesc.Height))
	{
		height = static_cast<int>(toDesc.Height) - destY;
	}
	
	// Describe the sub area we want to draw to
	D3D11_BOX sourceRegion;
	sourceRegion.left = left;
	sourceRegion.right = width;
	sourceRegion.top = top;
	sourceRegion.bottom = height;
	sourceRegion.front = 0;
	sourceRegion.back = 1;

	// Copy part of a texture resource to the back buffer, or wherever we are drawing to
	device->CopySubresourceRegion(drawTo, 0, destX, destY, 0, pTexture, 0, &sourceRegion);
}

void TextureType::Unload()
{
	SAFE_RELEASE(pTexture);
	SAFE_RELEASE(pView);
}