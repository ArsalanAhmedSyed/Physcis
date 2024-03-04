#include "SphereCollider.h"
#include "PlaneCollider.h"
#include "Collider.cpp"
#include "Debug.h"

bool SphereCollider::CollidesWith(SphereCollider& other,CollisionManifold& out)
{
	//Add up the raidus
	float radiiSum = other.GetRadius() + this->_radius;
	float distanceInBetween = GetDistanceInBetween(other.GetPosition());
	Vector3 diff =  this->GetPosition() - other.GetPosition();

	// check if the position is less then the combined radius 
	if ( distanceInBetween <= radiiSum)
	{
		//The direction of the collision
		out.collisionNormal = diff;
		out.collisionNormal.Normalize();
		DebugPrintF("Collision normal = %f, %f, %f \n", out.collisionNormal.x, out.collisionNormal.y, out.collisionNormal.z);
		//Number of Contact made
		out.ContactPointCount = 1;
		//Position of the contact point
		out.points[0].Position = GetPosition() + (out.collisionNormal * GetRadius());
		//depth of penetration
		out.points[0].penetrationDepth = (other.GetPosition() - this->GetPosition()).Magnitude() - ( this->GetRadius());
		
		return true;
	}
		
    return false;
}

bool SphereCollider::CollidesWith(BoxCollider& other,CollisionManifold& out)
{
	if (AABBSphereOverlap(other, *this))
	{
		Vector3 diff = this->GetPosition() - other.GetPosition();
		//The direction of the collision
		out.collisionNormal = diff;
		out.collisionNormal.Normalize();
		//DebugPrintF("Collision normal = %f, %f, %f \n", out.collisionNormal.x, out.collisionNormal.y, out.collisionNormal.z);
		//Number of Contact made
		out.ContactPointCount = 1;
		//Position of the contact point
		out.points[0].Position = this->GetPosition() + (out.collisionNormal * GetRadius());
		//depth of penetration
		out.points[0].penetrationDepth = (other.GetPosition() - this->GetPosition()).Magnitude() - (other.GetHalfExtend().Magnitude()); 

		return true;
	}

	return false;
}

bool SphereCollider::CollidesWith(PlaneCollider& other, CollisionManifold& out)
{
	Vector3 normal = other.GetNormal();
	normal.Normalize();

	float distance = (this->GetPosition() - other.GetPosition()) * normal;
	Vector3 contactPoint = GetPosition() - distance * normal;
	Vector3 diff = this->GetPosition() - other.GetPosition();

	if (distance <= this->GetRadius())
	{
		//The direction of the collision
		out.collisionNormal = normal;

		//Number of Contact made
		out.ContactPointCount = 1;

		//Position of the contact point
		out.points[0].Position = contactPoint;

		//depth of penetration
		out.points[0].penetrationDepth = fabs(diff.y - this->GetRadius());

		return true;
	}

	return false;
}

float SphereCollider::GetDistanceInBetween(const Vector3& otherPos)
{
	Vector3 objectAPos = this->GetPosition();
	float xPos = otherPos.x - objectAPos.x;
	float yPos = otherPos.y - objectAPos.y;
	float zPos = otherPos.z - objectAPos.z;
	float squareValue = pow((xPos), 2) + pow(yPos, 2) + pow(zPos, 2);
	return squareValue;//sqrt(squareValue);
}
