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

void DirectXClass::UpdateScreenSize()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = deviceContext->Map(screenSizeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (SUCCEEDED(hr))
	{
		ScreenSizeBuffer* dataPtr = (ScreenSizeBuffer*)mappedResource.pData;
		dataPtr->width = static_cast<float>(Screen::GetWidth());
		dataPtr->height = static_cast<float>(Screen::GetHeight());
		deviceContext->Unmap(screenSizeBuffer, 0);
	}
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

bool DirectXClass::CreateShaders()
{
	// Basic vertex shader
	const char* vsCode = R"(
	    cbuffer ScreenSizeBuffer : register(b0)
	    {
	        float screenWidth;
	        float screenHeight;
	        float2 padding;
	    };

	    struct VertexInput {
	        float3 position : POSITION;
	        float4 color : COLOR;
	    };
	    
	    struct PixelInput {
	        float4 position : SV_POSITION;
	        float4 color : COLOR;
	    };
	    
	    PixelInput main(VertexInput input) {
	        PixelInput output;
	        
	        // Convert from screen coordinates to NDC using screen size from constant buffer
	        float2 screenPos = input.position.xy;
	        float2 ndcPos;
	        ndcPos.x = (screenPos.x / screenWidth) * 2.0f - 1.0f;
	        ndcPos.y = -(screenPos.y / screenHeight) * 2.0f + 1.0f;
	        
	        output.position = float4(ndcPos, 0.0f, 1.0f);
	        output.color = input.color;
	        return output;
	    }
	)";

    // Basic pixel shader
    const char* psCode = R"(
        struct PixelInput {
            float4 position : SV_POSITION;
            float4 color : COLOR;
        };
        
        float4 main(PixelInput input) : SV_TARGET {
            return input.color;
        }
    )";

    // Compile and create vertex shader
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3DCompile(vsCode, strlen(vsCode), nullptr, nullptr, nullptr, "main", "vs_4_0", 0, 0, &vsBlob, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        return false;
    }

    hr = D3DDevice->CreateVertexShader(vsBlob->GetBufferPointer(), 
                                     vsBlob->GetBufferSize(), nullptr, &vertexShader);
    if (FAILED(hr)) {
        vsBlob->Release();
        return false;
    }

    // Create input layout
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, 
          D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, 
          D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    hr = D3DDevice->CreateInputLayout(layout, 2, vsBlob->GetBufferPointer(),
                                    vsBlob->GetBufferSize(), &inputLayout);
    vsBlob->Release();
    if (FAILED(hr))
        return false;

    // Compile and create pixel shader
    ID3DBlob* psBlob = nullptr;
    hr = D3DCompile(psCode, strlen(psCode), nullptr, nullptr, nullptr, "main", "ps_4_0", 0, 0, &psBlob, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        return false;
    }

    hr = D3DDevice->CreatePixelShader(psBlob->GetBufferPointer(), 
                                    psBlob->GetBufferSize(), nullptr, &pixelShader);
    psBlob->Release();
    if (FAILED(hr))
        return false;

    return true;
}

bool DirectXClass::CreateScreenSizeBuffer()
{
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(ScreenSizeBuffer);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	HRESULT hr = D3DDevice->CreateBuffer(&bufferDesc, nullptr, &screenSizeBuffer);
	return SUCCEEDED(hr);
}

bool DirectXClass::CreateVertexBuffer(UINT bufferSize)
{
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.ByteWidth = bufferSize;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hr = D3DDevice->CreateBuffer(&bufferDesc, nullptr, &vertexBuffer);
    return !FAILED(hr);
}

bool DirectXClass::UpdateVertexBuffer(void* data, UINT dataSize)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = deviceContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 
                                  0, &mappedResource);
    if (FAILED(hr))
        return false;

    memcpy(mappedResource.pData, data, dataSize);
    deviceContext->Unmap(vertexBuffer, 0);
    return true;
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

	// Create shaders and vertex buffer
	if (!CreateShaders())
		return false;

	if (!CreateScreenSizeBuffer())
		return false;

	// Create a vertex buffer big enough for 1000 vertices initially
	if (!CreateVertexBuffer(1000 * sizeof(DirectX::VertexPositionColor)))
		return false;

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
	clientWidth = Screen::GetWidth();
	clientHeight = Screen::GetHeight();

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
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(inputLayout);
	SAFE_RELEASE(vertexShader);
	SAFE_RELEASE(pixelShader);
	SAFE_RELEASE(screenSizeBuffer);
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