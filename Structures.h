#pragma once
#include <cstring>
#include <d3d11.h>
#include <directxmath.h>
#include "Vector3.h"

using namespace DirectX;

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 TexC;

	bool operator<(const SimpleVertex other) const
	{
		return memcmp((void*)this, (void*)&other, sizeof(SimpleVertex)) > 0;
	};
};

struct MeshData
{
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;
	UINT VBStride;
	UINT VBOffset;
	UINT IndexCount;
};

struct SurfaceInfo
{
	XMFLOAT4 AmbientMtrl;
	XMFLOAT4 DiffuseMtrl;
	XMFLOAT4 SpecularMtrl;
};

struct Light
{
	XMFLOAT4 AmbientLight;
	XMFLOAT4 DiffuseLight;
	XMFLOAT4 SpecularLight;

	float SpecularPower;
	XMFLOAT3 LightVecW;
};

struct ConstantBuffer
{
	XMMATRIX World;
	XMMATRIX View;
	XMMATRIX Projection;

	SurfaceInfo surface;

	Light light;

	XMFLOAT3 EyePosW;
	float HasTexture;
};

struct Geometry
{
	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;
	int numberOfIndices;

	UINT vertexBufferStride;
	UINT vertexBufferOffset;

	Geometry() {};

	~Geometry() 
	{
		if (vertexBuffer == nullptr) { vertexBuffer->Release(); }
		if (indexBuffer == nullptr) { indexBuffer->Release(); }
	}
};

struct Material
{
	XMFLOAT4 diffuse;
	XMFLOAT4 ambient;
	XMFLOAT4 specular;
	float specularPower;
};

struct ContactPoint
{
	Vector3 Position{};
	float penetrationDepth;
};

struct CollisionManifold
{
	int ContactPointCount = 0;
	ContactPoint points[4];
	Vector3 collisionNormal;
};
