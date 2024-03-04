#pragma once
#include "Collider.h"

class PlaneCollider : public Collider
{
public:
	PlaneCollider(Transform* transform, Vector3 normal);
	virtual bool CollidesWith(Collider& other, CollisionManifold& out) override { return other.CollidesWith(*this, out); }
	virtual bool CollidesWith(SphereCollider& other, CollisionManifold& out) override;
	virtual bool CollidesWith(BoxCollider& other, CollisionManifold& out) override;
	virtual bool CollidesWith(PlaneCollider& other, CollisionManifold& out) override { return false; }

	Vector3 GetHalfExtend() { return _halfExtend; }
	Vector3 GetAABBMin() { return _AABBMin; }
	Vector3 GetAABBMax() { return _AABBMax; }
	void UpdateAABB();

	Vector3 GetNormal() { return _normal; }

private:
	Vector3 _normal;

	Vector3 GetCenterOfObject();
	void SetPlainHalfExtend() override {};

	//Set
	Vector3 _halfExtend = Vector3(15.f, 0.5f, 1.f);
	Vector3 _AABBMin{};
	Vector3 _AABBMax{};
};

