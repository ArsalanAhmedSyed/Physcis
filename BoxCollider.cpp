#include "BoxCollider.h"
#include <math.h>
#include "Debug.h"
#include "collider.cpp"
#include "PlaneCollider.h"

using namespace std;

//Collision Between *BOX and BOX* collider
bool BoxCollider::CollidesWith(BoxCollider& other,CollisionManifold& out)
{
	this->UpdateAABB();
	other.UpdateAABB();

	//Does it not collider along Z?
	if ((this->_AABBMax.z <= other._AABBMin.z) || (this->_AABBMin.z >= other._AABBMax.z)) return false;

	//Does it not collider along Y?
	if ((this->_AABBMax.y <= other._AABBMin.y) || (this->_AABBMin.y >= other._AABBMax.y)) return false;

	//Does it not collider along X?
	if ((this->_AABBMax.x <= other._AABBMin.x) || (this->_AABBMin.x >= other._AABBMax.x)) return false;

	Vector3 diff =  GetPosition() - other.GetPosition();

	//The direction of the collision
	out.collisionNormal = diff;
	out.collisionNormal.Normalize();
	//Number of Contact made
	out.ContactPointCount = 1;
	//Position of the contact point
	out.points[0].Position = GetPosition() + (out.collisionNormal * _halfExtend.Magnitude());

	//depth of penetration
	out.points[0].penetrationDepth = (other.GetPosition() - this->GetPosition()).Magnitude() - (this->GetHalfExtend().Magnitude());
	
	//Otherwise: no conditions satisified; must be colliding
	return true;
}

//Collision Between *SPHERE and BOX* collider
bool BoxCollider::CollidesWith(SphereCollider& other, CollisionManifold& out)
{
	if (AABBSphereOverlap(*this, other))
	{
		Vector3 diff = this->GetPosition() - other.GetPosition();
		//The direction of the collision
		out.collisionNormal = diff;
		out.collisionNormal.Normalize();
		//Number of Contact made
		out.ContactPointCount = 1;
		//Position of the contact point
		out.points[0].Position = GetPosition() + (out.collisionNormal * other.GetRadius());
		//depth of penetration
		out.points[0].penetrationDepth = (other.GetPosition() - this->GetPosition()).Magnitude() - (this->GetHalfExtend().Magnitude());

		return true;
	}

	return false;
}

//Collision Between *PLANE and BOX* collider
bool BoxCollider::CollidesWith(PlaneCollider& other, CollisionManifold& out)
{
	UpdateAABB();
	other.UpdateAABB();

	//Does it not collider along Z?
	if ((this->_AABBMax.z <= other.GetAABBMin().z) || (this->_AABBMin.z >= other.GetAABBMax().z)) return false;

	//Does it not collider along Y?
	if ((this->_AABBMax.y <= other.GetAABBMin().y) || (this->_AABBMin.y >= other.GetAABBMax().y)) return false;

	//Does it not collider along X?
	if ((this->_AABBMax.x <= other.GetAABBMin().x) || (this->_AABBMin.x >= other.GetAABBMax().x)) return false;

	out.collisionNormal = Vector3(0,1,0);
	out.collisionNormal.Normalize();

	out.ContactPointCount = 1;
	out.points[0].Position = PlaneBoxCollide(this->GetPosition(), other.GetNormal());

	//the 1 represents the halfextend of plane, it is too high causing the cube to fly	
	out.points[0].penetrationDepth = fabs (this->GetPosition().y - this->GetHalfExtend().y);

	return true;
}

void BoxCollider::SetPlainHalfExtend() {}

Vector3 BoxCollider::GetCenterOfObject()
{
	Vector3 vecPos =  _transform->GetPosition();
	Vector3 CenterPos = Vector3(0,0,0) + vecPos;

	return CenterPos;
}

void BoxCollider::UpdateAABB()
{
	_AABBMin = GetCenterOfObject() - _halfExtend;
	_AABBMax = GetCenterOfObject() + _halfExtend;
}
