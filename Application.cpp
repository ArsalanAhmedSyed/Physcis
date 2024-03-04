#include "Application.h"
#include <iostream>
#include <memory>

#define NUMBEROFCUBES 2
#define FPS60 1.0/60.0f

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

bool Application::HandleKeyboard(MSG msg)
{
	switch (msg.wParam)
	{
	case VK_UP:
		_cameraOrbitRadius = max(_cameraOrbitRadiusMin, _cameraOrbitRadius - (_cameraSpeed * 2.f));
		return true;
		break;

	case VK_DOWN:
		_cameraOrbitRadius = min(_cameraOrbitRadiusMax, _cameraOrbitRadius + (_cameraSpeed * 2.f));
		return true;
		break;

	case VK_RIGHT:
		_cameraOrbitAngleXZ -= _cameraSpeed;
		return true;
		break;

	case VK_LEFT:
		_cameraOrbitAngleXZ += _cameraSpeed;
		return true;
		break;
	}

	return false;
}

Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
	_pVertexBuffer = nullptr;
	_pIndexBuffer = nullptr;
	_pConstantBuffer = nullptr;
	CCWcullMode=nullptr;
	CWcullMode= nullptr;
	DSLessEqual = nullptr;
	RSCullNone = nullptr;
	 _WindowHeight = 0;
	 _WindowWidth = 0;

	 _timer = nullptr;
}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{

	HRESULT hr = 0;

    if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
        return E_FAIL;
	}

    RECT rc;
	if (!GetClientRect(_hWnd, &rc)) { return E_FAIL; }
    _WindowWidth = rc.right - rc.left;
    _WindowHeight = rc.bottom - rc.top;

    if (FAILED(InitDevice()))
    {
        Cleanup();

        return E_FAIL;
    }

	
	hr = CreateDDSTextureFromFile(_pd3dDevice, L"Assets\\Textures\\stone.dds", nullptr, &_pTextureRV);
	hr = CreateDDSTextureFromFile(_pd3dDevice, L"Assets\\Textures\\floor.dds", nullptr, &_pGroundTextureRV);
	
	if (FAILED(hr))
	{
		return E_FAIL;
	}

    // Setup Camera
	XMFLOAT3 eye = XMFLOAT3(0.0f, 2.0f, -1.0f);
	XMFLOAT3 at = XMFLOAT3(0.0f, 2.0f, 0.0f);
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);

	_camera = new Camera(eye, at, up, (float)_renderWidth, (float)_renderHeight, 0.01f, 200.0f);

	// Setup the scene's light
	basicLight.AmbientLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	basicLight.DiffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	basicLight.SpecularLight = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	basicLight.SpecularPower = 20.0f;
	basicLight.LightVecW = XMFLOAT3(0.0f, 1.0f, -1.0f);

	std::shared_ptr<Geometry> donutGeometry = std::make_shared<Geometry>();
	objMeshData = OBJLoader::Load("Assets/3DModels/donut.obj", _pd3dDevice);
	donutGeometry->indexBuffer = objMeshData.IndexBuffer;
	donutGeometry->numberOfIndices = objMeshData.IndexCount;
	donutGeometry->vertexBuffer = objMeshData.VertexBuffer;
	donutGeometry->vertexBufferOffset = objMeshData.VBOffset;
	donutGeometry->vertexBufferStride = objMeshData.VBStride;
	
	std::shared_ptr<Geometry> cubeGeometry = std::make_shared<Geometry>();
	cubeGeometry->indexBuffer = _pIndexBuffer;
	cubeGeometry->vertexBuffer = _pVertexBuffer;
	cubeGeometry->numberOfIndices = 36;
	cubeGeometry->vertexBufferOffset = 0;
	cubeGeometry->vertexBufferStride = sizeof(SimpleVertex);

	std::shared_ptr<Geometry> planeGeometry = std::make_shared<Geometry>();
	planeGeometry->indexBuffer = _pPlaneIndexBuffer;
	planeGeometry->vertexBuffer = _pPlaneVertexBuffer;
	planeGeometry->numberOfIndices = 6;
	planeGeometry->vertexBufferOffset = 0;
	planeGeometry->vertexBufferStride = sizeof(SimpleVertex);

	std::shared_ptr<Material> shinyMaterial = std::make_shared <Material>();
	shinyMaterial->ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	shinyMaterial->diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	shinyMaterial->specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	shinyMaterial->specularPower = 10.0f;

	std::shared_ptr<Material> noSpecMaterial = std::make_shared <Material>();
	noSpecMaterial->ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	noSpecMaterial->diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	noSpecMaterial->specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	noSpecMaterial->specularPower = 0.0f;
	
	GameObject* gameObject = new GameObject("Floor", planeGeometry, noSpecMaterial);
	gameObject->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
	gameObject->GetTransform()->SetScale(15.0f, 15.0f, 15.0f);
	gameObject->GetTransform()->SetRotation(90.0f, 0.0f, 0.0f);
	gameObject->SetPhysicsModel(new RigidBodyModel(gameObject->GetTransform()));
	gameObject->GetPhyicsModel()->SetCollider(new PlaneCollider(gameObject->GetTransform(), Vector3(0,1,0)));

	gameObject->GetPhyicsModel()->SetMass(0.0f);
	gameObject->GetPhyicsModel()->GetCollider()->SetPlainHalfExtend();
	gameObject->GetApperance()->SetTextureRV(_pGroundTextureRV);

	_gameObjects.push_back(gameObject);

	for (auto i = 0; i < NUMBEROFCUBES; i++)
	{
		gameObject = new GameObject("Cube " + to_string(i), cubeGeometry, noSpecMaterial);
		gameObject->GetTransform()->SetScale(1.0f, 1.0f, 1.0f);
		gameObject->GetTransform()->SetPosition(3.0f + (i * 2.5f), 1.0f, 10.0f);
		gameObject->GetApperance()->SetTextureRV(_pTextureRV);
		gameObject->SetPhysicsModel(new RigidBodyModel(gameObject->GetTransform()));
		gameObject->GetPhyicsModel()->SetCollider(new BoxCollider(gameObject->GetTransform(), gameObject->GetPhyicsModel()));
		_gameObjects.push_back(gameObject);
	}

	//Initialise the particle spawner
	_emitter = new Emitter(Vector3(-3.f, 1.f, 5.0f), 20, donutGeometry, shinyMaterial, _pTextureRV);

	//Enable cube gravity
	_gameObjects[_cubeA]->GetPhyicsModel()->SetGravity(true);
	_gameObjects[_cubeB]->GetPhyicsModel()->SetGravity(true);

	_timer = new Timer();

	return S_OK;
}

HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.hlsl", "VS", "vs_4_0", &pVSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The HLSL file cannot be compiled.  Error output to Visual Studio Console.", L"Error", MB_OK);
        return hr;
    }

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	if (FAILED(hr))
	{	
		pVSBlob->Release();
        return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.hlsl", "PS", "ps_4_0", &pPSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The HLSL file cannot be compiled.  Error output to Visual Studio Console.", L"Error", MB_OK);
        return hr;
    }

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();

    if (FAILED(hr))
        return hr;
	
    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
        return hr;

    // Set the input layout
    _pImmediateContext->IASetInputLayout(_pVertexLayout);

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = _pd3dDevice->CreateSamplerState(&sampDesc, &_pSamplerLinear);

	return hr;
}

HRESULT Application::InitVertexBuffer()
{
	HRESULT hr;

    // Create vertex buffer
    SimpleVertex vertices[] =
    {
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },

		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
    };

    D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * 24;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices;

    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pVertexBuffer);

    if (FAILED(hr))
        return hr;

	// Create vertex buffer
	SimpleVertex planeVertices[] =
	{
		{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 5.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(5.0f, 5.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(5.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
	};

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = planeVertices;

	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pPlaneVertexBuffer);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitIndexBuffer()
{
	HRESULT hr;

    // Create index buffer
    WORD indices[] =
    {
		3, 1, 0,
		2, 1, 3,

		6, 4, 5,
		7, 4, 6,

		11, 9, 8,
		10, 9, 11,

		14, 12, 13,
		15, 12, 14,

		19, 17, 16,
		18, 17, 19,

		22, 20, 21,
		23, 20, 22
    };

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 36;     
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = indices;
    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pIndexBuffer);

    if (FAILED(hr))
        return hr;

	// Create plane index buffer
	WORD planeIndices[] =
	{
		0, 3, 1,
		3, 2, 1,
	};

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 6;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = planeIndices;
	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pPlaneIndexBuffer);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    _hInst = hInstance;
    RECT rc = {0, 0, 960, 540};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"FGGC Semester 2 Framework", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        if (pErrorBlob) pErrorBlob->Release();

        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT Application::InitDevice()
{
    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;

#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	UINT sampleCount = 4;

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = _renderWidth;
    sd.BufferDesc.Height = _renderHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _hWnd;
	sd.SampleDesc.Count = sampleCount;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
        return hr;

    hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
    pBackBuffer->Release();

    if (FAILED(hr))
        return hr;

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)_renderWidth;
    vp.Height = (FLOAT)_renderHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pImmediateContext->RSSetViewports(1, &vp);

	InitShadersAndInputLayout();

	InitVertexBuffer();
	InitIndexBuffer();

    // Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
    hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);

    if (FAILED(hr))
        return hr;

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = _renderWidth;
	depthStencilDesc.Height = _renderHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = sampleCount;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	_pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);
	_pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

	_pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);

	// Rasterizer
	D3D11_RASTERIZER_DESC cmdesc;

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_NONE;
	hr = _pd3dDevice->CreateRasterizerState(&cmdesc, &RSCullNone);

	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	_pd3dDevice->CreateDepthStencilState(&dssDesc, &DSLessEqual);

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));

	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;

	cmdesc.FrontCounterClockwise = true;
	hr = _pd3dDevice->CreateRasterizerState(&cmdesc, &CCWcullMode);

	cmdesc.FrontCounterClockwise = false;
	hr = _pd3dDevice->CreateRasterizerState(&cmdesc, &CWcullMode);

    return S_OK;
}

void Application::Cleanup()
{
    if (_pImmediateContext) _pImmediateContext->ClearState();
	if (_pSamplerLinear) _pSamplerLinear->Release();

	if (_pTextureRV) _pTextureRV->Release();

	if (_pGroundTextureRV) _pGroundTextureRV->Release();

    if (_pConstantBuffer) _pConstantBuffer->Release();

    if (_pVertexBuffer) _pVertexBuffer->Release();
    if (_pIndexBuffer) _pIndexBuffer->Release();
	if (_pPlaneVertexBuffer) _pPlaneVertexBuffer->Release();
	if (_pPlaneIndexBuffer) _pPlaneIndexBuffer->Release();

    if (_pVertexLayout) _pVertexLayout->Release();
    if (_pVertexShader) _pVertexShader->Release();
    if (_pPixelShader) _pPixelShader->Release();
    if (_pRenderTargetView) _pRenderTargetView->Release();
    if (_pSwapChain) _pSwapChain->Release();
    if (_pImmediateContext) _pImmediateContext->Release();
    if (_pd3dDevice) _pd3dDevice->Release();
	if (_depthStencilView) _depthStencilView->Release();
	if (_depthStencilBuffer) _depthStencilBuffer->Release();

	if (DSLessEqual) DSLessEqual->Release();
	if (RSCullNone) RSCullNone->Release();

	if (CCWcullMode) CCWcullMode->Release();
	if (CWcullMode) CWcullMode->Release();

	if (_camera)
	{
		delete _camera;
	}

	for (auto gameObject : _gameObjects)
	{
		if (gameObject)
		{
			delete gameObject;
		}
	}

	//TEMP
	if (_timer) _timer = nullptr;
}

void Application::moveForward(int objectNumber)
{
	_gameObjects[objectNumber]->GetPhyicsModel()->AddForce(Vector3(0, 0, -1.f));
}

void Application::moveBackward(int objectNumber)
{
	_gameObjects[objectNumber]->GetPhyicsModel()->AddForce(Vector3(0, 0, 1.f));
}

void Application::moveUp(int objectNumber)
{
	_gameObjects[objectNumber]->GetPhyicsModel()->AddForce(Vector3(0, 20, 0.f));
}

void Application::moveDown(int objectNumber)
{
	_gameObjects[objectNumber]->GetPhyicsModel()->AddForce(Vector3(0, -0.5f, 0.f));
}
void Application::moveLeft(int objectNumber)
{
	_gameObjects[objectNumber]->GetPhyicsModel()->AddForce(Vector3(-0.5f, 0, 0.f));
}
void Application::moveRight(int objectNumber)
{
	_gameObjects[objectNumber]->GetPhyicsModel()->AddForce(Vector3(0.5f, 0, 0.f));
}

void Application::Update()
{
	float deltaTime = _timer->GetDeltaTime();
	accumulator += deltaTime;
	while (accumulator >= FPS60)
	{
		accumulator -= FPS60;

		for (auto gameObject : _gameObjects)
		{
			gameObject->Update(FPS60);
		}

		_emitter->Update(deltaTime);

		#pragma region  Movement Keyboard Input
		//R for backwards
		if (GetAsyncKeyState(0x52))
		{
			moveBackward(_controllingObject);
		}
		//E for forward
		if(GetAsyncKeyState(0x45))
		{
			moveForward(_controllingObject);
		}
		//W for up
		if (GetAsyncKeyState(0x57))
		{
			moveUp(_controllingObject);
		}
		//S for down
		if (GetAsyncKeyState(0x53))
		{
			moveDown(_controllingObject);
		}
		//D for right
		if (GetAsyncKeyState(0x44))
		{
			moveRight(_controllingObject);
		}
		//A for left
		if (GetAsyncKeyState(0x41))
		{
			moveLeft(_controllingObject);
		}
		#pragma endregion //Movement Keyboard Input

		#pragma region Other Keyboard Input
		if (GetAsyncKeyState('1') & 0x0001)
		{
			//Swap Controlling cube
			_controllingObject = (_controllingObject == 1) ? 2 : 1;
		}
		if (GetAsyncKeyState('2') & 0x0001)
		{
			//Set box 1 collision to Sphere collider
			_gameObjects[1]->GetPhyicsModel()->SetCollider(new SphereCollider(_gameObjects[1]->GetTransform(), _gameObjects[1]->GetPhyicsModel()));
		}
		if (GetAsyncKeyState('3') & 0x0001)
		{
			//Set box 1 collision to box collider
			_gameObjects[1]->GetPhyicsModel()->SetCollider(new BoxCollider(_gameObjects[1]->GetTransform(), _gameObjects[1]->GetPhyicsModel()));
		}
		if (GetAsyncKeyState('4') & 0x0001)
		{
			//Set box 2 collider to sphere Collider
			_gameObjects[2]->GetPhyicsModel()->SetCollider(new SphereCollider(_gameObjects[2]->GetTransform(), _gameObjects[2]->GetPhyicsModel()));
		}
		if (GetAsyncKeyState('5') & 0x0001)
		{
			//set box 2 collider to Box collider
			_gameObjects[2]->GetPhyicsModel()->SetCollider(new BoxCollider(_gameObjects[2]->GetTransform(), _gameObjects[2]->GetPhyicsModel()));
		}
		#pragma endregion //Other Keyboard Input

		#pragma region Camera Update
		float angleAroundZ = _cameraOrbitAngleXZ;

		float x = _cameraOrbitRadius * cos(angleAroundZ);
		float z = _cameraOrbitRadius * sin(angleAroundZ);

		XMFLOAT3 cameraPos = _camera->GetPosition();
		cameraPos.x = x * 1.5;
		cameraPos.z = z * 1.5;

		_camera->SetPosition(cameraPos);
		_camera->Update();
		#pragma endregion //Camera Update

		#pragma region Collision Check 
		_cubeA = 1;
		_cubeB = 2;
		_plane = 0;

		//Collision check between the two boxes. (collision check can be set as sphere and not just box collision)
		CollisionManifold manifold = CollisionManifold();
		if (_gameObjects[_cubeB]->GetPhyicsModel()->isCollideable() && _gameObjects[_cubeA]->GetPhyicsModel()->isCollideable() &&
			_gameObjects[_cubeB]->GetPhyicsModel()->GetCollider()->CollidesWith(*_gameObjects[_cubeA]->GetPhyicsModel()->GetCollider(), manifold))
		{
			ResolveCollisions(manifold, _cubeB, _cubeA);
		}

		//All game object collision check with the plane (this collision also works with sphere and box colliders)
		for (size_t i = 1; i < _gameObjects.size(); i++)
		{
			CollisionManifold manifold1 = CollisionManifold();
			if (_gameObjects[i]->GetPhyicsModel()->isCollideable() && _gameObjects[_plane]->GetPhyicsModel()->isCollideable() &&
				_gameObjects[i]->GetPhyicsModel()->GetCollider()->CollidesWith(*_gameObjects[_plane]->GetPhyicsModel()->GetCollider(), manifold1))
			{
				ResolvePlaneCollisions(manifold1, i, _plane);
			}
		}
		#pragma endregion //Collision Check 
	}

	_timer->Tick();
}

void Application::ResolvePlaneCollisions(CollisionManifold& manifold, int objectOne, int objectTwo)
{
	Transform* objectATransform = _gameObjects[objectOne]->GetTransform();
	Transform* objectBTransform = _gameObjects[objectTwo]->GetTransform();

	PhysicsModel* objectA = _gameObjects[objectOne]->GetPhyicsModel();
	PhysicsModel* objectB = _gameObjects[objectTwo]->GetPhyicsModel();

	//Get difference in position
	Vector3 collisionNormal = manifold.collisionNormal;
	//Get Difference in velocity;
	Vector3 relativeVelocity = objectB->GetVelocity() - objectA->GetVelocity();

	//Stop if the dot product value is higher then one
		// It would mean that the objects are not colliding anymore.
	if (collisionNormal * relativeVelocity < 0.0f)
	{
		//Amount to control the collision force applied (how munch bounce to applie)
		float restitution = 0.7f;

		//velocity after collision
		float vj = -(1 + restitution) * collisionNormal * relativeVelocity;
		float j = vj * ((objectA->GetInverseMass()) + (objectB->GetInverseMass()));

		//Calculate Impulse to the objects
		Vector3 object1Impulse = -(objectA->GetInverseMass() * j * collisionNormal);
		Vector3 object2Impulse = (objectB->GetInverseMass() * j * collisionNormal);

		Vector3 pentrationResolve1 = -(collisionNormal * manifold.points[0].penetrationDepth * objectA->GetInverseMass());
		Vector3 pentrationResolve2 = (collisionNormal * manifold.points[0].penetrationDepth * objectB->GetInverseMass());

		objectBTransform->SetPosition(objectBTransform->GetPosition() + pentrationResolve2);
		objectATransform->SetPosition(objectATransform->GetPosition() + pentrationResolve1);

		//Apply Impulse to the object
		objectA->ApplyImpulse(object1Impulse);
		objectB->ApplyImpulse(object2Impulse);

	}		

	manifold = CollisionManifold();
}

void Application::ResolveCollisions(CollisionManifold& manifold, int objectOne, int objectTwo)
{
	Transform* objectATransform = _gameObjects[objectOne]->GetTransform();
	Transform* objectBTransform = _gameObjects[objectTwo]->GetTransform();

	PhysicsModel* objectA = _gameObjects[objectOne]->GetPhyicsModel();
	PhysicsModel* objectB = _gameObjects[objectTwo]->GetPhyicsModel();

	//Get difference in position
	Vector3 collisionNormal = manifold.collisionNormal;
	//Get Difference in velocity;
	Vector3 relativeVelocity = objectB->GetVelocity() - objectA->GetVelocity();

	//Stop if the dot product value is higher then one
		// It would mean that the objects are not colliding anymore.
	if (collisionNormal * relativeVelocity < 0.0f)
	{
		//Amount to control the collision force applied (how munch bounce to applie)
		float restitution = 0.7f;

		//velocity after collision
		float vj = -(1 + restitution) * collisionNormal * relativeVelocity;
		float j = vj * ((objectA->GetInverseMass()) + (objectB->GetInverseMass()));

		//Calculate Impulse to the objects
		Vector3 object1Impulse = -(objectA->GetInverseMass() * j * collisionNormal);
		Vector3 object2Impulse = (objectB->GetInverseMass() * j * collisionNormal);

		Vector3 pentrationResolve1 = -(collisionNormal * manifold.points[0].penetrationDepth * objectA->GetInverseMass());
		Vector3 pentrationResolve2 = (collisionNormal * manifold.points[0].penetrationDepth * objectB->GetInverseMass());

		objectBTransform->SetPosition(objectBTransform->GetPosition() + pentrationResolve2);
		objectATransform->SetPosition(objectATransform->GetPosition() + pentrationResolve1);

		//Apply Impulse to the object
		objectA->ApplyImpulse(object1Impulse);
		objectB->ApplyImpulse(object2Impulse);

		objectA->AddRelativeForce(object1Impulse * 20, manifold.points[0].Position);
		objectB->AddRelativeForce(object2Impulse * 20, manifold.points[0].Position);
	}

	manifold = CollisionManifold();
}

void Application::Draw()
{
    //
    // Clear buffers
    //

	float ClearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f }; // red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);
	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    //
    // Setup buffers and render scene
    //
	_pImmediateContext->IASetInputLayout(_pVertexLayout);

	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);

	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);

    ConstantBuffer cb;

	XMFLOAT4X4 viewAsFloats = _camera->GetView();
	XMFLOAT4X4 projectionAsFloats = _camera->GetProjection();

	XMMATRIX view = XMLoadFloat4x4(&viewAsFloats);
	XMMATRIX projection = XMLoadFloat4x4(&projectionAsFloats);

	cb.View = XMMatrixTranspose(view);
	cb.Projection = XMMatrixTranspose(projection);
	
	cb.light = basicLight;
	cb.EyePosW = _camera->GetPosition();

	// Render all scene objects
	for (auto gameObject : _gameObjects)
	{
		// Get render material
		std::shared_ptr<Material> material = gameObject->GetApperance()->GetMaterial();

		// Copy material to shader
		cb.surface.AmbientMtrl = material->ambient;
		cb.surface.DiffuseMtrl = material->diffuse;
		cb.surface.SpecularMtrl = material->specular;

		// Set world matrix
		cb.World = XMMatrixTranspose(gameObject->GetWorldMatrix());

		// Set texture
		if (gameObject->GetApperance()->HasTexture())
		{
			ID3D11ShaderResourceView * textureRV = gameObject->GetApperance()->GetTextureRV();
			_pImmediateContext->PSSetShaderResources(0, 1, &textureRV);
			cb.HasTexture = 1.0f;
		}
		else
		{
			cb.HasTexture = 0.0f;
		}

		// Update constant buffer
		_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

		// Draw object
		gameObject->GetApperance()->Draw(_pImmediateContext);
	}


	_emitter->Draw(_pImmediateContext, _pConstantBuffer, cb);

    //
    // Present our back buffer to our front buffer
    //
    _pSwapChain->Present(0, 0);
}