#pragma once

#include <directxmath.h>
#include <d3d11_4.h>
#include <string>
#include <memory>

#include "Transform.h"
#include "Apperance.h"
#include "ParticleModel.h"
#include "RigidBodyModel.h"

using namespace DirectX;
using namespace std;

class GameObject
{
public:
	GameObject(string type, std::shared_ptr<Geometry> geometry, std::shared_ptr<Material> material);
	~GameObject();

	string GetType() const { return _type; }

	void SetParent(GameObject * parent) { _parent = parent; }

	void Update(float deltaTime);
	
	Transform* GetTransform()		{ return _transform; }
	std::shared_ptr<Apperance> GetApperance()		{ return _appereance; }

	PhysicsModel* GetPhyicsModel() { return _physicsModel; }
	void SetPhysicsModel(PhysicsModel* physicsType) { _physicsModel = physicsType; }

	XMMATRIX GetWorldMatrix() const { return XMLoadFloat4x4(&_world); }

private:
	string _type;
	XMFLOAT4X4 _world{};

	Vector3 _position{};
	Quaternion _rotation{};
	Vector3 _scale{};

	GameObject * _parent;

	//Components
	Transform* _transform{};
	std::shared_ptr<Apperance> _appereance{};
	PhysicsModel* _physicsModel{};
};

