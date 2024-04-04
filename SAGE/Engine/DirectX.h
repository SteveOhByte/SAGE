#ifndef DIRECTX_H
#define DIRECTX_H
#include <windows.h>
#include "SageMath.h"
#include "Timer.h"
#include "Font.h"

using namespace std;

#ifndef H_DirectX
#define H_DIRECT_X
// Safe release macro, for releasing directX objects
#ifndef SAFE_RELEASE
#define SAFE_RELEASE( x ) if ( (x) != NULL ) { (x)->Release(); (x) = NULL; }
#endif

class DirectXClass
{
	bool depthStencilUsed;
	bool displayFps;

protected:
	// Win32 variables
	HINSTANCE instance;
	HWND mainWnd;
	int clientWidth;
	int clientHeight;

	// Direct 3D variables
	ID3D11Device* D3DDevice;
	ID3D11DeviceContext* deviceContext;
	IDXGISwapChain* swapChain;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11Texture2D* backBuffer;
	ID3D11RasterizerState* rasterState;
	TimerType timer;
	FontType font;
	int presentInterval;
	Color clearColour;

	bool InitializeSwapChain();
	bool InitializeRenderTarget();
	void InitializeViewPorts() const;
	void RenderScene();
	virtual void Render() {};
	virtual void Tick(float deltaTime) {};

public:
	// Win32 methods
	bool InitWindowsApp(const wstring& windowTitle, int nShowCmd);
	bool ErrorMessage(LPCWSTR errorMsg) const;
	int MessageLoop();

	void SetHandleInstance(const HINSTANCE hInstance) { instance = hInstance; }
	void SetClientWidth(const int width) { clientWidth = width; }
	void SetClientHeight(const int height) { clientHeight = height; }
	HWND GetMainWnd() const { return mainWnd; }

	// Direct 3D methods
	bool InitializeDirect3D();
	virtual LRESULT ProcessWindowMessages(UINT msg, WPARAM wParam, LPARAM lParam);
	bool ResizeDirectXComponents(int width, int height);
	bool SetRasterState(const D3D11_RASTERIZER_DESC& rasterizerState);
	bool CreateDepthStencilBuffer();

	void SetDepthStencil(const bool value) { depthStencilUsed = value; }
	ID3D11Device* GetD3DDevice() const { return D3DDevice; }

	void DisplayFramesPerSecond(int xPos, int yPos);
	void DisplayFps(const bool value) { displayFps = value; }

	explicit DirectXClass(HINSTANCE hInstance);
	DirectXClass() = default;

protected:
	~DirectXClass();
};

#endif
#endif