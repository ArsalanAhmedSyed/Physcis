#include "GameObject.h"
#include "Debug.h"
GameObject::GameObject(string type, std::shared_ptr<Geometry> geometry, std::shared_ptr<Material> material) : _type(type)
{
	_parent = nullptr;
	_transform = new Transform();
	_appereance = std::make_shared<Apperance>(geometry, material);
	//_physicsModel = std::make_unique<RigidBodyModel>(_transform);
}

GameObject::~GameObject()
{
	_parent = nullptr;
}

void GameObject::Update(float deltaTime)
{
	_scale = _transform->GetScale();
	_rotation = _transform->GetOrientation();
	_position = _transform->GetPosition();

	// Calculate world matrix
	XMMATRIX scale = XMMatrixScaling(_scale.x, _scale.y, _scale.z);
	XMMATRIX rotation = XMMatrixRotationQuaternion(XMVectorSet(_rotation.v.x, _rotation.v.y, _rotation.v.z,_rotation.n));
	XMMATRIX translation = XMMatrixTranslation(_position.x, _position.y, _position.z);

	XMStoreFloat4x4(&_world, scale * rotation * translation);

	if (_parent != nullptr)
	{
		XMStoreFloat4x4(&_world, this->GetWorldMatrix() * _parent->GetWorldMatrix());
	}

	//_particleModel->Update(deltaTime);
	_physicsModel->Update(deltaTime);
}
