#pragma once
#include <vector>
#include <memory>
#include <d3d11_1.h>
#include "GameObject.h"
#include "Apperance.h"
#include "Structures.h"

class Emitter 
{
public:

	Emitter(Vector3 position, int numberOfParticles, std::shared_ptr<Geometry> geometry,
		std::shared_ptr<Material> material, ID3D11ShaderResourceView* textureRV);

	void Draw(ID3D11DeviceContext* immediateContext, ID3D11Buffer* constantBuffer, ConstantBuffer& cb); 
	void Update(float deltaTime);

private:
	Vector3 position;
	std::vector<GameObject*> _particles;
};