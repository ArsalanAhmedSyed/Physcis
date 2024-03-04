#pragma once
#include "PhysicsModel.h"

class ParticleModel : public PhysicsModel 
{
public:
	ParticleModel(Transform* transfrom);
	ParticleModel(Transform* transform, float resetTime, Vector3 pertubation, bool invertGravity);

	void Update(float deltaTime) override;
	void Reset();

	void AddRelativeForce(Vector3 force, Vector3 point) override;
private:
	Vector3 _startPosition{};
	Transform* _transform{};
	Vector3 _pertubation{};

	//Allow the particle to go upwards or downwards
	bool _invertGravity{};

	//Time it is alive for
	float _timeAlive{0.0f};
	//Time at which the particle should reset
	float _resetTime{};
};

