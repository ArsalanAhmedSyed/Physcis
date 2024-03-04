#include "PlaneCollider.h"
#include "Collider.cpp"
#include "Debug.h"

PlaneCollider::PlaneCollider(Transform* transform, Vector3 normal) : Collider(transform) 
{ 
	_normal = normal; 
	_normal.Normalize();

	_halfExtend.x *= 15;
	_halfExtend.y *= 0.5;
	_halfExtend.z *= 15;
}

bool PlaneCollider::CollidesWith(SphereCollider& other, CollisionManifold& out)
{
	Vector3 normal = Vector3(0,1,0);
	normal.Normalize();

	float distance = ((this->GetPosition() - other.GetPosition()) * normal) * -1;
	Vector3 contactPoint = this->GetPosition() - distance * normal;
	Vector3 diff = this->GetPosition() - other.GetPosition();

	if (distance <= other.GetRadius())
	{
		//Pass in negative normal
		out.collisionNormal = -normal;

		//Number of Contact made
		out.ContactPointCount = 1;

		//Position of the contact point
		out.points[0].Position = contactPoint;

		//depth of penetration
		out.points[0].penetrationDepth = fabs(diff.y - (this->GetHalfExtend().y - other.GetRadius()));

		return true;
	}

	return false;
}

bool PlaneCollider::CollidesWith(BoxCollider& other, CollisionManifold& out)
{
	UpdateAABB();
	other.UpdateAABB();

	//Does it not collider along Z?
	if ((this->_AABBMax.z <= other.GetAABBMin().z) || (this->_AABBMin.z >= other.GetAABBMax().z)) return false;

	//Does it not collider along Y?
	if ((this->_AABBMax.y <= other.GetAABBMin().y) || (this->_AABBMin.y >= other.GetAABBMax().y)) return false;

	//Does it not collider along X?
	if ((this->_AABBMax.x <= other.GetAABBMin().x) || (this->_AABBMin.x >= other.GetAABBMax().x)) return false;

	//Pass in negative normal
	out.collisionNormal = - _normal;

	out.collisionNormal.Normalize();
	out.ContactPointCount = 1;
	out.points[0].Position = PlaneBoxCollide(other.GetPosition(), this->GetPosition());

	//the 1 represents the halfextend of plane, it is too high causing the cube to fly	
	out.points[0].penetrationDepth = fabs(other.GetPosition().y - other.GetHalfExtend().y);

	return true;
}

Vector3 PlaneCollider::GetCenterOfObject()
{
	Vector3 vecPos = _transform->GetPosition();
	Vector3 CenterPos = Vector3(0, 0, 0) + vecPos;

	return CenterPos;
}

void PlaneCollider::UpdateAABB()
{
	_AABBMin = GetCenterOfObject() - _halfExtend;
	_AABBMax = GetCenterOfObject() + _halfExtend;
}

