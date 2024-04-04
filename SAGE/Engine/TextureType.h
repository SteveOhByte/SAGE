#ifndef _TEXTURE_TYPE_H
#define TEXTURE_TYPE_H

#include <string>
#include <d3d11_1.h>

class TextureType
{
public:
	// Constructors
	TextureType();
	~TextureType() { Unload(); }

	// Loads the texture from disk
	bool Load(ID3D11Device* device, const wchar_t* fileName);
	void Unload();

	// Draws the texture to another 'resource'. Typically, drawTo will be the back buffer
	void Draw(ID3D11DeviceContext* device, ID3D11Texture2D* drawTo, int destX, int destY) const;

	// Get height & width of the texture
	int GetHeight() const { return desc.Height; }
	int GetWidth() const { return desc.Width; }

	// Get the resource view
	ID3D11ShaderResourceView* GetResourceView() const { return pView; }

private:
	ID3D11Texture2D* pTexture;
	ID3D11ShaderResourceView* pView;

	std::wstring filePath;

	D3D11_TEXTURE2D_DESC desc;
};

#endif