#include "ParticleModel.h"
#include <algorithm>

ParticleModel::ParticleModel(Transform* transfrom) : PhysicsModel(transfrom){}

ParticleModel::ParticleModel(Transform* transform, float resetTime, Vector3 postion, bool invertGravity) :
	_transform(transform), _resetTime(resetTime), _pertubation(postion), _invertGravity(invertGravity), PhysicsModel(transform)
{
	_transform->SetPosition(postion);
	_startPosition = transform->GetPosition();
}

void ParticleModel::Update(float deltaTime)
{
	_timeAlive += deltaTime * 100;
	if (_timeAlive > _resetTime)
	{
		Reset();
	}

	PhysicsModel::Update(deltaTime);
}

void ParticleModel::Reset()
{
	_transform->SetPosition(_startPosition);
	_timeAlive = 0.0f;
}

void ParticleModel::AddRelativeForce(Vector3 force, Vector3 point)
{
	AddForce(force);
}
