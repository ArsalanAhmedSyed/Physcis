#pragma once

#include <windows.h>
#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <vector>
#include "Loaders\DDSTextureLoader.h"
#include "Loaders\OBJLoader.h"
#include "resource.h"
#include "Camera.h"
#include "Structures.h"
#include "GameObject.h"
#include "Vector3.h"
#include "Timer.h"
#include "Debug.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "PlaneCollider.h"
#include "Emitter.h"

using namespace DirectX;

class Application
{
private:
	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
	ID3D11Device*           _pd3dDevice;
	ID3D11DeviceContext*    _pImmediateContext;
	IDXGISwapChain*         _pSwapChain;
	ID3D11RenderTargetView* _pRenderTargetView;
	ID3D11VertexShader*     _pVertexShader;
	ID3D11PixelShader*      _pPixelShader;
	ID3D11InputLayout*      _pVertexLayout;

	ID3D11Buffer*           _pVertexBuffer;
	ID3D11Buffer*           _pIndexBuffer;

	ID3D11Buffer*           _pPlaneVertexBuffer;
	ID3D11Buffer*           _pPlaneIndexBuffer;

	ID3D11Buffer*           _pConstantBuffer;

	ID3D11DepthStencilView* _depthStencilView = nullptr;
	ID3D11Texture2D* _depthStencilBuffer = nullptr;

	ID3D11ShaderResourceView * _pTextureRV = nullptr;
	ID3D11ShaderResourceView * _pGroundTextureRV = nullptr;

	ID3D11SamplerState * _pSamplerLinear = nullptr;

	MeshData objMeshData;

	Light basicLight;

	vector<GameObject *> _gameObjects;

	Camera * _camera = nullptr;
	float _cameraOrbitRadius = 7.0f;
	float _cameraOrbitRadiusMin = 2.0f;
	float _cameraOrbitRadiusMax = 50.0f;
	float _cameraOrbitAngleXZ = -90.0f;
	float _cameraSpeed = 0.05f;

	UINT _WindowHeight;
	UINT _WindowWidth;

	// Render dimensions - Change here to alter screen resolution
	UINT _renderHeight = 1080;
	UINT _renderWidth = 1920;

	ID3D11DepthStencilState* DSLessEqual;
	ID3D11RasterizerState* RSCullNone;

	ID3D11RasterizerState* CCWcullMode;
	ID3D11RasterizerState* CWcullMode;

	Timer* _timer{};
	float accumulator{};

	

private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();
	HRESULT InitVertexBuffer();
	HRESULT InitIndexBuffer();

	void moveForward(int objectNumber);
	void moveBackward(int objectNumber);
	void moveUp(int objectNumber);
	void moveDown(int objectNumber);
	void moveLeft(int objectNumber);
	void moveRight(int objectNumber);

	//object that is being controlled from the gameobject vector list
	int _controllingObject{1};

	//To get objects from the gameobjet Vector list
	int _cubeA{ 1 };
	int _cubeB{ 2 };
	int _plane{ 0 };

	//To spawn aprticles
	Emitter* _emitter;

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	bool HandleKeyboard(MSG msg);

	void Update();

	//Collision resolve 
	void ResolvePlaneCollisions(CollisionManifold& manifold, int objectOne, int objectTwo);
	void ResolveCollisions(CollisionManifold& manifold, int objectOne, int objectTwo);

	void Draw();
};

