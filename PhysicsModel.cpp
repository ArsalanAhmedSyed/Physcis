#include "PhysicsModel.h"
#include "Debug.h"
#include "SphereCollider.h"
#include "BoxCollider.h"

PhysicsModel::PhysicsModel(Transform* transform) : _transform(transform) //, _mass(mass)
{
	_velocity = Vector3(0, 0, 0);
	_acceleration = Vector3(0,0,0);
	_keepItConstant = false;
	_netForce = Vector3(0, 0, 0);
	_simulateGravity = false;
}

void PhysicsModel::Update(float deltaTime)
{
	if (_mass == 0) { return; }

	Vector3 DragForce = DragFrictionForce();
	Vector3 KineticForcec = KineticFrictionForce();

	if (_simulateGravity)
	{
		_netForce -= GravityForce();
	}

	_netForce += DragForce ;
	_netForce += KineticForcec;

	_acceleration = _netForce / _mass;
	_velocity += _acceleration * deltaTime;
	_transform->SetPosition(AddVelocity(_velocity));

	_netForce = Vector3(0, 0, 0);

	if (_keepItConstant == false) { return; }

	Vector3 velXAccel = CalculateVelocity(deltaTime);
	_transform->SetPosition(AddVelocity(velXAccel));
}

Vector3 PhysicsModel::CalculateVelocity(float deltaTime)
{
	
	return _velocity;
}

Vector3 PhysicsModel::AddVelocity(Vector3 velocity)
{
	Vector3 position = _transform->GetPosition();
	return position += _velocity;
}

Vector3 PhysicsModel::DragFrictionForce()
{
	const float airDens = 1.225;
	float magnitude = _velocity.Magnitude() * _velocity.Magnitude();
	const float scale = _transform->GetScale() * _transform->GetScale(); //sugestion sum the vector /3

	float dragCalc = 0.5f * airDens * 1.05 * 1 * magnitude;

	Vector3 temp = _velocity;
	Vector3 force = temp * dragCalc * -1;

	return force;
};

Vector3 PhysicsModel::KineticFrictionForce()
{
	float coefficent = 0.5f;
	Vector3 velocity = _velocity;
	Vector3 friction;

	if (_transform->GetPosition().y <= 2)
	{
		float N = _mass * _gravity.y;
		friction.x = coefficent * velocity.x * N;
		friction.z = coefficent * velocity.z * N;
	}

	return friction * -1;
}