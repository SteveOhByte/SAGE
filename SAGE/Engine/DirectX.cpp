#include <string>
#include <sstream>
#include "DirectX.h"

using namespace std;

LRESULT CALLBACK WndProc(const HWND hWnd, const UINT msg, const WPARAM wParam, const LPARAM lParam)
{
	static DirectXClass* application = nullptr;

	switch (msg)
	{
		case WM_CREATE:
			const CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
			application = static_cast<DirectXClass*>(cs->lpCreateParams);
			return 0;
	}

	if (application)
		return application->ProcessWindowMessages(msg, wParam, lParam);

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT DirectXClass::ProcessWindowMessages(const UINT msg, const WPARAM wParam, const LPARAM lParam)
{
	switch (msg)
	{
		case WM_SIZE:
			if (D3DDevice)
			{
				if (!ResizeDirectXComponents(LOWORD(lParam), HIWORD(lParam))) PostQuitMessage(0);
				return 0;
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}

	return DefWindowProc(mainWnd, msg, wParam, lParam);
}

bool DirectXClass::ResizeDirectXComponents(const int width, const int height)
{
	clientWidth = width;
	clientHeight = height;

	renderTargetView->Release();
	if (depthStencilView)
		depthStencilView->Release();
	if (depthStencilBuffer)
		depthStencilBuffer->Release();

	if (FAILED(swapChain->ResizeBuffers(1, clientWidth, clientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0)))
		ErrorMessage(L"Failed to resize swapchain");

	if (!InitializeRenderTarget())
		return false;

	if (depthStencilUsed)
		CreateDepthStencilBuffer();

	InitializeViewPorts();

	return true;
}

bool DirectXClass::SetRasterState(const D3D11_RASTERIZER_DESC& rasterizerState)
{
	if (rasterState)
		rasterState->Release();

	if (FAILED(D3DDevice->CreateRasterizerState( &rasterizerState, &rasterState)))
		return ErrorMessage(L"Raster State not created");

	deviceContext->RSSetState(rasterState);
	return true;
}

void DirectXClass::RenderScene()
{
	timer.CheckTime();
	Tick(static_cast<float>(timer.GetTimeDeltaTime()));
	deviceContext->ClearRenderTargetView(renderTargetView, clearColour);
	if (depthStencilUsed)
	{
		deviceContext->OMSetDepthStencilState(nullptr, 0);
		deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	Render();

	if (displayFps)
		DisplayFramesPerSecond(5, 5);

	swapChain->Present(presentInterval, 0);
}

int DirectXClass::MessageLoop()
{
	MSG msg = {nullptr};

	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break;
		}
		RenderScene();
	}
	return 0;
}

bool DirectXClass::InitializeDirect3D()
{
	if (!InitializeSwapChain())
		return false;

	if (!InitializeRenderTarget())
		return false;

	if (depthStencilUsed)
	{
		if (!CreateDepthStencilBuffer())
			return false;
	}

	InitializeViewPorts();
	font.InitializeFont(D3DDevice, deviceContext, L"..\\Font\\Arial16.spritefont");

	return true;
}

bool DirectXClass::InitializeSwapChain()
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = this->mainWnd;
	swapChainDesc.Windowed = FALSE;

	const HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &D3DDevice, nullptr, &deviceContext);
	if (FAILED(hr))
	{
		return ErrorMessage(L"Could not create swap chain");
	}

	return true;
}

bool DirectXClass::InitializeRenderTarget()
{
	this->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&backBuffer));
	if (FAILED(D3DDevice->CreateRenderTargetView(backBuffer, NULL, &this->renderTargetView)))
		return ErrorMessage(L"Could not create Render Target");

	deviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr);

	return true;
}

bool DirectXClass::CreateDepthStencilBuffer()
{
	D3D11_TEXTURE2D_DESC descDepth;
	D3D11_DEPTH_STENCIL_VIEW_DESC descDepthStencilView;

	descDepth.Width = clientWidth;
	descDepth.Height = clientHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	if (FAILED(D3DDevice->CreateTexture2D(&descDepth, NULL, &depthStencilBuffer)))
		return ErrorMessage(L"Depth Stencil texture not created");

	descDepthStencilView.Format = descDepth.Format;
	descDepthStencilView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDepthStencilView.Texture2D.MipSlice = 0;

	if (FAILED(D3DDevice->CreateDepthStencilView(depthStencilBuffer, &descDepthStencilView, &depthStencilView)))
		return ErrorMessage(L"Depth Stencil view not created");

	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
	return true;
}

void DirectXClass::InitializeViewPorts() const
{
	D3D11_VIEWPORT viewport;

	// Create the Viewport 
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<FLOAT>(clientWidth);
	viewport.Height = static_cast<FLOAT>(clientHeight);
	deviceContext->RSSetViewports(1, &viewport);
}

DirectXClass::DirectXClass(const HINSTANCE hInstance)
{
	// Win32 variables
	instance = hInstance;
	mainWnd = nullptr;
	clientWidth = 1920;
	clientHeight = 1080;

	// Direct 3D variables
	D3DDevice = nullptr;
	swapChain = nullptr;
	renderTargetView = nullptr;
	depthStencilView = nullptr;
	depthStencilBuffer = nullptr;
	rasterState = nullptr;

	depthStencilUsed = false;
	displayFps = false;
	presentInterval = 1;
	clearColour = Color(0.0f, 0.0f, 0.3f, 1.0f);

	deviceContext = nullptr;
	backBuffer = nullptr;
}

void DirectXClass::DisplayFramesPerSecond(const int xPos, const int yPos)
{
	wostringstream message;
	wstring messageOut;

	message << L"FPS....." << timer.GetFps();
	messageOut = message.str();

	font.PrintMessage(xPos, yPos, messageOut.c_str(), FC_GREEN);
}

DirectXClass::~DirectXClass()
{
	if (renderTargetView)
		renderTargetView->Release();
	if (swapChain)
		swapChain->Release();
	if (deviceContext)
		deviceContext->Release();
	if (D3DDevice)
		D3DDevice->Release();
	if (rasterState)
		rasterState->Release();
	if (depthStencilView)
		depthStencilView->Release();
}

bool DirectXClass::ErrorMessage(const LPCWSTR errorMsg) const
{
	MessageBox(mainWnd, errorMsg, L"Fatal Error!", MB_ICONERROR);
	return false;
}

bool DirectXClass::InitWindowsApp(const wstring& windowTitle, const int nShowCmd)
{
	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = this->instance;
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = L"BasicWndClass";

	if (!RegisterClass(&wc))
	{
		MessageBox(nullptr, L"RegisterClass FAILED", nullptr, 0);
		return false;
	}

	RECT desktopRect;
	GetClientRect(GetDesktopWindow(), &desktopRect);
	clientWidth = desktopRect.right;
	clientHeight = desktopRect.bottom;

	mainWnd = CreateWindow(
		L"BasicWndClass",
		windowTitle.c_str(),
		WS_POPUP,
		0,
		0,
		clientWidth,
		clientHeight,
		nullptr,
		nullptr,
		this->instance,
		this);

	if (mainWnd == nullptr)
	{
		MessageBox(nullptr, L"CreateWindow FAILED", nullptr, 0);
		return false;
	}

	ShowWindow(mainWnd, nShowCmd);
	UpdateWindow(mainWnd);

	return true;
}