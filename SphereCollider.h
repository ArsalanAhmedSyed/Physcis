#pragma once
#include <math.h>

#include "Collider.h"
#include "Vector3.h"
#include "PhysicsModel.h"

class SphereCollider : public Collider
{
public:
	SphereCollider(Transform* transform, PhysicsModel* physicsModel) : Collider(transform), _physicsModel(physicsModel) {}

	virtual bool CollidesWith(Collider& other, CollisionManifold& out) override { return other.CollidesWith(*this, out); }
	virtual bool CollidesWith(SphereCollider& other,CollisionManifold& out) override;
	virtual bool CollidesWith(BoxCollider& other,CollisionManifold& out) override;
	virtual bool CollidesWith(PlaneCollider& other, CollisionManifold& out) override;


	virtual void SetPlainHalfExtend() override {};

	float GetRadius() const { return _radius; }
	PhysicsModel* GetPhysics() { return _physicsModel; }
private:

	PhysicsModel* _physicsModel;

	float _radius{1.5f};
	float GetDistanceInBetween(const Vector3& transform);
};

