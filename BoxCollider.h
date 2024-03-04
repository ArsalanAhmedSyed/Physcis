#pragma once
#include "Collider.h"
#include "Vector3.h"
#include "PhysicsModel.h"

class BoxCollider : public Collider
{
public:
	BoxCollider(Transform* transform, PhysicsModel* physicsModel) : Collider(transform), _physicsModel(physicsModel) {}
	virtual bool CollidesWith(Collider& other, CollisionManifold& out) override { return other.CollidesWith(*this, out); }
	virtual bool CollidesWith(SphereCollider& other,CollisionManifold& out) override;
	virtual bool CollidesWith(BoxCollider& other, CollisionManifold& out) override;
	virtual bool CollidesWith(PlaneCollider& other, CollisionManifold& out) override;

	//Getters
	PhysicsModel* GetPhysics() { return _physicsModel; }
	Vector3 GetHalfExtend() { return _halfExtend; }
	Vector3 GetAABBMin() { return _AABBMin; }
	Vector3 GetAABBMax() { return _AABBMax; }
	void UpdateAABB();

private:
	Vector3 GetCenterOfObject();
	void SetPlainHalfExtend() override;

	Vector3 _halfExtend = Vector3(1,1,1);
	Vector3 _AABBMin;
	Vector3 _AABBMax;

	PhysicsModel* _physicsModel;
};

