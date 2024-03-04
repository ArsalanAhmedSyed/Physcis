#pragma once
#include "PhysicsModel.h"

using namespace DirectX;

class RigidBodyModel : public PhysicsModel
{
public:
	RigidBodyModel(Transform* transform);

	void Update(float deltaTime) override;

	void AddRelativeForce(Vector3 force, Vector3 point) override;

private:
	XMFLOAT3X3 _inertiaTensor;
	float _angularDamping{0.5f};
	Vector3 _angularVelocity{};
	Vector3 _torque{};

	void CalculateAngularVelocity(float deltaTime);
	const Vector3 CalculateCrossProduct(Vector3 a, Vector3 b);

};

