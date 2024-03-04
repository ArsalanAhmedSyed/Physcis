#pragma once

#include <directxmath.h>
#include <d3d11_4.h>
#include <memory>

#include "Transform.h"
#include "Collider.h"

class PhysicsModel
{
public:
	//, float mass
	PhysicsModel(Transform* transform);

	virtual void Update(float deltaTime);

	void SetVelocity(Vector3 velocity) { _velocity = velocity; }
	Vector3 GetVelocity() const { return _velocity; }
	void SetAcceleration(Vector3 acceleraton) { _acceleration = acceleraton; }

	//Calculate of forces
	void AddForce(Vector3 force) { _netForce += force; }
	Vector3 GravityForce() { return  _gravity * _mass; }
	Vector3 DragFrictionForce();

	//Collider Functions
	bool isCollideable() const { return _collider != nullptr; }
	Collider* GetCollider() const { return _collider; }
	void SetCollider(Collider* collider) { _collider = collider; }

	void ApplyImpulse(Vector3 impluse) { _velocity += impluse; }
	float GetMass() { return _mass; }
	float GetInverseMass() { if (_mass == 0) return 0; return 1.0f / _mass; }
	void SetMass(float mass) { _mass = mass; }

	virtual void AddRelativeForce(Vector3 force, Vector3 point) = 0;

	void SetGravity(bool applyGravity) { _simulateGravity = applyGravity; }

	Vector3 KineticFrictionForce();

protected:
	Transform* _transform{};
	Vector3 _velocity{};
	Vector3 _netForce{};

	Vector3 _acceleration{};
	float _mass{1.0f};

private:
				/* Functions */
	Vector3 AddVelocity(Vector3 velocity);
	Vector3 CalculateVelocity(float deltaTime);
	
				/* Variables */
	bool _keepItConstant{};
	bool _simulateGravity{};
	Vector3 _gravity = Vector3(0,9.81f,0);
	Collider* _collider{};
};

